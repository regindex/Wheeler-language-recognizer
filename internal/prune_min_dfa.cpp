// include partition refinement algorithm

#include <cstdint>
#include <cassert>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>

void tokenize(std::string const &str, const char delim, 
            std::vector<std::string> &out) 
{ 
    // construct a stream from the string 
    std::stringstream ss(str); 

    // clear out vector
    out.clear();
    
    // segment str using delim
    std::string s; 
    while (std::getline(ss, s, delim)) { 
        out.push_back(s); 
    } 
}

// simple parser for intermediate file; it reads line by line origin \t destination \t label \n
void prune_dfa(std::string input_file, std::string min_dfa_file, std::string max_dfa_file)
{
    // open stream to input
    std::ifstream input(input_file);
    std::string line;
    const char delim = ' '; 
    size_t nodes, edges, source, accepting, dest;
    char label;
    std::vector<std::string> out; 

    std::getline(input, line);
    tokenize(line, delim, out); 

    assert(out.size() == 4);

    nodes = std::stoull(out[0]);
    edges = std::stoull(out[1]);
    source = std::stoull(out[2]);
    accepting = std::stoull(out[3]);

    std::cout << "Nodes: " << nodes << "\n";
    std::cout << "Edges: " << edges << "\n";
    std::cout << "Source: " << source << "\n";
    std::cout << "No. acc. states: " << accepting << "\n";
    
    std::vector<char> vmin(nodes, 127);
    std::vector<char> vmax(nodes, 0);

    for(size_t i=0;i<edges;++i)
    {
        std::getline(input, line);
        tokenize(line, delim, out);

        assert(out.size() == 3);
        
        dest = std::stoull(out[2]);
        label = std::stoull(out[1]);

        if( vmin[dest] > label ){ vmin[dest] = label; }
        if( vmax[dest] < label ){ vmax[dest] = label; }
    }

    input.clear();                
    input.seekg(0, std::ios::beg); 

    std::ofstream min_dfa;
    std::ofstream max_dfa;

    min_dfa.open(min_dfa_file);
    max_dfa.open(max_dfa_file);

    size_t edge_min = 0; 
    size_t edge_max = 0;

    std::getline(input, line);
    for(size_t i=0;i<edges;++i)
    {
        std::getline(input, line);
        tokenize(line, delim, out);

        assert(out.size() == 3);

        dest = std::stoull(out[2]);
        label = std::stoull(out[1]);

        if( vmin[dest] == label )
        { 

            if( label <= 36 )
            {
                min_dfa << std::stoull(out[0]) << " " << dest << " " <<  char(label+36) << "\n";
            }
            else{ min_dfa << std::stoull(out[0]) << " " << dest << " " <<  char(label) << "\n"; }
            edge_min++; 
        }

        if( vmax[dest] == label )
        { 
            if( label <= 36 )
            {
                max_dfa << std::stoull(out[0]) << " " << dest << " " <<  char(label+36) << "\n";
            }
            else{ max_dfa << std::stoull(out[0]) << " " << dest << " " <<  char(label) << "\n"; }
            edge_max++;
        }
        
    }

    min_dfa << nodes << " " <<  edge_min << " " << source << " " << accepting << "\n";
    max_dfa << nodes << " " <<  edge_max << " " << source << " " << accepting << "\n";

    min_dfa.close();
    max_dfa.close();

    // close stream to input file
    input.close();
}  
   
int main(int argc, char** argv)
{
    // read command line arguments
    std::string out_file_min, out_file_max, in_file;
    // read input from file
    if(argc > 3)
    { 
        // set input parameters
        in_file = std::string(argv[1]);
        out_file_min = std::string(argv[2]);
        out_file_max = std::string(argv[3]);

        prune_dfa(in_file, out_file_min, out_file_max); 

    }
    else
    {
        std::cerr << "invalid no. arguments\n";
        std::cerr << "Format your command as follows: ./prune.x input.dfa out.min out.max\n";
        exit(1);
    }

  return 0;
}