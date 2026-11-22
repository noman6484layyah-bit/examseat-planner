# ExamSeat specification (v1)

This is the single source of truth for file formats and rules. If code and this file
disagree, fix one of them in a pull request. Do not guess.

## 1. Scope of v1

- One run plans **one exam slot** (one date, one time window). All loaded students sit that exam.
- The slot is given when the program starts (menu option or argument), not stored in the CSV files.
- Several exams per day is a stretch goal, not part of v1.

## 2. Input files (CSV, UTF-8)

General rules for all three files:

- The first row is a header. Columns are found **by header name** (case-insensitive), in any order. Extra columns are ignored.
- A missing **required** column is a fatal error for that file.
- Both `\n` and `\r\n` line endings work. A UTF-8 BOM at the start of the file is ignored.
- Fields may be quoted with `"..."`. Inside quotes a comma is part of the text and `""` means one quote.
- Spaces around every field are trimmed. Completely blank lines are skipped.

### students.csv

| Column | Required | Rule |
|---|---|---|
| `roll_no` | yes | Not empty. Trimmed and upper-cased, so ` s1 ` and `S1` are the same id. Must be unique. |
| `name` | yes | Not empty. Trimmed. |
| `course` | yes | Not empty. Trimmed and upper-cased, so `cs` and `CS` are the same course. |

### rooms.csv

| Column | Required | Rule |
|---|---|---|
| `room` | yes | Not empty, unique (exact match after trimming). |
| `rows` | yes | Whole number, 1 or more. |
| `cols` | yes | Whole number, 1 or more. |
| `type` | yes | `LectureHall` or `ComputerLab` (any letter case). Anything else is rejected. |
| `broken_seats` | no | Seats that cannot be used, written `row:col` separated by `;`, for example `1:2;3:4`. **Row and column are numbered from 1 in files and reports.** The program converts to 0-based internally. A seat outside the room rejects the room row. |

### invigilators.csv

| Column | Required | Rule |
|---|---|---|
| `id` | yes | Not empty, trimmed and upper-cased, unique. |
| `name` | yes | Not empty. |
| `slots` | yes | One or more availability windows separated by `;`, each `YYYY-MM-DD HH:MM-HH:MM`. Every window must parse. |

## 3. Time slots

- Format: `YYYY-MM-DD HH:MM-HH:MM`, 24-hour clock, two digits everywhere. Example: `2026-12-10 09:00-12:00`.
- The date must exist in the calendar (`2027-02-29` is invalid, `2028-02-29` is valid).
- The end must be after the start. Times run from `00:00` to `23:59`.
- **Touching is not overlapping.** `09:00-12:00` and `12:00-14:00` can share an invigilator. `09:00-12:00` and `11:00-13:00` cannot.
- An invigilator can supervise the exam slot only if the slot lies **fully inside** one of their windows.

## 4. Error handling

| Situation | What happens |
|---|---|
| File missing or unreadable | Fatal. Clear message naming the file. Nothing is planned. |
| File empty (no header) or a required column missing | Fatal for that file, with a message. |
| Header only, no data rows | Not an error. The program says there is nothing to seat. |
| A bad data row (missing field, bad number, duplicate id, bad slot...) | That row is **rejected** and the run continues. |

Rejected rows are written to `rejected_rows.txt`, one per line: file name, line number, the row as read, and the reason.
For duplicates, the **first** row wins and later ones are rejected.
The program must never crash, hang, or print a plan that breaks a rule, whatever the input.

## 5. Seating rules

1. A student sits in exactly one seat. A seat holds at most one student.
2. No two students of the **same course** sit in seats directly left, right, in front or behind each other. Diagonal is allowed.
3. Broken seats are never used.
4. Before placing anyone, a **feasibility check** runs. In a rows x cols room one course can fill at most `ceil(rows * cols / 2)` seats (chessboard bound). If a course is too big for the rooms, or there are more students than seats, the program stops with an explanation such as `Room B is 12 seats short`. It does not search forever.
5. Placement is greedy with a **capped** number of backtracking retries. If the cap is reached the program reports failure instead of running on.
6. The same input and the same strategy always give the same plan (no unseeded randomness).

## 6. Invigilator rules

1. Every room that has students gets exactly one invigilator.
2. An invigilator supervises at most one room per slot.
3. The invigilator must be available for the exam slot (section 3).
4. Among the available people, pick the one with the **fewest duties so far**. Ties go to the smaller id.
5. If no one is available for a room, the room is reported as unstaffed. The plan is still produced.

## 7. Outputs

- A seating chart per room (text), a seat lookup by roll number, and an invigilator duty roster (CSV or text).
- Every plan is saved as a new numbered version. Old versions are never overwritten.
- Seats are shown **1-based** in all output.

## 8. Done means

A feature is done when its unit tests pass and every folder in `data/generated/edge` that touches it behaves as its `EXPECTED.txt` says.
Run `python tools/generate_data.py --edge` to create those folders.

## 9. Who owns what

| Area | Owner | Main files |
|---|---|---|
| Design, storage contract, integration, review | Noman Ahmad | `Invigilator`, `PlanRepository`, `ExamSession`, UML |
| Placement engine, feasibility check, conflict checker | Member B | `SeatingStrategy`, engine, checker |
| CSV reading, validation, rejected rows, menu, reports | Member C | reader, `FileManager` |
