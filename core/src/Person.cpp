#include "examseat/Person.h"

#include <stdexcept>
#include <utility>

#include "examseat/StringUtil.h"

namespace examseat {

// Ids are trimmed and upper-cased here, in ONE place, so " s1 " and "S1" are the same
// person everywhere in the program. Names are only trimmed.
Person::Person(std::string id, std::string name)
    : id_(toUpper(trim(id))), name_(trim(name)) {
    if (id_.empty()) throw std::invalid_argument("Person: id must not be empty");
    if (name_.empty()) throw std::invalid_argument("Person: name must not be empty");
}

}  // namespace examseat
