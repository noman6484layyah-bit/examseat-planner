#pragma once
#include <string>

namespace examseat {

// A time window on one calendar day, e.g. "2026-12-10 09:00-12:00".
// Immutable value type: once created it never changes.
//
// Rules (see docs/SPEC.md):
//  - Touching slots do NOT overlap: 09:00-12:00 and 12:00-14:00 are fine together.
//  - A slot must end after it starts, and both times lie within 00:00-23:59.
class TimeSlot {
public:
    // Throws std::invalid_argument for a bad date or start >= end.
    // Minutes are counted from midnight (09:00 = 540).
    TimeSlot(std::string date, int startMinute, int endMinute);

    // Parses "YYYY-MM-DD HH:MM-HH:MM". Throws std::invalid_argument with a message
    // that says what was wrong, so the CSV reader can put it in rejected_rows.txt.
    static TimeSlot parse(const std::string& text);

    const std::string& date() const { return date_; }
    int startMinute() const { return start_; }
    int endMinute() const { return end_; }

    bool overlaps(const TimeSlot& other) const;   // same day and the windows share time
    bool contains(const TimeSlot& other) const;   // other lies fully inside this one

    std::string toString() const;                 // same format that parse() reads
    bool operator==(const TimeSlot& o) const {
        return date_ == o.date_ && start_ == o.start_ && end_ == o.end_;
    }

private:
    std::string date_;
    int start_;
    int end_;
};

}  // namespace examseat
