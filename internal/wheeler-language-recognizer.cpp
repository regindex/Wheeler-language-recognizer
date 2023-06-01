// include DFA automaton implementation
#include "automaton.hpp"
#include "/home/davide/wheeler_language_recognition/external/malloc_count/malloc_count.h"

typedef DFA_unidirectional_out_labelled DFA;

uint pair_key(uint i, uint j, uint n)
{
    return (i * n) + j;
}

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
uint read_interval(std::string input_file, std::vector< std::pair <uint,uint> >& intervals)
{
    // open stream to input
    std::ifstream input(input_file);
    std::string line;
    const char delim = '\t'; 
    uint beg, end;
    uint max_beg = 0;
    std::vector<std::string> out; 

    while(true)
    {
        std::getline(input, line);
        tokenize(line, delim, out);

        if(out.size() != 2){ break; };

        beg = std::stoull(out[0]);
        end = std::stoull(out[1]);

        if( beg > max_beg ){ max_beg = beg; }

        intervals.push_back(std::make_pair(beg,end));
    }

    // close stream to input file
    input.close();
    // return maximum beginning value
    return max_beg;
}  

// simple parser for intermediate file; it reads line by line origin \t destination \t label \n
void read_min_dfa(std::string input_file, DFA &A)
{
    // open stream to input
    std::ifstream input(input_file);
    std::string line;
    const char delim = ' '; 
    std::vector<std::string> out; 

    // remove first line
    uint origin, dest;
    int label;
    std::getline(input, line);
    while(true)
    {
        std::getline(input, line);
        tokenize(line, delim, out);

        if(out.size() != 3){ break; };

        origin = std::stoull(out[0]);
        //std::cout << out[1].size() << "\n";
        label = std::stoull(out[1]);
        dest = std::stoull(out[2]);

        A.at(origin)->out.insert({label,dest});
    }

    // close stream to input file
    input.close();
} 

void counting_sort(std::vector< std::pair <uint,uint> > &vec, std::vector<uint> &out, uint m)
{
    std::vector<uint> count(m,0);
    for(size_t i=0; i<vec.size(); ++i)
    {   
        uint cs = vec[i].first;
        count[cs]++;
    }  
    uint prev_c = count[0];
    count[0] = 0;
    for(size_t i=1; i<count.size(); ++i)
    {
        uint tmp = count[i];
        count[i] = count[i-1] + prev_c;
        prev_c = tmp;
    }  
    for (size_t i = 0; i < vec.size(); ++i)
    {
        uint cs = vec[i].first;
        uint index = count[cs]++;
        out[index] = i;
    }
}

// DFS function to find if a cycle exists
bool isCyclic(uint v, bool visited[], bool* recStack, DFA &A)
{
    if (visited[v] == false)
    {
        // mark state as visited
        visited[v] = true;
        recStack[v] = true;

        // visit all adjacent states
        for (auto& i: A.at(v)->out)
        {
            if (!visited[i.second] && isCyclic(i.second, visited, recStack, A))
            {
                return true;
            }
            else if (recStack[i.second])
            {
                return true;
            }
        }
    }

    // remove the state from recursion stack
    recStack[v] = false;
    return false;
}

bool DFS_cycle_detection(DFA &A)
{
    // mark all states as not visited
    uint V = A.no_nodes();
    bool* visited = new bool[V];
    bool* recStack = new bool[V];
    for (uint i = 0; i < V; ++i) {
        visited[i] = false;
        recStack[i] = false;
    }

    // recursive function call
    for (uint i = 0; i < V; ++i)
    {
        //std::cout << i << "\n";
        if (!visited[i] && isCyclic(i, visited, recStack, A))
        {
            return true;
        }
    }
 
    return false;
}
   
