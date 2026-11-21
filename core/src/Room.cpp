#include "examseat/Room.h"

#include <stdexcept>
#include <utility>

namespace examseat {

Room::Room(std::string name, int rows, int cols)
    : name_(std::move(name)), rows_(rows), cols_(cols) {
    if (name_.empty()) throw std::invalid_argument("Room: name must not be empty");
    if (rows <= 0 || cols <= 0)
        throw std::invalid_argument("Room: rows and cols must be positive");
    seats_.resize(static_cast<std::size_t>(rows) * cols);
}

bool Room::inBounds(int r, int c) const {
    return r >= 0 && r < rows_ && c >= 0 && c < cols_;
}

int Room::capacity() const {
    int n = 0;
    for (const Seat& s : seats_) if (s.available) ++n;
    return n;
}

bool Room::isFree(int r, int c) const {
    if (!inBounds(r, c)) return false;
    const Seat& s = seats_[index(r, c)];
    return s.available && s.occupant == nullptr;
}

bool Room::markBroken(int r, int c) {
    if (!inBounds(r, c)) return false;
    Seat& s = seats_[index(r, c)];
    if (s.occupant != nullptr) return false;
    s.available = false;
    return true;
}

bool Room::place(const Student& student, int r, int c) {
    if (!isFree(r, c)) return false;
    seats_[index(r, c)].occupant = &student;
    return true;
}

void Room::clearSeat(int r, int c) {
    if (inBounds(r, c)) seats_[index(r, c)].occupant = nullptr;
}

const Student* Room::occupant(int r, int c) const {
    if (!inBounds(r, c)) return nullptr;
    return seats_[index(r, c)].occupant;
}

bool Room::hasSameCourseNeighbour(int r, int c, const std::string& course) const {
    static const int dr[4] = {-1, 1, 0, 0};
    static const int dc[4] = {0, 0, -1, 1};
    for (int i = 0; i < 4; ++i) {
        const Student* s = occupant(r + dr[i], c + dc[i]);   // out of bounds -> nullptr
        if (s != nullptr && s->course() == course) return true;
    }
    return false;
}

int Room::maxSeatsForOneCourse(int rows, int cols) {
    if (rows <= 0 || cols <= 0) return 0;
    return (rows * cols + 1) / 2;
}

}  // namespace examseat
