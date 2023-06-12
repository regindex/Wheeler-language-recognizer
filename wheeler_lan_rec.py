#!/usr/bin/env python3

import sys, time, argparse, subprocess, os.path
from pathlib import Path
cwd = Path(__file__).parent.absolute()
sys.path.insert(1, str(cwd)+'/external/re_to_dfa/src/')
from re_to_dfa import *


Description = """
Tool to check if a regular expression recognizes a Wheeler language.
"""

dirname = os.path.dirname(os.path.abspath(__file__))
dfamin_exe    =  "external/dfaMinimizationComparison/Cpp/Modified/minimizer.x"
convform_exe  =  "external/DFA-suffix-doubling/convert-format"
pruning_exe   =  "build/prune.x"
partref_exe   =  "external/finite-automata-partition-refinement/build/part-ref32.x"
doublingmerge_exe =  "external/DFAgen-suffixdoubling/suffix-doubling-on-pruned-graphs"
recognizer_exe = "build/recognizer.x"

def main():
    parser = argparse.ArgumentParser(description=Description, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('input', help='input regex', type=str)
    parser.add_argument('--DFA', help='take in input a DFA instead of a regexp', action='store_true')
    #parser.add_argument('--verbose',  help='verbose mode on (def. False)',action='store_true')
    args = parser.parse_args()

    logfile_name = args.input + ".log"
    if( not args.DFA ):
        logfile_name = "data/regexp.log"
    # get main directory
    args.main_dir = os.path.split(sys.argv[0])[0]
    print("Sending logging messages to file:", logfile_name)
    
    with open(logfile_name,"a") as logfile:

        if( not args.DFA ):

            dfa = re_to_dfa(args.input)

            ttable = dfa.transition_table;

            dfa_text = "0 36 1\n"
            max_state = 0
            no_edges = 1
            no_finalstates = 0

            # write DFA

            for i in ttable.items():
                source = int(i[0][1:]) + 1
                if( source > max_state ):
                    max_state = source
                for j in i[1].items():
                    dest = int(j[1][1:]) + 1
                    if( dest > max_state ):
                        max_state = dest

                    dfa_text = dfa_text + str(source) + " "+ str(ord(j[0])) + " " + str(dest) + "\n"
                    no_edges += 1

            for i in dfa.finalstates:

                dfa_text = dfa_text + str(int(i[1:])+1) + "\n"
                no_finalstates += 1

            print("max state: " +str(max_state))

            with open("data/regexp.dfa",'w') as out:
                out.write(str(max_state+1) + " " + str(no_edges) + " " + "0" + " " + str(no_finalstates) + "\n")
                out.write(dfa_text)

            dfa_text = ""

            #args.input += ".dfa"
            args.input = "data/regexp.dfa"

        start0 = start = time.time()

        ############################################

        command = "{exe} -in < {input} > {output}".format(exe=dfamin_exe, input=args.input, output=args.input+".min")
        print("==== computing minimum DFA. Command: ", command)

        with open(args.input, 'rb', 0) as a, open(args.input+".min", 'w') as b:
            rc = subprocess.call(dfamin_exe, stdin=a, stdout=b)

        print("Elapsed time: {0:.4f}".format(time.time()-start))

        ############################################

        start = time.time()

        command = "{exe} {input} {mindfa} {maxdfa}".format(exe=pruning_exe, input=args.input+".min", mindfa=args.input+".prmin", maxdfa=args.input+".prmax")

        print("==== pruning minimum DFA. Command: ", command)
        if(execute_command(command,logfile,logfile_name)!=True):
            return
        print("Elapsed time: {0:.4f}".format(time.time()-start))

        ############################################

        start = time.time()

        no_nodes = 0
        source = 0
        with open(args.input+".prmin", "rb") as file:
            try:
                file.seek(-2, os.SEEK_END)
                while file.read(1) != b'\n':
                    file.seek(-2, os.SEEK_CUR) 
            except OSError:
                file.seek(0)
            last_line = file.readline().decode()
            no_nodes = int(last_line.split(" ")[0])
            source = int(last_line.split(" ")[2])

        command = "{exe} {input} {output} {nodes} {source} 0 1 0 1 0 1".format(exe=partref_exe, input=args.input+".prmin", output=args.input+".infima", nodes=no_nodes, source=source)

        print("==== compute infima strings DFA. Command: ", command)
        if(execute_command(command,logfile,logfile_name)!=True):
            return

        command = "{exe} {input} {output} {nodes} {source} 0 1 1 1 0 1".format(exe=partref_exe, input=args.input+".prmax", output=args.input+".suprema", nodes=no_nodes, source=source)

        print("==== compute suprema strings DFA. Command: ", command)
        if(execute_command(command,logfile,logfile_name)!=True):
            return

        print("Elapsed time: {0:.4f}".format(time.time()-start))

        ############################################

        start = time.time()

        command = "{exe} {input1} {input2}".format(exe=doublingmerge_exe, input1=args.input+".infima", input2=args.input+".suprema")
        print("==== merging infima and suprema pruned DFAs. Command: ", command)

        with open(args.input+".interval", 'w') as b:
            rc = subprocess.call(command.split(), stdout=b)

        print("Elapsed time: {0:.4f}".format(time.time()-start))

        ############################################

        start = time.time()

        command = "{exe} {dfa} {interval}".format(exe=recognizer_exe, dfa=args.input+".min", interval=args.input+".interval")

        print("==== compute A^2 pruned automaton and check language Wheelerness. Command: ", command)
        if(execute_command(command,logfile,logfile_name)!=True):
            return

        print("Elapsed time: {0:.4f}".format(time.time()-start))

        with open("answer", 'rb', 0) as a:
            answer = a.readline().decode()
            if(answer == "1"):
                print("*** The language recognized by " + args.input + " is Wheeler!")
            else:
                print("*** The language recognized by " + args.input + " is NOT Wheeler!")

        ############################################

        print("### Total elapsed time: {0:.4f}".format(time.time()-start0))


# execute command: return True is everything OK, False otherwise
def execute_command(command,logfile,logfile_name,env=None):
  try:
    #subprocess.run(command.split(),stdout=logfile,stderr=logfile,check=True,env=env)
    subprocess.check_call(command.split(),stdout=logfile,stderr=logfile,env=env)
  except subprocess.CalledProcessError:
    print("Error executing command line:")
    print("\t"+ command)
    print("Check log file: " + logfile_name)
    return False
  return True


if __name__ == '__main__':
    main()