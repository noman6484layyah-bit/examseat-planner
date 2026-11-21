// Tiny test harness with no external library. (assert() is disabled in release builds,
// so we use our own CHECK macro that always runs.)
#include <iostream>
#include <stdexcept>

#include "examseat/Invigilator.h"
#include "examseat/Room.h"
#include "examseat/Student.h"

using namespace examseat;

static int failures = 0;
#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::cout << "FAIL line " << __LINE__ << ": " #cond "\n";            \
            ++failures;                                                          \
        }                                                                        \
    } while (0)

template <typename F>
static bool throwsInvalid(F f) {
    try { f(); } catch (const std::invalid_argument&) { return true; } catch (...) {}
    return false;
}

int main() {
    // --- Person / Student / Invigilator ---
    CHECK(throwsInvalid([] { Student("", "Ali", "CS"); }));
    CHECK(throwsInvalid([] { Student("CS-01", "", "CS"); }));
    CHECK(throwsInvalid([] { Student("CS-01", "Ali", ""); }));
    Student ali("CS-01", "Ali", "CS");
    Invigilator drKhan("I-01", "Dr. Khan");
    CHECK(ali.role() == "Student");
    CHECK(drKhan.role() == "Invigilator");
    const Person& asPerson = ali;           // polymorphism through a base reference
    CHECK(asPerson.role() == "Student");

    // --- Room construction ---
    CHECK(throwsInvalid([] { LectureHall("A", 0, 3); }));
    CHECK(throwsInvalid([] { LectureHall("A", 3, -1); }));
    CHECK(throwsInvalid([] { LectureHall("", 3, 3); }));
    LectureHall room("A", 2, 3);
    CHECK(room.capacity() == 6);
    CHECK(room.type() == "Lecture Hall");

    // --- place / bounds / broken seats ---
    Student bob("CS-02", "Bob", "CS");
    CHECK(room.place(ali, 0, 0));
    CHECK(!room.place(bob, 0, 0));          // already taken
    CHECK(!room.place(bob, -1, 0));         // out of bounds
    CHECK(!room.place(bob, 0, 3));          // out of bounds
    CHECK(room.markBroken(1, 2));
    CHECK(room.capacity() == 5);
    CHECK(!room.place(bob, 1, 2));          // broken
    CHECK(!room.markBroken(0, 0));          // occupied, cannot break
    room.clearSeat(0, 0);
    CHECK(room.occupant(0, 0) == nullptr);

    // --- neighbour rule, including edges and corners ---
    LectureHall g("G", 3, 3);
    Student s1("CS-10", "S1", "CS"), s2("MA-10", "S2", "MA");
    g.place(s1, 1, 1);
    CHECK(g.hasSameCourseNeighbour(0, 1, "CS"));
    CHECK(g.hasSameCourseNeighbour(2, 1, "CS"));
    CHECK(g.hasSameCourseNeighbour(1, 0, "CS"));
    CHECK(g.hasSameCourseNeighbour(1, 2, "CS"));
    CHECK(!g.hasSameCourseNeighbour(0, 0, "CS"));   // diagonal is allowed
    CHECK(!g.hasSameCourseNeighbour(0, 1, "MA"));   // different course
    g.place(s2, 0, 0);
    CHECK(!g.hasSameCourseNeighbour(2, 2, "CS"));   // far corner, no crash
    CHECK(!g.hasSameCourseNeighbour(-1, -1, "CS")); // out of bounds, no crash

    // --- feasibility bound (chessboard) ---
    CHECK(Room::maxSeatsForOneCourse(1, 1) == 1);
    CHECK(Room::maxSeatsForOneCourse(2, 3) == 3);
    CHECK(Room::maxSeatsForOneCourse(3, 3) == 5);
    CHECK(Room::maxSeatsForOneCourse(0, 5) == 0);

    if (failures == 0) std::cout << "All tests passed\n";
    return failures == 0 ? 0 : 1;
}
