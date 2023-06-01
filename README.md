# Wheeler-language-recognizer

## Download and Compile

1 - git clone https://github.com/regindex/Wheeler-language-recognizer.git
2 - git submodule update --init --recursive
3 - make -C external/DFA-suffix-doubling/
4 - make -C external/DFAgen-suffixdoubling/
5 - g++ external/dfaMinimizationComparison/Cpp/Modified/Modified.cpp -o external/dfaMinimizationComparison/Cpp/Modified/minimizer.x
6 - cp external/utils.py external/re_to_dfa/src/re_to_dfa/
7 - git -C external/WNFA-partition-refinement/ submodule update --init --recursive
8 - mkdir external/WNFA-partition-refinement/build
9 - cd external/WNFA-partition-refinement/build/
10 - cmake ..
11 - cd ../../../
12 - make -C external/WNFA-partition-refinement/build/
13 - mkdir build
14 - cd build
15 - cmake ..
16 - make 

## Usage example 

1 - python3 wheeler_lan_rec.py "a(ac*a)|(bc*b)"
2 - python3 wheeler_lan_rec.py --DFA input.dfa 
