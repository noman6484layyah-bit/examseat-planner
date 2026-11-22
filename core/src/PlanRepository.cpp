#include "examseat/PlanRepository.h"

#include <stdexcept>

namespace examseat {

int MemoryPlanRepository::save(const SeatingPlan& plan) {
    plans_.push_back(plan);
    return static_cast<int>(plans_.size());
}

SeatingPlan MemoryPlanRepository::load(int version) const {
    if (version < 1 || version > static_cast<int>(plans_.size()))
        throw std::out_of_range("MemoryPlanRepository: no such version");
    return plans_[static_cast<std::size_t>(version) - 1];
}

std::vector<int> MemoryPlanRepository::versions() const {
    std::vector<int> v;
    for (std::size_t i = 1; i <= plans_.size(); ++i) v.push_back(static_cast<int>(i));
    return v;
}

}  // namespace examseat
