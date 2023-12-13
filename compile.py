#!/usr/bin/env python3

import sys, subprocess, os

print("###### Compiling DFA-suffix-doubling...")
subprocess.call("make -C external/DFA-suffix-doubling/".split())

print("###### Compiling DFAgen-suffixdoubling...")
subprocess.call("make -C external/DFAgen-suffixdoubling/".split())

print("###### Compiling dfaMinimizationComparison...")
subprocess.call("g++ external/dfaMinimizationComparison/Cpp/Modified/Modified.cpp -o external/dfaMinimizationComparison/Cpp/Modified/minimizer.x".split())

print("###### Compiling finite-automata-partition-refinement...")
subprocess.call("mkdir external/finite-automata-partition-refinement/build".split())
os.chdir("external/finite-automata-partition-refinement/build")
subprocess.call("cmake ..".split())
subprocess.call("make".split())
os.chdir("../../../")

print("###### Compiling RegexpToAutomaton...")
subprocess.call("make -C external/RegexpToAutomaton/".split())

print("###### Compiling Wheeler-language-recognizer...")
subprocess.call("mkdir build".split())
os.chdir("build")
subprocess.call("cmake ..".split())
subprocess.call("make".split())