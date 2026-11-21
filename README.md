# ExamSeat: Exam Seating and Invigilation Planner

A C++ program that builds exam seating plans and assigns invigilators automatically.
Rule example: no two students of the same course sit next to each other.
If no valid plan exists, it explains why instead of crashing.

> Status: **Phase 0 (setup)**. Core classes and tests exist; the placement engine is next.

## Build and run

Requires: a C++17 compiler (g++), CMake 3.16+, Python 3 (for test data only).

```bash
# Windows (MinGW):
cmake -S . -B build -G "MinGW Makefiles"
# Linux / macOS:
# cmake -S . -B build

cmake --build build
ctest --test-dir build --output-on-failure     # run the tests
./build/console_app/examseat                   # Windows: build\console_app\examseat.exe
```

## Generate test data

```bash
python tools/generate_data.py --edge      # writes data/generated/ (normal sets + edge cases)
```

Each folder in `data/generated/edge/` has an `EXPECTED.txt` saying what the program must do.

## Project layout

| Folder | What it holds |
|---|---|
| `core/` | All logic as a library: classes, placement, checking. No keyboard or screen code. |
| `console_app/` | The console menu that calls `core`. |
| `tests/` | Automated tests (`ctest`). |
| `tools/` | Test-data generator. |
| `data/sample/` | Small sample files committed to the repo. |

## Team workflow

- Never commit directly to `main`. Make a branch (`feature/placement-engine`), open a pull request, and have one teammate review it.
- Commit small and often, from your own GitHub account.
- Run `ctest` before every pull request. A red test blocks the merge.
- Never commit passwords, `.env` files or real student data.

## Team

| Member | Area |
|---|---|
| Noman Ahmad (514634) | OOP design, storage layer, integration and review |
| Muhammad Sulaiman Ali (595239) | To be agreed |
| [Third member] | To be agreed |
