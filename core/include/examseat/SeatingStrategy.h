#pragma once
#include <string>
#include <vector>

#include "examseat/Room.h"
#include "examseat/Student.h"

namespace examseat {

// Abstract seating rule. The placement engine works with ANY SeatingStrategy and never
// knows which one (polymorphism), so a new rule is a new class and nothing else changes.
class SeatingStrategy {
public:
    virtual ~SeatingStrategy() = default;

    virtual std::string name() const = 0;

    // May this student sit at (r, c) of this room right now?
    virtual bool canSit(const Student& s, const Room& room, int r, int c) const = 0;

    // Optional hook: reorder the students before placement (e.g. by roll number).
    // The default leaves the order unchanged.
    virtual void orderStudents(std::vector<const Student*>& /*students*/) const {}
};

// Reference strategy: the seat must be free and no neighbour may share the course.
class AlternateCourseStrategy : public SeatingStrategy {
public:
    std::string name() const override { return "Alternate course"; }
    bool canSit(const Student& s, const Room& room, int r, int c) const override;
};

}  // namespace examseat
