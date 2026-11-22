#include "examseat/Student.h"

#include <stdexcept>
#include <utility>

#include "examseat/StringUtil.h"

namespace examseat {

// Course codes are trimmed and upper-cased, so "cs" and "CS " are the same course.
Student::Student(std::string id, std::string name, std::string course)
    : Person(std::move(id), std::move(name)), course_(toUpper(trim(course))) {
    if (course_.empty()) throw std::invalid_argument("Student: course must not be empty");
}

}  // namespace examseat
