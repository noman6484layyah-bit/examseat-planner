#include "examseat/StringUtil.h"

#include <cctype>

namespace examseat {

std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";
    std::size_t first = s.find_first_not_of(ws);
    if (first == std::string::npos) return "";
    std::size_t last = s.find_last_not_of(ws);
    return s.substr(first, last - first + 1);
}

std::string toUpper(std::string s) {
    for (char& ch : s) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return s;
}

}  // namespace examseat
