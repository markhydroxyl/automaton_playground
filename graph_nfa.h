#ifndef GRAPH_NFA_H
#define GRAPH_NFA_H

#include <string>
#include <vector>
#include "regex.h"

struct GraphNFAState {
	GraphNFAState(int t, GraphNFAState *n, GraphNFAState *f);
	int _t;
	GraphNFAState *next;
	GraphNFAState *fork;
};

struct GraphNFA {
  public:
	GraphNFA();
	GraphNFA(int t);
	GraphNFA(std::string::const_iterator head, std::string::const_iterator end);
	~GraphNFA();
	void destroy_states();
	std::string to_str() const;
	void connect_dangling(GraphNFAState *s);
  private:
	GraphNFAState *start;
	std::vector<GraphNFAState **> *dangling;
	std::string::const_iterator build_nfa(std::string::const_iterator head, std::string::const_iterator end);
	void concat(int c);
	void concat(GraphNFA *n);
	void nfa_union(GraphNFA *n);
	void unary(bool zero, bool inf);
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
