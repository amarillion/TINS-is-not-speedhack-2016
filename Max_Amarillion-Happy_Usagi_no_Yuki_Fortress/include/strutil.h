#ifndef STRUTIL_H
#define STRUTIL_H

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <string>

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}


//TOOD: merge with speehdack15 stringUtil
static inline bool contains (const std::string &haystack, const std::string &substring)
{
	return haystack.find(substring) != std::string::npos;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

// check if argument starts with prefix
static inline bool startsWith(const std::string &prefix, const std::string &argument)
{
	return (argument.substr(0, prefix.size()) == prefix);
}

static inline std::string toUpper(const std::string &str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), ::toupper);
	return result;
}

static inline bool equalsIgnoreCase (const std::string &one, const std::string &other)
{
	return toUpper (one) == toUpper (other);
}

#endif
