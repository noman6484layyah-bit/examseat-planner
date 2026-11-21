#include "examseat/Student.h"

#include <stdexcept>
#include <utility>

namespace examseat {

Student::Student(std::string id, std::string name, std::string course)
    : Person(std::move(id), std::move(name)), course_(std::move(course)) {
    if (course_.empty()) throw std::invalid_argument("Student: course must not be empty");
}

}  // namespace examseat
