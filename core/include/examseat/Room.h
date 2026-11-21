#pragma once
#include <string>
#include <vector>

#include "examseat/Student.h"

namespace examseat {

// One seat in a room grid.
struct Seat {
    bool available = true;               // false = broken or blocked seat
    const Student* occupant = nullptr;   // not owned: the Student must outlive the Room
};

// Abstract room made of a rows x cols grid of seats.
// The grid is private: the only way to change it is through checked methods,
// so an invalid assignment cannot happen (encapsulation).
class Room {
public:
    // Throws std::invalid_argument if rows or cols is not positive, or the name is empty.
    Room(std::string name, int rows, int cols);
    virtual ~Room() = default;

    virtual std::string type() const = 0;

    const std::string& name() const { return name_; }
    int rows() const { return rows_; }
    int cols() const { return cols_; }

    bool inBounds(int r, int c) const;
    int capacity() const;                 // number of available (not broken) seats
    bool isFree(int r, int c) const;      // in bounds, available and empty

    bool markBroken(int r, int c);        // false if out of bounds or already occupied
    bool place(const Student& s, int r, int c);   // false if the seat is not free
    void clearSeat(int r, int c);         // no effect if out of bounds
    const Student* occupant(int r, int c) const;  // nullptr if empty or out of bounds

    // True if a seat directly left, right, in front or behind (r, c) holds a student
    // of the given course. This is the heart of the "no same-course neighbours" rule.
    bool hasSameCourseNeighbour(int r, int c, const std::string& course) const;

    // Upper bound on how many students of ONE course fit in a rows x cols grid when
    // neighbours must differ: a chessboard pattern, so ceil(rows * cols / 2).
    // Used by the feasibility check ("impossible, here is why").
    static int maxSeatsForOneCourse(int rows, int cols);

private:
    std::size_t index(int r, int c) const { return static_cast<std::size_t>(r) * cols_ + c; }

    std::string name_;
    int rows_;
    int cols_;
    std::vector<Seat> seats_;
};

class LectureHall : public Room {
public:
    using Room::Room;
    std::string type() const override { return "Lecture Hall"; }
};

class ComputerLab : public Room {
public:
    using Room::Room;
    std::string type() const override { return "Computer Lab"; }
};

}  // namespace examseat
