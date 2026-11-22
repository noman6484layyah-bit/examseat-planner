#include <memory>
#include <stdexcept>

#include "check.h"
#include "examseat/ExamSession.h"
#include "examseat/PlanRepository.h"
#include "examseat/SeatingPlan.h"
#include "examseat/SeatingStrategy.h"
#include "examseat/TimeSlot.h"

using namespace examseat;

static bool parseFails(const char* text) {
    return throwsInvalid([&] { TimeSlot::parse(text); });
}

static void testTimeSlot() {
    TimeSlot a = TimeSlot::parse("2026-12-10 09:00-12:00");
    CHECK(a.startMinute() == 540 && a.endMinute() == 720);
    CHECK(a.toString() == "2026-12-10 09:00-12:00");
    CHECK(TimeSlot::parse("  2026-12-10 09:00-12:00 \r").toString() == a.toString());   // CRLF safe

    CHECK(parseFails(""));
    CHECK(parseFails("2026-12-10"));
    CHECK(parseFails("10am-12pm"));
    CHECK(parseFails("2026-12-10 9:00-12:00"));       // one-digit hour
    CHECK(parseFails("2026-12-10 09:00 - 12:00"));    // spaces around the dash
    CHECK(parseFails("2026-12-10 24:00-25:00"));
    CHECK(parseFails("2026-12-10 09:60-12:00"));
    CHECK(parseFails("2026-12-10 12:00-09:00"));      // ends before it starts
    CHECK(parseFails("2026-12-10 09:00-09:00"));      // zero length
    CHECK(parseFails("2026-13-01 09:00-12:00"));      // month 13
    CHECK(parseFails("2026-02-30 09:00-12:00"));      // no such day
    CHECK(parseFails("2027-02-29 09:00-12:00"));      // not a leap year
    CHECK(!parseFails("2028-02-29 09:00-12:00"));     // leap year
    CHECK(throwsInvalid([] { TimeSlot("2026-12-10", -5, 60); }));

    TimeSlot morning = TimeSlot::parse("2026-12-10 09:00-12:00");
    TimeSlot noon = TimeSlot::parse("2026-12-10 12:00-14:00");
    TimeSlot overlap = TimeSlot::parse("2026-12-10 11:00-13:00");
    TimeSlot nextDay = TimeSlot::parse("2026-12-11 09:00-12:00");
    CHECK(!morning.overlaps(noon));      // back-to-back is NOT a clash
    CHECK(!noon.overlaps(morning));
    CHECK(morning.overlaps(overlap));
    CHECK(overlap.overlaps(morning));
    CHECK(morning.overlaps(morning));
    CHECK(!morning.overlaps(nextDay));   // different day
    TimeSlot window = TimeSlot::parse("2026-12-10 08:00-13:00");
    CHECK(window.contains(morning));
    CHECK(!window.contains(noon));                                  // 12:00-14:00 sticks out past 13:00
    CHECK(window.contains(TimeSlot::parse("2026-12-10 12:00-13:00")));
    CHECK(window.contains(window));                                 // a slot contains itself
    CHECK(!morning.contains(window));
    CHECK(!window.contains(nextDay));
    CHECK(morning == TimeSlot::parse("2026-12-10 09:00-12:00"));
}

static void testInvigilator() {
    Invigilator inv("i1", "Dr. Khan");
    CHECK(inv.id() == "I1");
    TimeSlot exam = TimeSlot::parse("2026-12-10 09:00-12:00");
    CHECK(!inv.isAvailableFor(exam));                       // no availability yet
    inv.addAvailability(TimeSlot::parse("2026-12-10 08:00-13:00"));
    CHECK(inv.isAvailableFor(exam));
    CHECK(!inv.isAvailableFor(TimeSlot::parse("2026-12-10 12:00-14:00")));   // sticks out
    CHECK(inv.dutyCount() == 0);
    inv.addDuty();
    CHECK(inv.dutyCount() == 1);
}

