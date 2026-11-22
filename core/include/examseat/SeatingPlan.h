#pragma once
#include <string>
#include <vector>

#include "examseat/TimeSlot.h"

namespace examseat {

struct SeatAssignment {
    std::string studentId;
    std::string roomName;
    int row;   // 0-based inside the program. Files and reports show 1-based.
    int col;
};

struct DutyAssignment {
    std::string invigilatorId;
    std::string roomName;
};

// The RESULT of planning one slot: who sits where, who supervises which room.
// It only stores ids and names (no pointers), so it can be saved, reloaded and
// compared without the original objects.
//
// The add methods refuse anything that would break the structure, so a plan can never
// contain a double-booked seat, a student with two seats, or an invigilator in two
// rooms. (The ConflictChecker still re-verifies the seating rules independently.)
class SeatingPlan {
public:
    explicit SeatingPlan(TimeSlot slot) : slot_(std::move(slot)) {}

    const TimeSlot& slot() const { return slot_; }

    // false if: negative row/col, empty ids, the student is already seated,
    // or that seat in that room is already taken.
    bool addSeat(const std::string& studentId, const std::string& roomName, int row, int col);

    // false if: empty ids, the room already has an invigilator, or this invigilator
    // already supervises another room (one plan = one slot, so that would be a clash).
    bool addDuty(const std::string& invigilatorId, const std::string& roomName);

    const std::vector<SeatAssignment>& seats() const { return seats_; }
    const std::vector<DutyAssignment>& duties() const { return duties_; }

    const SeatAssignment* findStudent(const std::string& studentId) const;   // nullptr if not seated
    const DutyAssignment* dutyForRoom(const std::string& roomName) const;    // nullptr if unstaffed
    std::vector<SeatAssignment> seatsInRoom(const std::string& roomName) const;

private:
    TimeSlot slot_;
    std::vector<SeatAssignment> seats_;
    std::vector<DutyAssignment> duties_;
};

}  // namespace examseat
