/**
 * Header file for Array NFAs.
 * Inspired by modifying the graph implementation.
 */

#ifndef MATRIX_NFA_H
#define MATRIX_NFA_H

#include <array>
#include <utility>
#include "regex.h"

struct ArrayNFA {
  public:
	ArrayNFA(const std::vector<int> *regex, int len);
	std::string to_str() const;
	bool match(std::string::const_iterator head, std::string::const_iterator end) const;
  private:
	struct ArrayNFAElem {
		int start;
		int tail;
		std::vector<std::array<int, 3>> *states;
		std::vector<std::pair<int, int>> *dangling;

		ArrayNFAElem(std::vector<std::array<int, 3>> *states, int start);
		~ArrayNFAElem();
		void reinit(int start);
		void reinit(int start, int _t);
		size_t build_nfa(const std::vector<int> *regex, size_t head);

		void connect_dangling(int idx);
		void unary(bool zero, bool inf);
		void concat(ArrayNFAElem *n);
		void nfa_union(ArrayNFAElem *n);
	};

	int start;
	std::vector<std::array<int, 3>> states;

	void add_to_state_set(int idx, std::vector<bool> *vec) const;
};

class ArrayNFARegex : public Regex {
  public:
	ArrayNFARegex(std::string regex);
	~ArrayNFARegex() override;
	std::string to_str() const override;
	bool match(std::string str) const override;
  private:
	ArrayNFA nfa;
};

#endif
