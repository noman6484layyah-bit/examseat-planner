#pragma once
#include <memory>
#include <string>
#include <vector>

#include "examseat/Invigilator.h"
#include "examseat/Room.h"
#include "examseat/Student.h"
#include "examseat/TimeSlot.h"

namespace examseat {

// Everything that takes part in ONE exam slot. The session OWNS its students, rooms and
// invigilators (through unique_ptr), so the raw pointers inside Room seats stay valid
// as long as the session lives. Build it once (CSV reader), then hand it to the engine.
class ExamSession {
public:
    explicit ExamSession(TimeSlot slot) : slot_(std::move(slot)) {}

    const TimeSlot& slot() const { return slot_; }

    // Each add returns false (and keeps ownership with the caller) if the pointer is
    // null or the id/name already exists. Ids are already normalised by Person.
    bool addStudent(std::unique_ptr<Student> s);
    bool addRoom(std::unique_ptr<Room> r);
    bool addInvigilator(std::unique_ptr<Invigilator> i);

    const std::vector<std::unique_ptr<Student>>& students() const { return students_; }
    // Rooms and invigilators are non-const on purpose: the engine places students into
    // the room grids and records duties on the invigilators.
    std::vector<std::unique_ptr<Room>>& rooms() { return rooms_; }
    const std::vector<std::unique_ptr<Room>>& rooms() const { return rooms_; }
    std::vector<std::unique_ptr<Invigilator>>& invigilators() { return invigilators_; }
    const std::vector<std::unique_ptr<Invigilator>>& invigilators() const { return invigilators_; }

    const Student* findStudent(const std::string& id) const;   // nullptr if unknown
    int totalCapacity() const;                                  // available seats, all rooms

private:
    TimeSlot slot_;
    std::vector<std::unique_ptr<Student>> students_;
    std::vector<std::unique_ptr<Room>> rooms_;
    std::vector<std::unique_ptr<Invigilator>> invigilators_;
};

}  // namespace examseat
