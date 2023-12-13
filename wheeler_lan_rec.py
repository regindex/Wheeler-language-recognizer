#!/usr/bin/env python3

import sys, time, argparse, subprocess, os.path

Description = """
Tool to check if a regular expression or a DFA recognizes a Wheeler language.
"""

dirname = os.path.dirname(os.path.abspath(__file__))
dfamin_exe    =  "external/dfaMinimizationComparison/Cpp/Modified/minimizer.x"
convform_exe  =  "external/DFA-suffix-doubling/convert-format"
pruning_exe   =  "build/prune.x"
partref_exe   =  "external/finite-automata-partition-refinement/build/part-ref32.x"
doublingmerge_exe =  "external/DFAgen-suffixdoubling/suffix-doubling-on-pruned-graphs"
reg_to_dfa_exe =  "external/RegexpToAutomaton/regToAutomaton"
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

        start0 = start = time.time()

        if( not args.DFA ):

            # add dollar to regexp
            args.input = "($)" + "(" + args.input + ")"
            print("Computing the minimum DFA of the regexp")
            # compute minimum DFA
            command = "{exe} {regexp} --DFAmin".format(exe=reg_to_dfa_exe,regexp=args.input)
            with open("data/regexp.mdfa","w+") as dfa_file:
                subprocess.call(command.split(), stdout=dfa_file)
            # set the resulting DFA as the input
            args.input = "data/regexp.mdfa"

            print("Elapsed time: {0:.4f}".format(time.time()-start))

        ############################################

        if( args.DFA ):

            start = time.time()

            # compute the minimum dfa        
            command = "{exe} -in < {input} > {output}".format(exe=dfamin_exe, input=args.input, output=args.input+".min")
            print("==== computing minimum DFA. Command: ", command)

            with open(args.input, 'rb', 0) as a, open(args.input+".min", 'w') as b:
                rc = subprocess.call(dfamin_exe, stdin=a, stdout=b)

            print("Elapsed time: {0:.4f}".format(time.time()-start))
            args.input = args.input+".min"

        ############################################

        start = time.time()

        command = "{exe} {input} {mindfa} {maxdfa}".format(exe=pruning_exe, input=args.input, mindfa=args.input+".prmin", maxdfa=args.input+".prmax")

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

        command = "{exe} {dfa} {interval}".format(exe=recognizer_exe, dfa=args.input, interval=args.input+".interval")

        print("==== compute A^2 pruned automaton and check language Wheelerness. Command: ", command)
        if(execute_command(command,logfile,logfile_name)!=True):
            return

        print("Elapsed time: {0:.4f}".format(time.time()-start))

        with open("answer", 'rb', 0) as a:
            answer = a.readline().decode()
            if(answer == "1"):
                print("*** The language recognized by the DFA in " + args.input + " is Wheeler!")
            else:
                print("*** The language recognized by the DFA in " + args.input + " is NOT Wheeler!")

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