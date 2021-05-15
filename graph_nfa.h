/**
 * Header file for Graph NFAs.
 * Heavily inspired by https://swtch.com/~rsc/regexp/regexp1.html
 */

#ifndef GRAPH_NFA_H
#define GRAPH_NFA_H

#include <string>
#include <vector>
#include <set>
#include "regex.h"

struct GraphNFA {
  public:
	GraphNFA();
	GraphNFA(int t);
	GraphNFA(std::string regex);
	~GraphNFA();
	void destroy_states();
	std::string to_str() const;
	bool match(std::string::const_iterator head, std::string::const_iterator end) const;
  private:
	struct GraphNFAState {
		GraphNFAState(int t, GraphNFAState *n, GraphNFAState *f);
		int _t;
		GraphNFAState *next;
		GraphNFAState *fork;
	};

	GraphNFAState *start;
	std::vector<GraphNFAState **> *dangling;

	std::string::const_iterator build_nfa(std::string::const_iterator head, std::string::const_iterator end);
	void connect_dangling(GraphNFAState *s);
	void concat(int c);
	void concat(GraphNFA *n);
	void nfa_union(GraphNFA *n);
	void unary(bool zero, bool inf);
	void add_to_state_set(GraphNFAState *s, std::set<GraphNFAState *> *set) const;
};

class GraphNFARegex: public Regex {
  public:
	GraphNFARegex(std::string regex);
	~GraphNFARegex() override;
	std::string to_str() const override;
	bool match(std::string str) const override;
  private:
	GraphNFA nfa;
};

#endif
