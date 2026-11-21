#pragma once
#include "examseat/Person.h"

namespace examseat {

class Invigilator : public Person {
public:
    using Person::Person;   // same constructor as Person for now

    std::string role() const override { return "Invigilator"; }
    // Later: free time slots and the number of duties already assigned.
};

}  // namespace examseat
