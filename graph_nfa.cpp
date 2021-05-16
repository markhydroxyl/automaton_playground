/**
 * Implementation of regex using NFAs, where NFA states
 * are represented as nodes in a graph and the transitions
 * are edges in the graph, tracking the states.
 */

#include <map>
#include <sstream>
#include "graph_nfa.h"

using std::string;
typedef std::string::const_iterator str_iter;

GraphNFA::GraphNFAState::GraphNFAState(int t, GraphNFAState *n, GraphNFAState *f) : _t(t), next(n), fork(f) {}

/**
 * Builds a new, empty NFA.
 */
GraphNFA::GraphNFA() : start(nullptr) {
	dangling = new std::vector<GraphNFAState **>();
}

/**
 * Builds a new NFA with one state with transition represented by t.
 */
GraphNFA::GraphNFA(int t) {
	start = new GraphNFAState(t, nullptr, nullptr);
	dangling = new std::vector<GraphNFAState **>();
	dangling->push_back(&start->next);
}

/**
 * Builds a new NFA from the given regex.
 */
GraphNFA::GraphNFA(const std::vector<int> *regex) {
	dangling = new std::vector<GraphNFAState **>();
	build_nfa(regex, 0);
	GraphNFAState *accept = new GraphNFAState(ACCEPT, nullptr, nullptr);
	connect_dangling(accept);
}

/**
 * N.B.: does not destroy the NFA's states! See destroy_states() instead.
 */
GraphNFA::~GraphNFA() {
	delete dangling;
}

GraphNFARegex::GraphNFARegex(string regex) : Regex(regex), nfa(&prs_regex) {}

GraphNFARegex::~GraphNFARegex() {
	nfa.destroy_states();
}

/**
 * Destructs all of the NFA's states.
 */
void GraphNFA::destroy_states() {
	std::set<GraphNFAState *> visited;
	std::vector<GraphNFAState *> queue;
	GraphNFAState *cur;
	size_t i = 0;

	visited.insert(start);
	queue.push_back(start);
	while (i < queue.size()) {
		#define DESTROY_INS(P) if(P){std::set<GraphNFAState*>::iterator v_it=visited.find(P);if(v_it==visited.end()){visited.insert(P);queue.push_back(P);}}
		cur = queue[i++];
		DESTROY_INS(cur->next);
		DESTROY_INS(cur->fork);
	}

	for (i = 0; i < queue.size(); ++i) {
		delete queue[i];
	}
}

/**
 * Creates a human-readable string representation of the NFA
 * by assigning numbers to each of the NFA's states and 
 * showing all of the connections.
 */
string GraphNFA::to_str() const {
	if (!start)
		return "empty NFA";

	std::stringstream ss;
	std::map<GraphNFAState *, int> visited;
	std::vector<GraphNFAState *> queue;
	GraphNFAState *cur;
	size_t i = 0;
	int j = 1;

	visited[start] = 0;
	queue.push_back(start);
	while (i < queue.size()) {
		#define TOSTR_INS(P) if(P){std::map<GraphNFAState*,int>::iterator v_it=visited.find(P);if(v_it==visited.end()){visited[P]=j++;queue.push_back(P);}}
		cur = queue[i++];
		TOSTR_INS(cur->next);
		TOSTR_INS(cur->fork);
	}

	for (i=0; i<queue.size(); ++i) {
		#define PRINT_PTR(P) if(P)ss<<((P->_t==ACCEPT)?'A':'S')<<visited[P];else ss<<"NULL";
		PRINT_PTR(queue[i]);
		switch(queue[i]->_t) {
		  case ACCEPT:
			ss << " (ACCEPT)";
			break;
		  case EPS:
			ss << " -EPS-> ";
			PRINT_PTR(queue[i]->next);
			ss << ", ";
			PRINT_PTR(queue[i]->fork);
			break;
		  default:
			ss << " -" << transition_tostr(queue[i]->_t) << "-> ";
			PRINT_PTR(queue[i]->next);
		}
		ss << std::endl;
	}
	
	return ss.str();
}

