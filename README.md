# Wheeler-language-recognizer

### Description

This is a tool to check if a regular expression recognizes a Wheeler language.

### Requirements

* A modern C++17 compiler such as `g++` version 8.4 or higher.
* The cmake build system, version \geq 3.10.
* A Linux 64-bit operating system.

### Download

To clone the repository, run:

```console
git clone https://github.com/regindex/Wheeler-language-recognizer.git
git submodule update --init --recursive
```

### Compile

You can compile using cmake and make:

```console
make -C external/DFA-suffix-doubling/
make -C external/DFAgen-suffixdoubling/
g++ external/dfaMinimizationComparison/Cpp/Modified/Modified.cpp -o external/dfaMinimizationComparison/Cpp/Modified/minimizer.x
cp external/utils.py external/re_to_dfa/src/re_to_dfa/
mkdir external/finite-automata-partition-refinement/build
cd external/finite-automata-partition-refinement/build/
cmake ..
cd ../../../
make -C external/finite-automata-partition-refinement/build/
mkdir build
cd build
cmake ..
make 
```

### Input

The tool takes in input a file containing in the first line the number of states, number of edges, source and number of accepting states of the input DFA. The rest of the file contains an edge per line (origin - label - destination) and a list of accepting states separated by newline characters (see below for an example).
```
9 12 0 2
0 0 1
1 1 2
2 1 3
2 2 4
3 1 5
3 3 6
4 2 7
4 3 8
6 3 6
6 1 5
8 3 8
8 2 7
5
7
```

### Usage

```
usage: wheeler_lan_rec.py [-h] [--DFA] input

Tool to check if a regular expression recognizes a Wheeler language.

positional arguments:
  input       input regex

optional arguments:
  -h, --help  show this help message and exit
  --DFA       take in input a DFA instead of a regexp
```

### Run on example data

```console
// take in input a regular expression
python3 wheeler_lan_rec.py "a(ac*a)|(bc*b)"
// take in input a DFA
python3 wheeler_lan_rec.py --DFA data/regexp.dfa 
```

# External resources

* [malloc_count](https://github.com/bingmann/malloc_count)

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation programme, project REGINDEX, grant agreement No 101039208