#include "examseat/Person.h"

#include <stdexcept>
#include <utility>

namespace examseat {

Person::Person(std::string id, std::string name)
    : id_(std::move(id)), name_(std::move(name)) {
    if (id_.empty()) throw std::invalid_argument("Person: id must not be empty");
    if (name_.empty()) throw std::invalid_argument("Person: name must not be empty");
}

}  // namespace examseat
