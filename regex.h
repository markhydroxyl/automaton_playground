#ifndef REGEX_H
#define REGEX_H

#include <string>

#define ACCEPT -1
#define EPS -2
// #define ALPHA -3
// #define NUMER -4
// #define WHTSP -5

// wrapper class for any regex implementation
class Regex {
  public:
	Regex(std::string regex);
	virtual ~Regex();
	std::string get_regex() const { return _r; }
	virtual std::string to_str() const;
	virtual bool match(std::string str) const;
  private:
	std::string _r;
};

void fatal_error(std::string err);
void usage();

#endif
