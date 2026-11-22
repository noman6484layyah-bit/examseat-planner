#pragma once
#include <vector>

#include "examseat/Person.h"
#include "examseat/TimeSlot.h"

namespace examseat {

class Invigilator : public Person {
public:
    using Person::Person;   // same constructor as Person

    std::string role() const override { return "Invigilator"; }

    // Windows in which this person can supervise (from invigilators.csv "slots").
    void addAvailability(const TimeSlot& window) { availability_.push_back(window); }
    const std::vector<TimeSlot>& availability() const { return availability_; }

    // True if the slot lies fully inside one availability window.
    bool isAvailableFor(const TimeSlot& slot) const;

    // Duties assigned so far, so the assigner can pick the least-loaded person.
    int dutyCount() const { return dutyCount_; }
    void addDuty() { ++dutyCount_; }

private:
    std::vector<TimeSlot> availability_;
    int dutyCount_ = 0;
};

}  // namespace examseat
