#pragma once
#include "examseat/Person.h"

namespace examseat {

class Student : public Person {
public:
    // Throws std::invalid_argument if the course is empty.
    Student(std::string id, std::string name, std::string course);

    std::string role() const override { return "Student"; }
    const std::string& course() const { return course_; }

private:
    std::string course_;
};

}  // namespace examseat
