#ifndef REGEX_H
#define REGEX_H

#include <string>
#include <vector>

#define ACCEPT -1
#define EPS -2
#define OP_BR -3
#define CL_BR -4
#define KQUES -5
#define KSTAR -6
#define KPLUS -7
#define R_UNN -8
#define WLDCR -9
#define ALPHA -10
#define NUMER -11
#define WHTSP -12

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
	std::vector<int> parse_regex(std::string regex);
  protected:
	int r_len;
	std::vector<int> prs_regex;
};

void fatal_error(std::string err);
void usage();

bool char_match(char c, int _t);
std::string transition_tostr(int _t);

#endif
