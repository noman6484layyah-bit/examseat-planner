#pragma once
// Tiny test helpers shared by every test file (no external library needed).
// assert() is switched off in release builds, so we use our own CHECK that always runs.
#include <iostream>
#include <stdexcept>

inline int g_failures = 0;

#define CHECK(cond)                                                                   \
    do {                                                                              \
        if (!(cond)) {                                                                \
            std::cout << "FAIL " << __FILE__ << ":" << __LINE__ << ": " #cond "\n";   \
            ++g_failures;                                                             \
        }                                                                             \
    } while (0)

template <typename F>
bool throwsInvalid(F f) {
    try { f(); } catch (const std::invalid_argument&) { return true; } catch (...) {}
    return false;
}

inline int finish(const char* suite) {
    if (g_failures == 0) std::cout << suite << ": all tests passed\n";
    return g_failures == 0 ? 0 : 1;
}
