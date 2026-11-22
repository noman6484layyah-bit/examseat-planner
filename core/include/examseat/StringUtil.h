#pragma once
#include <string>

namespace examseat {

// Removes spaces, tabs, \r and \n from both ends. (\r matters: files saved on
// Windows end every line with \r\n.)
std::string trim(const std::string& s);

// Upper-cases ASCII letters. Used to compare ids and course codes case-insensitively.
std::string toUpper(std::string s);

}  // namespace examseat