int main(int argc, char** argv)
{
    // initialize necessary data structures
    std::vector< std::pair <uint,uint> > intervals;
    std::unordered_map<uint,uint> pair_to_pos;
    std::unordered_map<char,std::vector<uint>> L;
    
    if(argc > 2)
    { 
        // set input arguments
        std::string in_dfa, in_interval;
        in_dfa = std::string(argv[1]);
        in_interval = std::string(argv[2]);

        // read intervals file
        uint max_beg = read_interval(in_interval,intervals);

        uint n = intervals.size();
        #ifdef VERBOSE
        {
            std::cout << "number of nodes: " << n << "\n";
        }
        #endif

        std::vector<uint> order;
        order.resize(n);
        counting_sort(intervals, order, max_beg+1);

        #ifdef VERBOSE
        {
            // print intervals
            for(uint i=0;i<intervals.size();++i)
            {
                std::cout << intervals[order[i]].first << " - " << intervals[order[i]].second << " : " << order[i] << "\n";
            }
            std::cout << "###########\n";
        }
        #endif

        
        // compute A^2 pruned states
        uint a_2_states = 0;
        uint i=0, j=1;
        std::cout << "compute A^2 pruned automaton states\n";
        std::cout << "Interval size: " << intervals.size() << "\n";

        while( i < intervals.size() - 1 )
        {
            if( intervals[order[i]].second > intervals[order[j]].first )
            {
                #ifdef VERBOSE
                {
                    std::cout << "create a state (" << order[i] << " " << order[j] << ")\n"; 
                    std::cout << "create a state (" << order[j] << " " << order[i] << ")\n"; 
                }
                #endif

                // only insert one copy of the two simmetric states
                pair_to_pos.insert({pair_key(order[i],order[j],n),a_2_states});
                // increment states by two
                a_2_states += 2;
                // increment j otherwise skip to next i
                ++j;
                if( j == intervals.size() ){ i++; j=i+1; }
            }
            else{ i++; j=i+1;  }
        }

        // free intervals data structures
        intervals.clear();
        // initalize A squared pruned
        #ifdef VERBOSE
        {
            std::cout << "A squared pruned states: " << a_2_states << "\n";
        }
        #endif

        std::cout << "A squared pruned states: " << a_2_states << "\n";

        DFA A = DFA(a_2_states);
        
        // read minimized DFA
        DFA M = DFA(n); 
        read_min_dfa(in_dfa,M); 

        // create L data structure
        for(uint x=0;x<n;++x)
        { 
            uint i = order[x];
            for (auto& j: M.at(i)->out)
            {
                if(L.find(j.first) != L.end())
                {
                    L[j.first].push_back(i);
                }
                else{ L[j.first] = std::vector<uint>{ i }; }
            }
        }

        #ifdef VERBOSE
        {
            // print L data structure
            for (auto& j: L)
            {
                std::cout << j.first << " : ";
                for(uint i=0;i<j.second.size();++i)
                {
                    std::cout << j.second[i] << " ";
                }
                std::cout << std::endl;
            }
        }
        #endif

        // free order vector and isa vector creation
        std::vector<uint> isa; 
        isa.resize(n);
        for(uint i=0;i<n;++i)
        {
            isa[order[i]] = i;
        }
        order.clear();

        // compute A^2 pruned automaton edges
        for (auto& l: L)
        {
            #ifdef VERBOSE
            {
                std::cout << "-------------n\n";
                std::cout << "label: " << l.first << "\n";
            }
            #endif
            uint i=0, j=1;
            uint lsize = l.second.size();
            if(lsize == 1){ continue; }

            while( i < lsize - 1 )
            {
                if( pair_to_pos.find(pair_key(l.second[i],l.second[j],n)) != pair_to_pos.end() )
                {
                    uint pair_pos = pair_to_pos.find(pair_key(l.second[i],l.second[j],n))->second;

                    uint new_i = M.at(l.second[i])->out.find(l.first)->second;
                    uint new_j = M.at(l.second[j])->out.find(l.first)->second;

                    bool invert = false;
                    #ifdef VERBOSE
                    {
                        std::cout << "pair (" << l.second[i] << " " << l.second[j] << ") -> (" << new_i << " " << new_j << ")\n";
                    }
                    #endif
                    
                    if( isa[new_i] > isa[new_j] ){ uint tmp = new_i; new_i = new_j; new_j = tmp; invert = true; }
                    if( pair_to_pos.find(pair_key(new_i,new_j,n)) != pair_to_pos.end() )
                    { 
                        #ifdef VERBOSE
                        {
                            std::cout << "add edge!\n";
                        }
                        #endif

                        uint new_pair_pos = pair_to_pos.find(pair_key(new_i,new_j,n))->second;
                        if( invert )
                        {
                            A.at(pair_pos)->out[l.first] = new_pair_pos+1;
                            A.at(pair_pos+1)->out[l.first] = new_pair_pos;
                        }
                        else
                        {
                            A.at(pair_pos)->out[l.first] = new_pair_pos;
                            A.at(pair_pos+1)->out[l.first] = new_pair_pos+1;
                        }
                    }
                    // increment j otherwise skip to next i
                    j++;
                    if( j == lsize ){ i++; j=i+1; }
                }
                else{ i++; j=i+1; }
            }
        }

        // free memory
        isa.clear();
        pair_to_pos.clear();
        L.clear();
        //M.~DFA();
        M.clear();
        // open output file
        std::ofstream ofile;
        ofile.open("answer");
        //ofile.open(out); 
        std::cout << "Check cyclicity!\n";
        if( DFS_cycle_detection(A) ){ std::cout << "Cycle found! The language is not Wheeler!\n"; ofile << 0; }
        else{ std::cout << "No cycle found! The language is Wheeler!\n"; ofile << 1; }
        ofile.close();
    }
    else
    {
        std::cerr << "invalid no. arguments\n";
        std::cerr << "Format your command as follows: TODO\n";
        exit(1);
    }


  return 0;
}