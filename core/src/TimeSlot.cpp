#include "examseat/TimeSlot.h"

#include <cctype>
#include <stdexcept>

#include "examseat/StringUtil.h"

namespace examseat {
namespace {

bool digitsAt(const std::string& s, std::size_t pos, std::size_t len) {
    if (pos + len > s.size()) return false;
    for (std::size_t i = pos; i < pos + len; ++i)
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    return true;
}

int numberAt(const std::string& s, std::size_t pos, std::size_t len) {
    int v = 0;
    for (std::size_t i = pos; i < pos + len; ++i) v = v * 10 + (s[i] - '0');
    return v;
}

bool isLeap(int y) { return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0; }

int daysInMonth(int y, int m) {
    static const int d[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return (m == 2 && isLeap(y)) ? 29 : d[m - 1];
}

// "YYYY-MM-DD", a real calendar date (2027-02-29 is rejected, 2028-02-29 is fine).
bool validDate(const std::string& d) {
    if (d.size() != 10 || d[4] != '-' || d[7] != '-') return false;
    if (!digitsAt(d, 0, 4) || !digitsAt(d, 5, 2) || !digitsAt(d, 8, 2)) return false;
    int y = numberAt(d, 0, 4), m = numberAt(d, 5, 2), day = numberAt(d, 8, 2);
    return y >= 1 && m >= 1 && m <= 12 && day >= 1 && day <= daysInMonth(y, m);
}

// "HH:MM" with exactly two digits each, 00:00 to 23:59.
bool parseClock(const std::string& s, int& minutes) {
    if (s.size() != 5 || s[2] != ':' || !digitsAt(s, 0, 2) || !digitsAt(s, 3, 2)) return false;
    int h = numberAt(s, 0, 2), m = numberAt(s, 3, 2);
    if (h > 23 || m > 59) return false;
    minutes = h * 60 + m;
    return true;
}

std::string clock(int minutes) {
    int h = minutes / 60, m = minutes % 60;
    return std::string(h < 10 ? "0" : "") + std::to_string(h) + ":" +
           (m < 10 ? "0" : "") + std::to_string(m);
}

}  // namespace

TimeSlot::TimeSlot(std::string date, int startMinute, int endMinute)
    : date_(trim(date)), start_(startMinute), end_(endMinute) {
    if (!validDate(date_))
        throw std::invalid_argument("TimeSlot: invalid date '" + date_ + "' (use YYYY-MM-DD)");
    if (start_ < 0 || end_ > 23 * 60 + 59 || start_ >= end_)
        throw std::invalid_argument("TimeSlot: the end time must be after the start time");
}

TimeSlot TimeSlot::parse(const std::string& text) {
    const std::string t = trim(text);
    const std::string expected = "expected 'YYYY-MM-DD HH:MM-HH:MM' but got '" + t + "'";
    std::size_t space = t.find(' ');
    if (space == std::string::npos) throw std::invalid_argument("TimeSlot: " + expected);
    std::string date = t.substr(0, space);
    std::string times = trim(t.substr(space + 1));
    if (times.size() != 11 || times[5] != '-') throw std::invalid_argument("TimeSlot: " + expected);
    int start = 0, end = 0;
    if (!parseClock(times.substr(0, 5), start) || !parseClock(times.substr(6), end))
        throw std::invalid_argument("TimeSlot: " + expected);
    return TimeSlot(date, start, end);   // validates the date and the order of the times
}

bool TimeSlot::overlaps(const TimeSlot& o) const {
    return date_ == o.date_ && start_ < o.end_ && o.start_ < end_;   // touching is NOT overlap
}

bool TimeSlot::contains(const TimeSlot& o) const {
    return date_ == o.date_ && start_ <= o.start_ && o.end_ <= end_;
}

std::string TimeSlot::toString() const {
    return date_ + " " + clock(start_) + "-" + clock(end_);
}

}  // namespace examseat
