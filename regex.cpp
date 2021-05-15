/**
 * Main function to run regex implementations.
 */

#include <iostream>
#include "regex.h"
#include "graph_nfa.h"
#include "array_nfa.h"

int main(int argc, char *argv[]) {
	if (argc == 1) {
		usage();
		return 0;
	} else if (argc > 2) {
		fatal_error("Wrong number of arguments.");
	}

	// TODO: use a derived class' implementation.
	Regex *regex = new ArrayNFARegex(argv[1]);
	std::cout << regex->to_str() << std::endl;

	// std::string s;
	// while (std::getline(std::cin, s)) {
	// 	std::cout << (regex->match(s)?"true":"false") << std::endl;
	// }

	delete regex;

	return 0;
}

void fatal_error(std::string err) {
	std::cout << err << std::endl;
	exit(1);
}

// Prints the help message to stdout.
void usage() {
	std::cout << "Usage: regex <exp>" << std::endl;
	std::cout << "\twhere exp is a valid regular expression." << std::endl;
}

Regex::Regex(std::string regex) : _r(regex), r_len(regex_size(regex)) {}

Regex::~Regex() {}

bool Regex::match(std::string str) const {
	str = str;
	fatal_error("Regex not implemented.");
	return false;
}

std::string Regex::to_str() const {
	fatal_error("Regex not implemented.");
	return "";
}

int Regex::regex_size(std::string regex) const {
	int layer = 0, cnt = 0;

	for (std::string::const_iterator c = regex.cbegin(); c != regex.cend(); ++c) {
		if (*c == '(') {
			++layer;
		} else if (*c == ')') {
			if (--layer < 0)
				fatal_error("Syntax error!");
		} else {
			++cnt;
		}
	}

	if (layer != 0)
		fatal_error("Syntax error!");

	return cnt;
}
