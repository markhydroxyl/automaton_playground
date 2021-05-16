/**
 * Header file for Graph NFAs.
 * Heavily inspired by https://swtch.com/~rsc/regexp/regexp1.html
 */

#ifndef GRAPH_NFA_H
#define GRAPH_NFA_H

#include <set>
#include "regex.h"

struct GraphNFA {
  public:
	GraphNFA();
	GraphNFA(int t);
	GraphNFA(const std::vector<int> *regex);
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

	size_t build_nfa(const std::vector<int> *regex, size_t offset);
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
