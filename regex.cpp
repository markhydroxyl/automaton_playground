/**
 * Main function to run regex implementations.
 */

#include <iostream>
#include <string>
#include "regex.h"

int main(int argc, char *argv[]) {
	if (argc == 1) {
		usage();
		return 0;
	} else if (argc > 2) {
		fatal_error("Wrong number of arguments.");
	}

	// TODO: use a derived class' implementation.
	Regex *regex = new Regex(argv[1]);

	std::string s;
	while (std::cin >> s) {
		std::cout << regex->match(s) << std::endl;
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

Regex::Regex(std::string regex) : _r(regex) {}

Regex::~Regex() {}

bool Regex::match(std::string str) {
	str = str;
	fatal_error("Regex not implemented.");
	return false;
}

std::string Regex::to_str() const {
	fatal_error("Regex not implemented.");
	return "";
}