static void testSeatingPlan() {
    SeatingPlan plan(TimeSlot::parse("2026-12-10 09:00-12:00"));
    CHECK(plan.addSeat("S1", "A", 0, 0));
    CHECK(!plan.addSeat("S1", "A", 0, 1));    // student already seated
    CHECK(!plan.addSeat("S2", "A", 0, 0));    // seat taken
    CHECK(plan.addSeat("S2", "B", 0, 0));     // same coordinates, other room: fine
    CHECK(!plan.addSeat("S3", "A", -1, 0));
    CHECK(!plan.addSeat("", "A", 1, 1));
    CHECK(!plan.addSeat("S3", "", 1, 1));
    CHECK(plan.seats().size() == 2);
    CHECK(plan.findStudent("S2") != nullptr && plan.findStudent("S2")->roomName == "B");
    CHECK(plan.findStudent("S9") == nullptr);
    CHECK(plan.seatsInRoom("A").size() == 1);
    CHECK(plan.seatsInRoom("Z").empty());

    CHECK(plan.addDuty("I1", "A"));
    CHECK(!plan.addDuty("I2", "A"));          // room already staffed
    CHECK(!plan.addDuty("I1", "B"));          // same invigilator, same slot, second room
    CHECK(plan.addDuty("I2", "B"));
    CHECK(plan.dutyForRoom("B")->invigilatorId == "I2");
    CHECK(plan.dutyForRoom("C") == nullptr);
}

static void testExamSession() {
    ExamSession session(TimeSlot::parse("2026-12-10 09:00-12:00"));
    CHECK(session.addStudent(std::make_unique<Student>("s1", "Ali", "cs")));
    CHECK(!session.addStudent(std::make_unique<Student>("S1", "Ali Again", "MA")));   // same id after normalising
    CHECK(!session.addStudent(nullptr));
    CHECK(session.students().size() == 1);
    CHECK(session.findStudent("S1") != nullptr);
    CHECK(session.findStudent("nope") == nullptr);

    CHECK(session.addRoom(std::make_unique<LectureHall>("A", 2, 3)));
    CHECK(session.addRoom(std::make_unique<ComputerLab>("B", 1, 1)));
    CHECK(!session.addRoom(std::make_unique<LectureHall>("A", 5, 5)));                // duplicate name
    CHECK(session.totalCapacity() == 7);
    session.rooms()[0]->markBroken(0, 0);
    CHECK(session.totalCapacity() == 6);

    CHECK(session.addInvigilator(std::make_unique<Invigilator>("I1", "Dr. Khan")));
    CHECK(!session.addInvigilator(std::make_unique<Invigilator>("i1", "Dr. Khan 2")));
}

static void testStrategy() {
    LectureHall room("A", 2, 2);
    Student cs1("CS-1", "A", "CS"), cs2("CS-2", "B", "CS"), ma1("MA-1", "C", "MA");
    AlternateCourseStrategy alt;
    const SeatingStrategy& rule = alt;               // used through the base class
    CHECK(rule.name() == "Alternate course");
    CHECK(rule.canSit(cs1, room, 0, 0));
    room.place(cs1, 0, 0);
    CHECK(!rule.canSit(cs2, room, 0, 1));            // same course next to cs1
    CHECK(!rule.canSit(cs2, room, 1, 0));
    CHECK(rule.canSit(cs2, room, 1, 1));             // diagonal is fine
    CHECK(rule.canSit(ma1, room, 0, 1));
    CHECK(!rule.canSit(ma1, room, 0, 0));            // occupied
    CHECK(!rule.canSit(ma1, room, 5, 5));            // out of bounds
    std::vector<const Student*> order{&cs1, &cs2, &ma1};
    rule.orderStudents(order);                       // default hook keeps the order
    CHECK(order[0] == &cs1 && order[2] == &ma1);
}

static void testRepository() {
    MemoryPlanRepository memory;
    PlanRepository& repo = memory;                   // callers only know the base class
    CHECK(repo.versions().empty());
    CHECK(throwsInvalid([] { throw std::invalid_argument("self-check of the helper"); }));

    SeatingPlan p1(TimeSlot::parse("2026-12-10 09:00-12:00"));
    p1.addSeat("S1", "A", 0, 0);
    SeatingPlan p2 = p1;
    p2.addSeat("S2", "A", 0, 1);
    CHECK(repo.save(p1) == 1);
    CHECK(repo.save(p2) == 2);
    CHECK(repo.versions().size() == 2);
    CHECK(repo.load(1).seats().size() == 1);         // version 1 was not overwritten
    CHECK(repo.load(2).seats().size() == 2);
    bool threw = false;
    try { repo.load(3); } catch (const std::out_of_range&) { threw = true; }
    CHECK(threw);
    threw = false;
    try { repo.load(0); } catch (const std::out_of_range&) { threw = true; }
    CHECK(threw);
}

int main() {
    testTimeSlot();
    testInvigilator();
    testSeatingPlan();
    testExamSession();
    testStrategy();
    testRepository();
    return finish("contracts");
}
