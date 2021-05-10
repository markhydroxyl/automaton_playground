/**
 * Implementation of regex using NFAs, where NFA states
 * are represented as a graph and the transitions runs
 * along the graph, tracking the states.
 */

#include <vector>
#include <string>
#include <set>
#include "regex.h"
#include "graph_nfa.h"

using std::string;
typedef std::string::const_iterator str_iter;
typedef std::vector<GraphNFAState **> PtrList;

struct GraphNFAState {
	GraphNFAState(int c, GraphNFAState *n, GraphNFAState *f) : _c(c), next(n), fork(f) {}
	// transition character
	int _c;
	// dangling transitions
	GraphNFAState *next;
	GraphNFAState *fork;
};

struct GraphNFA {
	GraphNFA(GraphNFAState *s, bool f);
	GraphNFA(GraphNFA *lhs, GraphNFA *rhs);
	GraphNFAState *start;
	PtrList dangling;
	void patch(GraphNFAState *sp);
	str_iter build_nfa(str_iter head, str_iter end);
};

class GraphNFARegex : public Regex {
  public:
	GraphNFARegex(std::string regex);
	~GraphNFARegex() override;
	std::string to_str() const override;
	bool match(std::string str) const override;
  private:
	GraphNFA nfa;
	str_iter build_nfa(str_iter head, str_iter end, GraphNFA *n);
	GraphNFA *get_last(std::vector<GraphNFA *> stack);
};

GraphNFA::GraphNFA(GraphNFAState *s=nullptr, bool f=false) : start(s) { 
	if (s) {
		dangling.push_back(&s->next);
		if (f)
			dangling.push_back(&s->fork);
	}
}

GraphNFA::GraphNFA(GraphNFA *lhs, GraphNFA *rhs) {
	GraphNFAState *sp = new GraphNFAState(EPS, lhs->start, rhs->start);
	dangling = lhs->dangling;
	for (GraphNFAState **r : rhs->dangling) {
		dangling.push_back(r);
	}
	delete lhs;
	delete rhs;
}

void GraphNFA::patch(GraphNFAState *sp) {
	for (GraphNFAState **s : dangling) {
		*s = sp;
	}
	dangling.clear();
	dangling.push_back(&sp->next);
}

GraphNFARegex::GraphNFARegex(string regex) : Regex(regex) {
	if (build_nfa(regex.cbegin(), regex.cend(), &nfa) != regex.cend()) {
		fatal_error("Syntax error in regex");
	}
	GraphNFAState *accept = new GraphNFAState(ACCEPT, nullptr, nullptr);
	nfa.patch(accept);
}

GraphNFARegex::~GraphNFARegex() {
	std::set<GraphNFAState *> s;
	GraphNFAState *cur = nfa.start;
	s.insert(cur);
	std::vector<GraphNFAState *> queue;
	size_t i = 0;
	while (i < queue.size()) {
		if (cur->next) {
			queue.push_back(cur->next);
			s.insert(cur->next);
		}
		if (cur->fork) {
			queue.push_back(cur->fork);
			s.insert(cur->fork);
		}
		cur = queue[++i];
	}
	for (std::set<GraphNFAState *>::iterator s_it = s.begin(); s_it != s.end(); ++s_it) {
		delete *s_it;
	}
}

str_iter GraphNFARegex::build_nfa(str_iter head, str_iter end, GraphNFA *n) {
	std::vector<GraphNFA *> stack;
	bool cont = true;
	for (; head != end && cont; ++head) {
		GraphNFAState *sp;
		GraphNFA *n2, *n3;
		switch(*head) {
		  case '?': // zero or one
		  	n2 = get_last(stack);
			sp = new GraphNFAState(EPS, n2->start, nullptr);
			n3 = new GraphNFA(sp, true);
			stack.push_back(n3);
			delete n2;
			break;
		  case '*': // zero or more
		  	n2 = get_last(stack);
			sp = new GraphNFAState(EPS, n2->start, nullptr);
			n2->patch(sp);
			n3 = new GraphNFA(sp);
			stack.push_back(n3);
			delete n2;
			break;
		  case '+': // one or more
			n2 = get_last(stack);
			sp = new GraphNFAState(EPS, n2->start, nullptr);
			n2->patch(sp);
			n2->dangling.clear();
			n2->dangling.push_back(&sp->next);
			stack.push_back(n2);
			break;
		  case '|': // union
			n2 = get_last(stack);
			head = build_nfa(++head, end, n3);
			GraphNFA *n4 = new GraphNFA(n2, n3);
			stack.push_back(n4);
		  	break;
		  case '(': // open bracket
			head = build_nfa(++head, end, n2);
			stack.push_back(n2);
		  	break;
		  case ')': // close bracket
			++head;
			cont = false;
		  	break;
		  default: // literals
			sp = new GraphNFAState(*head, nullptr, nullptr);
			n2 = new GraphNFA(sp);
			stack.push_back(n2);
		}
	}

	// concat all NFAs in the stack
	while (stack.size() > 1) {
		GraphNFA *n2 = get_last(stack);
		stack.back()->patch(n2->start);
		delete n2;
	}
	*n = *stack[0];
	delete stack[0];
	return head;
}

GraphNFA *get_last(std::vector<GraphNFA *> stack) {
	if (stack.empty())
		fatal_error("Syntax error in regular expression");
	GraphNFA *ret = stack.back();
	stack.pop_back();
	return ret;
}

string GraphNFARegex::to_str() const {
	
}

bool GraphNFARegex::match(std::string str) const {

}