size_t GraphNFA::build_nfa(const std::vector<int> *regex, size_t offset) {
	GraphNFA *n = new GraphNFA();
	for (; offset < regex->size(); ++offset) {
		int c = (*regex)[offset];
		if (c == KQUES)
			n->unary(true, false);
		else if (c == KSTAR)
			n->unary(true, true);
		else if (c == KPLUS)
			n->unary(false, true);
		else if (c == CL_BR)
			break;
		else if (c == OP_BR) {
			concat(n);
			n = new GraphNFA();
			offset = n->build_nfa(regex, ++offset);
			if (offset == regex->size()) break;
		} else if (c == R_UNN) {
			concat(n);
			n = new GraphNFA();
			offset = n->build_nfa(regex, ++offset);
			nfa_union(n);
			n = new GraphNFA();
			if (offset == regex->size()) break;
		} else {
			concat(n);
			n = new GraphNFA(c);
		}
	}
	concat(n);
	return offset;
}

/**
 * Concatenates the last element to a new state with the specified transition.
 * 
 * If this NFA is empty, sets the new state as the start state.
 */
void GraphNFA::concat(int t) {
	GraphNFAState *s = new GraphNFAState(t, nullptr, nullptr);
	if (start) connect_dangling(s);
	else start = s;
	dangling->push_back(&s->next);
}

/**
 * Concatenates (and deletes) another NFA.
 * 
 * If this NFA is empty, copies the new NFA, treating the new NFA as one element.
 */
void GraphNFA::concat(GraphNFA *n) {
	if (n->start) {
		if (start) connect_dangling(n->start);
		else start = n->start;
		// steal n->dangling
		delete dangling;
		dangling = n->dangling;
		n->dangling = nullptr;
	}
	delete n;
}

/**
 * Takes the union of this NFA with a new NFA (and deletes it).
 * 
 * If this NFA is empty, copies the new NFA, treating the new NFA as one element.
 */
void GraphNFA::nfa_union(GraphNFA *n) {
	if (!start)
		concat(n);
	else {
		GraphNFAState *s = new GraphNFAState(EPS, start, n->start);
		start = s;
		// dangling->resize();
		for (GraphNFAState **sp : *n->dangling)
			dangling->push_back(sp);
		delete n;
	}
}

/**
 * Applies a unary operator to the last element of the NFA,
 * characterized by whether it allows zero copies of the
 * element and whether it allows infinite copies of the element.
 * 
 * If the NFA is empty, no operation is applied.
 */
void GraphNFA::unary(bool zero, bool inf) {
	if (start) {
		GraphNFAState *s = new GraphNFAState(EPS, start, nullptr);
		if (zero) start = s;
		if (inf) connect_dangling(s);
		dangling->push_back(&s->fork);
	}
}

/**
 * Connects all of the NFA's dangling transitions to s.
 */
void GraphNFA::connect_dangling(GraphNFAState *s) {
	for (GraphNFAState **sp : *dangling)
		*sp = s;
	dangling->clear();
}

/**
 * Generates a human-readable string of the internal NFA.
 */
string GraphNFARegex::to_str() const {
	return nfa.to_str();
}

/**
 * Determines whether the NFA accepts the string.
 */
bool GraphNFA::match(str_iter head, str_iter end) const {
	std::set<GraphNFAState *> *cur = new std::set<GraphNFAState *>(),
							*next = new std::set<GraphNFAState *>(),
							*t;
	
	add_to_state_set(start, cur);
	for (; head != end; ++head) {
		for (GraphNFAState *s : *cur) {
			if (char_match(*head, s->_t)) // if transition matches
				add_to_state_set(s->next, next);
		}
		t = cur; cur = next; next = t; // swap
		next->clear();
	}

	bool accept = false;
	for (GraphNFAState *s : *cur) {
		if (s->_t == ACCEPT) {
			accept = true;
			break;
		}
	}

	delete cur;
	delete next;
	return accept;
}

/**
 * A helper function to properly add a state to the set of states when
 * simulating the NFA.
 */
void GraphNFA::add_to_state_set(GraphNFAState *s, std::set<GraphNFAState *> *set) const {
	if (s) {
		if (s->_t == EPS) {
			add_to_state_set(s->next, set);
			add_to_state_set(s->fork, set);
		} else {
			set->insert(s);
		}
	}
}

bool GraphNFARegex::match(std::string str) const {
	return nfa.match(str.cbegin(), str.cend());
}
