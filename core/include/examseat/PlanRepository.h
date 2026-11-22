#pragma once
#include <vector>

#include "examseat/SeatingPlan.h"

namespace examseat {

// Abstract storage contract. The rest of the program only ever talks to this class,
// so we can start with files and add a database later without rewriting anything.
//   FilePlanRepository : C++ file handling (to be written)
//   DbPlanRepository   : PostgreSQL, only if the instructor allows it (later)
//   MemoryPlanRepository below: keeps plans in RAM, used by the tests
class PlanRepository {
public:
    virtual ~PlanRepository() = default;

    // Stores the plan as a NEW version and returns its number (1, 2, 3, ...).
    // Old versions are never overwritten. Throws std::runtime_error if writing fails.
    virtual int save(const SeatingPlan& plan) = 0;

    // Throws std::out_of_range if the version does not exist,
    // std::runtime_error if the stored data cannot be read.
    virtual SeatingPlan load(int version) const = 0;

    virtual std::vector<int> versions() const = 0;   // all stored version numbers, ascending
};

class MemoryPlanRepository : public PlanRepository {
public:
    int save(const SeatingPlan& plan) override;
    SeatingPlan load(int version) const override;
    std::vector<int> versions() const override;

private:
    std::vector<SeatingPlan> plans_;
};

}  // namespace examseat
