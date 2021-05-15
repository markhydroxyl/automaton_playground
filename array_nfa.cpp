/**
 * Implementation of regex using NFAs, where NFA states
 * and transitions are represented as a sparse adjacency
 * list digraph.
 */

#include "array_nfa.h"
#include <sstream>
#include <iomanip>

using std::string;
typedef string::const_iterator str_iter;

ArrayNFA::ArrayNFAElem::ArrayNFAElem(std::vector<std::array<int, 3>> *states, int start) : start(start), tail(start), states(states) {
	dangling = new std::vector<std::pair<int, int>>();
}

ArrayNFA::ArrayNFAElem::~ArrayNFAElem() {
	delete dangling;
}

void ArrayNFA::ArrayNFAElem::reinit(int start) {
	this->start = start;
	this->tail = start;
	this->dangling->clear();
}

void ArrayNFA::ArrayNFAElem::reinit(int start, int _t) {
	(*states)[start][0] = _t;
	this->start = start;
	this->tail = start + 1;
	this->dangling->clear();
	this->dangling->push_back(std::make_pair(start, 1));
}

ArrayNFA::ArrayNFA(string regex, int len) : start(-1), states(len+1, {ACCEPT, -1, -1}) {
	ArrayNFAElem n(&states, 0);
	n.build_nfa(regex.cbegin(), regex.cend());
	n.connect_dangling(len);
	start = n.start;
}

ArrayNFARegex::ArrayNFARegex(string regex) : Regex(regex), nfa(regex, r_len) {}

ArrayNFARegex::~ArrayNFARegex() {}

string ArrayNFA::to_str() const {
	if (start == -1)
		return "empty NFA";

	const unsigned S_WIDTH = 5, ROWS = 5;
	std::stringstream ss, rs[ROWS];

	for (unsigned i=0; i<states.size(); ++i) {
		rs[0] << std::setw(S_WIDTH) << (i==(unsigned)start ? 'v' : ' ');

		rs[1] << std::setw(S_WIDTH) << i;

		if (states[i][0] == ACCEPT)
			rs[2] << std::setw(S_WIDTH) << "ACC";
		else if (states[i][0] == EPS)
			rs[2] << std::setw(S_WIDTH) << "EPS";
		else
			rs[2] << std::setw(S_WIDTH) << (char) states[i][0];

		if (states[i][1] == -1)
			rs[3] << std::setw(S_WIDTH) << "NULL";
		else
			rs[3] << std::setw(S_WIDTH) << states[i][1];

		if (states[i][2] == -1)
			rs[4] << std::setw(S_WIDTH) << "NULL";
		else
			rs[4] << std::setw(S_WIDTH) << states[i][2];

		for (unsigned j=0; j<ROWS; ++j) {
			rs[j] << '|';
		}
	}

	ss << std::setfill('-');
	for (unsigned i=0; i<ROWS; ++i) {
		ss << '|' << rs[i].rdbuf() << std::endl << '|';
		for (unsigned j=0; j<states.size(); ++j)
			ss << std::setw(S_WIDTH+1) << "-|";
		ss << std::endl;
	}
	return ss.str();
}

string ArrayNFARegex::to_str() const {
	return nfa.to_str();
}

str_iter ArrayNFA::ArrayNFAElem::build_nfa(str_iter head, str_iter end) {
	ArrayNFAElem n(states, tail);
	for (; head != end; ++head) {
		if (*head == '?')
			n.unary(true, false);
		else if (*head == '*')
			n.unary(true, true);
		else if (*head == '+')
			n.unary(false, true);
		else if (*head == ')')
			break;
		else if (*head == '(') {
			concat(&n);
			n.reinit(tail);
			head = n.build_nfa(++head, end);
			if (head == end) break;
		} else if (*head == '|') {
			concat(&n);
			n.reinit(tail);
			head = n.build_nfa(++head, end);
			nfa_union(&n);
			n.reinit(tail);
			if (head == end) break;
		} else {
			concat(&n);
			n.reinit(tail, *head);
		}
	}
	concat(&n);
	return head;
}

void ArrayNFA::ArrayNFAElem::connect_dangling(int idx) {
	for (std::pair<int, int> p : *dangling)
		(*states)[p.first][p.second] = idx;
	dangling->clear();
}

void ArrayNFA::ArrayNFAElem::unary(bool zero, bool inf) {
	if (start < tail) {
		(*states)[tail][0] = EPS;
		(*states)[tail][1] = start;
		if (zero) start = tail;
		if (inf) connect_dangling(tail);
		dangling->push_back(std::make_pair(tail++, 2));
	}
}

void ArrayNFA::ArrayNFAElem::concat(ArrayNFAElem *n) {
	if (n->start < n->tail) {
		if (start >= tail)
			start = n->start;
		else
			connect_dangling(n->start);

		tail = n->tail;
		dangling->clear();

		std::vector<std::pair<int, int>> *t;
		t = dangling; dangling = n->dangling; n->dangling = t;
	}
}

void ArrayNFA::ArrayNFAElem::nfa_union(ArrayNFAElem *n) {
	if (n->start < n->tail) {
		if (start >= tail)
			concat(n);

		tail = n->tail;
		(*states)[tail][0] = EPS;
		(*states)[tail][1] = start;
		(*states)[tail][2] = n->start;
		start = tail++;
		for (std::pair<int, int> p : *n->dangling)
			dangling->push_back(p);
	}
}

bool ArrayNFARegex::match(std::string str) const {
	return nfa.match(str.cbegin(), str.cend());
}

bool ArrayNFA::match(str_iter head, str_iter end) const {
	const unsigned size = states.size();
	std::vector<bool> *cur = new std::vector<bool>(size, false), 
					*next = new std::vector<bool>(size, false),
					*t;
	
	add_to_state_set(start, cur);
	for (; head != end; ++head) {
		for (unsigned idx=0; idx < size; ++idx) {
			if ((*cur)[idx] && states[idx][0] == *head) // if in cur set and correct transition
				add_to_state_set(states[idx][1], next);

			(*cur)[idx] = false;
		}
		t = cur; cur = next; next = t; // swap
	}

	bool accept = false;
	for (unsigned idx=0; idx < size; ++idx) {
		if ((*cur)[idx] && states[idx][0] == ACCEPT) {
			accept = true;
			break;
		}
	}
	delete cur;
	delete next;
	return accept;
}

void ArrayNFA::add_to_state_set(int idx, std::vector<bool> *vec) const {
	if (idx >= 0) {
		if (states[idx][0] == EPS) {
			add_to_state_set(states[idx][1], vec);
			add_to_state_set(states[idx][2], vec);
		} else {
			(*vec)[idx] = true;
		}
	}
}
