#include "examseat/SeatingPlan.h"

namespace examseat {

bool SeatingPlan::addSeat(const std::string& studentId, const std::string& roomName, int row, int col) {
    if (studentId.empty() || roomName.empty() || row < 0 || col < 0) return false;
    // Linear searches are fine here: 5,000 students means about 25 million comparisons.
    for (const SeatAssignment& a : seats_) {
        if (a.studentId == studentId) return false;                                   // already seated
        if (a.roomName == roomName && a.row == row && a.col == col) return false;     // seat taken
    }
    seats_.push_back({studentId, roomName, row, col});
    return true;
}

bool SeatingPlan::addDuty(const std::string& invigilatorId, const std::string& roomName) {
    if (invigilatorId.empty() || roomName.empty()) return false;
    for (const DutyAssignment& d : duties_) {
        if (d.roomName == roomName) return false;             // room already staffed
        if (d.invigilatorId == invigilatorId) return false;   // same slot, second room = clash
    }
    duties_.push_back({invigilatorId, roomName});
    return true;
}

const SeatAssignment* SeatingPlan::findStudent(const std::string& studentId) const {
    for (const SeatAssignment& a : seats_)
        if (a.studentId == studentId) return &a;
    return nullptr;
}

const DutyAssignment* SeatingPlan::dutyForRoom(const std::string& roomName) const {
    for (const DutyAssignment& d : duties_)
        if (d.roomName == roomName) return &d;
    return nullptr;
}

std::vector<SeatAssignment> SeatingPlan::seatsInRoom(const std::string& roomName) const {
    std::vector<SeatAssignment> out;
    for (const SeatAssignment& a : seats_)
        if (a.roomName == roomName) out.push_back(a);
    return out;
}

}  // namespace examseat
