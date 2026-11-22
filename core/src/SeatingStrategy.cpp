#include "examseat/SeatingStrategy.h"

namespace examseat {

bool AlternateCourseStrategy::canSit(const Student& s, const Room& room, int r, int c) const {
    return room.isFree(r, c) && !room.hasSameCourseNeighbour(r, c, s.course());
}

}  // namespace examseat
