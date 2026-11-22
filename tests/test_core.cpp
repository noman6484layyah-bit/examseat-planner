#include "check.h"
#include "examseat/Invigilator.h"
#include "examseat/Room.h"
#include "examseat/Student.h"

using namespace examseat;

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

    // --- normalisation: ids trimmed and upper-cased, courses upper-cased ---
    Student messy("  s1 ", " Ali Khan ", " cs ");
    CHECK(messy.id() == "S1");
    CHECK(messy.name() == "Ali Khan");
    CHECK(messy.course() == "CS");
    CHECK(throwsInvalid([] { Student("   ", "Ali", "CS"); }));

    return finish("core");
}
