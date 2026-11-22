#include "examseat/ExamSession.h"

#include <utility>

namespace examseat {

bool ExamSession::addStudent(std::unique_ptr<Student> s) {
    if (!s) return false;
    for (const auto& existing : students_)
        if (existing->id() == s->id()) return false;
    students_.push_back(std::move(s));
    return true;
}

bool ExamSession::addRoom(std::unique_ptr<Room> r) {
    if (!r) return false;
    for (const auto& existing : rooms_)
        if (existing->name() == r->name()) return false;
    rooms_.push_back(std::move(r));
    return true;
}

bool ExamSession::addInvigilator(std::unique_ptr<Invigilator> i) {
    if (!i) return false;
    for (const auto& existing : invigilators_)
        if (existing->id() == i->id()) return false;
    invigilators_.push_back(std::move(i));
    return true;
}

const Student* ExamSession::findStudent(const std::string& id) const {
    for (const auto& s : students_)
        if (s->id() == id) return s.get();
    return nullptr;
}

int ExamSession::totalCapacity() const {
    int total = 0;
    for (const auto& r : rooms_) total += r->capacity();
    return total;
}

}  // namespace examseat
