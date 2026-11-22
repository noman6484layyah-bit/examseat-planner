#include "examseat/Invigilator.h"

namespace examseat {

bool Invigilator::isAvailableFor(const TimeSlot& slot) const {
    for (const TimeSlot& window : availability_)
        if (window.contains(slot)) return true;
    return false;
}

}  // namespace examseat
