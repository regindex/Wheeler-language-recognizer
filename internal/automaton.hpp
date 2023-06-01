#ifndef AUTOMATON_HPP_
#define AUTOMATON_HPP_

#include <cstdint>
#include <cassert>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>

typedef uint32_t uint;

/*
	node of the DFA
*/
struct node
{
	// out edges
	std::unordered_map<char,uint> out;
};

// class for directed bidirectional unlabeled NFA
class DFA_unidirectional_out_labelled{

public:
	// empty constructor
	DFA_unidirectional_out_labelled(){}
	// constructor 
	DFA_unidirectional_out_labelled(uint n)
	{
		nodes = n;
		//initialize DFA
		DFA.resize(nodes);
	}
	// destructor
	~DFA_unidirectional_out_labelled(){}

	node* at(uint i){

		assert(i < nodes);
		// return pointer to node
		return &DFA[i];
	}

	uint no_nodes(){ return nodes; }
	uint no_edges(){ return edges; }

	void clear()
	{
		DFA.resize(0);
		DFA.shrink_to_fit();
	}

private:
	// number of nodes in the DFA
	uint nodes;
	// number of edges in the DFA
	uint edges;
	// vector containing all edges
	std::vector<node> DFA;
};


#endif