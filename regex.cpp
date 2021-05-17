/**
 * Main function to run regex implementations.
 */

#include <iostream>
#include <ctype.h>
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

	Regex *regex = new ArrayNFARegex(argv[1]);
	std::cout << regex->to_str() << std::endl;

	std::string s;
	while (std::getline(std::cin, s)) {
		std::cout << (regex->match(s)?"true":"false") << std::endl;
	}

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

Regex::Regex(std::string regex) : _r(regex), prs_regex(parse_regex(regex)) {}

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

std::vector<int> Regex::parse_regex(std::string regex) {
	int layer = 0, brk = 0;
	std::vector<int> prs_regex;
	for (std::string::const_iterator c = regex.cbegin(); c != regex.cend(); ++c) {
		int _c = *c;
		switch(*c) {
		  case '(':
			++layer;
			++brk;
			_c = OP_BR;
			break;
		  case ')':
			if (--layer < 0)
				fatal_error("Syntax error!");
		  	_c = CL_BR;
			break;
		  case '\\':
			++c;
			switch (*c) {
			  case 'A':
			  case 'a':
				_c = ALPHA;
				break;
			  case '0':
				_c = NUMER;
				break;
			  case ' ':
				_c = WHTSP;
				break;
			  case '(':
			  case ')':
			  case '\\':
			  case '.':
			  case '?':
			  case '*':
			  case '+':
			  case '|':
				_c = *c;
				break;
			  default:
				fatal_error("Syntax error!");
			}
			break;
		  case '.':
			_c = WLDCR;
			break;
		  case '?':
			_c = KQUES;
			break;
		  case '*':
			_c = KSTAR;
			break;
		  case '+':
			_c = KPLUS;
			break;
		  case '|':
			_c = R_UNN;
			break;
		  default:
			;
		}
		prs_regex.push_back(_c);
	}

	if (layer != 0)
		fatal_error("Syntax error!");
	
	r_len = prs_regex.size() - 2*brk;
	return prs_regex;
}

bool char_match(char c, int _t) {
	bool a;
	switch (_t)	{
	  case WLDCR:
		a = true;
		break;
	  case ALPHA:
		a = isalpha(c);
		break;
	  case NUMER:
		a = isalnum(c);
		break;
	  case WHTSP:
		a = isspace(c);
		break;
	  default:
		a = (c == _t);
	}
	return a;
}

std::string transition_to_str(int _t) {
	std::string ret;
	switch(_t) {
	  case WLDCR:
		ret = "WLDCR";
		break;
	  case ALPHA:
		ret = "ALPHA";
		break;
	  case NUMER:
		ret = "NUMER";
		break;
	  case WHTSP:
		ret = "WHTSP";
		break;
	  case ACCEPT:
		ret = "ACC";
		break;
	  case EPS:
		ret = "EPS";
		break;
	  default:
		ret.assign((char*)(&_t));
	}
	return ret;
}
