#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <algorithm>
#include <cctype>
#
// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), s.end());
}

// trim both ends (in place)
inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

inline int fm_hex(const unsigned char x) {
    if ((x >= '0') && (x <= '9'))
        return x - '0';
    else if ((x >= 'a') && (x <= 'f'))
        return x - 'a' + 10;
    else if ((x >= 'A') && (x <= 'F'))
        return x - 'A' + 10;
    else
        return -1;
}

inline std::string to_hex(const unsigned char x) {
    static const char rg[]{ "0123456789ABCDEF" };
    const char s[] {rg[x >> 8], rg[x & 15], '\0'};
    return std::string(s);
}

inline bool is_tokenchar(const unsigned char x) {
    return ((x >= '0' && x <= '9') || (x >= 'a' && x <= 'z') ||
            (x >= 'A' && x <= 'Z') || x == '_');
}

#endif // UTILS_HPP
