// Day-1 demo: place 3 CS and 3 MA students in a 2 x 3 room with a simple greedy rule.
// This is NOT the final engine. It just shows the classes working together.
#include <iostream>
#include <memory>
#include <vector>

#include "examseat/Room.h"
#include "examseat/Student.h"

using namespace examseat;

int main() {
    std::vector<std::unique_ptr<Student>> students;
    const char* courses[] = {"CS", "MA"};
    for (const char* course : courses)
        for (int i = 1; i <= 3; ++i) {
            std::string id = std::string(course) + "-0" + std::to_string(i);
            students.push_back(std::make_unique<Student>(id, "Student " + id, course));
        }

    LectureHall room("Room A", 2, 3);
    std::vector<bool> used(students.size(), false);

    for (int r = 0; r < room.rows(); ++r) {
        for (int c = 0; c < room.cols(); ++c) {
            for (std::size_t s = 0; s < students.size(); ++s) {
                if (used[s]) continue;
                if (room.hasSameCourseNeighbour(r, c, students[s]->course())) continue;
                room.place(*students[s], r, c);
                used[s] = true;
                break;
            }
        }
    }

    std::cout << room.name() << " (" << room.type() << ", " << room.rows() << " x "
              << room.cols() << ")\n";
    for (int r = 0; r < room.rows(); ++r) {
        for (int c = 0; c < room.cols(); ++c) {
            const Student* s = room.occupant(r, c);
            std::cout << " [" << (s ? s->id() : "empty") << "]";
        }
        std::cout << "\n";
    }
    return 0;
}
