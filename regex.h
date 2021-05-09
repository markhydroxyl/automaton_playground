#ifndef REGEX_H
#define REGEX_H

#include <string>

// wrapper class for any regex implementation
class Regex {
  public:
	Regex(std::string regex);
	virtual ~Regex();
	std::string get_regex() const { return _r; }
	virtual std::string to_str() const;
	virtual bool match(std::string str);
  private:
	std::string _r;
};

void fatal_error(std::string err);
void usage();

#endif
