#pragma once
#include <string>

namespace examseat {

// Abstract base class: a Person cannot be created directly.
// Student and Invigilator inherit from it (inheritance + abstraction).
class Person {
public:
    // Throws std::invalid_argument if id or name is empty.
    Person(std::string id, std::string name);
    virtual ~Person() = default;   // virtual: deleting through a Person* is safe

    virtual std::string role() const = 0;   // each child answers for itself (polymorphism)

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }

private:
    std::string id_;
    std::string name_;
};

}  // namespace examseat
