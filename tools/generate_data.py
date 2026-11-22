#!/usr/bin/env python3
"""Generate fake, reproducible test data for ExamSeat.

Same --seed gives the same files, so any bug can be reproduced.

  python tools/generate_data.py                      # normal datasets -> data/generated/
  python tools/generate_data.py --edge               # also write the edge-case folders
  python tools/generate_data.py --students 500 --rooms 12 --courses 6 --seed 7 --out data/my_run

No real student data is used or needed.
"""
import argparse
import csv
import os
import random

COURSES = ["CS", "MA", "PH", "EE", "ME", "CE", "BA", "SE", "CH", "EN"]
FIRST = ["Ali", "Ayesha", "Bilal", "Fatima", "Hamza", "Hira", "Imran", "Maryam", "Omar",
         "Sana", "Usman", "Zainab", "Taha", "Noor", "Hassan", "Iqra", "Rayyan", "Laiba"]
LAST = ["Khan", "Ahmed", "Malik", "Sheikh", "Butt", "Raza", "Qureshi", "Siddiqui", "Mirza",
        "Chaudhry", "Hussain", "Javed", "Aslam", "Farooq", "Nawaz", "Baig"]
EXAM_DAY = "2026-12-10"
WINDOW = f"{EXAM_DAY} 08:00-13:00"   # contains the exam slot 09:00-12:00


def write(path, header, rows):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(header)
        w.writerows(rows)


def make_set(out, n_students, n_rooms, n_courses, seed, n_invigilators=None):
    rng = random.Random(seed)
    courses = COURSES[:max(1, min(n_courses, len(COURSES)))]
    students = []
    for i in range(1, n_students + 1):
        name = f"{rng.choice(FIRST)} {rng.choice(LAST)}"
        students.append((f"S{i:05d}", name, rng.choice(courses)))
    rooms = []
    for i in range(1, n_rooms + 1):
        rows, cols = rng.choice([(5, 6), (6, 6), (6, 8), (8, 8), (7, 7)])
        rooms.append((f"Room-{i:02d}", rows, cols, rng.choice(["LectureHall", "ComputerLab"])))
    n_inv = n_invigilators if n_invigilators is not None else max(1, n_rooms + n_rooms // 4)
    inv = [(f"I{i:03d}", f"Dr. {rng.choice(FIRST)} {rng.choice(LAST)}", WINDOW) for i in range(1, n_inv + 1)]
    write(os.path.join(out, "students.csv"), ["roll_no", "name", "course"], students)
    write(os.path.join(out, "rooms.csv"), ["room", "rows", "cols", "type"], rooms)
    write(os.path.join(out, "invigilators.csv"), ["id", "name", "slots"], inv)


def raw(path, text):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", newline="", encoding="utf-8") as f:   # newline="" keeps \r\n as written
        f.write(text)


S_HEAD = "roll_no,name,course\n"
R_OK = "room,rows,cols,type\nRoom-01,5,6,LectureHall\n"
I_OK = "id,name,slots\nI001,Dr. Khan,2026-12-10 08:00-13:00\n"


def edge_cases(base):
    cases = {
        "01_empty_students_file": ("", R_OK, I_OK, "Clear error: students file is empty. No crash."),
        "02_header_only": (S_HEAD, R_OK, I_OK, "Zero students loaded. Report 'nothing to seat', no crash."),
        "03_duplicate_roll_numbers": (S_HEAD + "S1,Ali Khan,CS\nS1,Bilal Ahmed,MA\nS2,Sana Malik,CS\n", R_OK, I_OK,
                                      "Second S1 row goes to rejected_rows with reason 'duplicate roll number'."),
        "04_missing_fields": (S_HEAD + "S1,Ali Khan,\nS2,,CS\n,Sana Malik,MA\nS3,Hira Raza,PH\n", R_OK, I_OK,
                              "Rows 1-3 rejected with reasons. S3 loads."),
        "05_spaces_and_case": (S_HEAD + "  s1 , Ali Khan , cs \nS2,Bilal Ahmed,CS\n", R_OK, I_OK,
                               "Trim whitespace. Decide and document the case rule (cs and CS are the same course)."),
        "06_crlf_line_endings": ("roll_no,name,course\r\nS1,Ali Khan,CS\r\nS2,Bilal Ahmed,MA\r\n", R_OK, I_OK,
                                 "Both students load. No stray \\r in the course name."),
        "07_utf8_bom": ("﻿roll_no,name,course\nS1,Ali Khan,CS\n", R_OK, I_OK,
                        "Header is read correctly despite the BOM."),
        "08_quoted_comma_in_name": (S_HEAD + 'S1,"Khan, Ali",CS\nS2,"O\'Brien ""Bob""",MA\n', R_OK, I_OK,
                                    "Names keep their commas and quotes. Fields are not split wrongly."),
        "09_bad_room_sizes": (S_HEAD + "S1,Ali Khan,CS\n", "room,rows,cols,type\nA,0,5,LectureHall\nB,-2,4,LectureHall\nC,x,y,Lab\nD,1,1,ComputerLab\n", I_OK,
                              "A, B, C rejected with reasons. D (1x1) is valid."),
        "10_single_student": (S_HEAD + "S1,Ali Khan,CS\n", R_OK, I_OK, "One student seated, one invigilator assigned, no crash."),
        "11_one_huge_course": (S_HEAD + "".join(f"S{i},Student {i},CS\n" for i in range(1, 21)),
                               "room,rows,cols,type\nA,2,3,LectureHall\nB,2,3,LectureHall\nC,2,3,LectureHall\n", I_OK,
                               "20 CS students, 3 rooms of 2x3. One course fits at most 3 per room = 9. Must report 'impossible', not hang."),
        "12_exact_capacity": (S_HEAD + "".join(f"S{i},Student {i},{'CS' if i % 2 else 'MA'}\n" for i in range(1, 7)),
                              "room,rows,cols,type\nA,2,3,LectureHall\n", I_OK,
                              "6 students, 6 seats, alternating courses. Valid plan, every seat used."),
        "13_one_over_capacity": (S_HEAD + "".join(f"S{i},Student {i},{'CS' if i % 2 else 'MA'}\n" for i in range(1, 8)),
                                 "room,rows,cols,type\nA,2,3,LectureHall\n", I_OK,
                                 "7 students, 6 seats. Report '1 seat short', no crash."),
        "14_fewer_invigilators_than_rooms": (S_HEAD + "".join(f"S{i},Student {i},{'CS' if i % 2 else 'MA'}\n" for i in range(1, 13)),
                                             "room,rows,cols,type\nA,2,3,LectureHall\nB,2,3,LectureHall\n", I_OK,
                                             "2 rooms, 1 invigilator in the same slot. Report the unstaffed room."),
        "15_invalid_slot_format": (S_HEAD + "S1,Ali Khan,CS\n", R_OK,
                                   "id,name,slots\nI001,Dr. Khan,10am-12pm\nI002,Dr. Raza,2026-12-10 08:00-13:00\nI003,Dr. Ali,2026-02-30 08:00-13:00\n",
                                   "I001 and I003 rejected with the TimeSlot error text as the reason. I002 loads."),
        "16_invigilator_not_available": (S_HEAD + "S1,Ali Khan,CS\n", R_OK,
                                         "id,name,slots\nI001,Dr. Khan,2026-12-11 08:00-13:00\nI002,Dr. Raza,2026-12-10 10:00-13:00\n",
                                         "Exam slot is 2026-12-10 09:00-12:00. Neither invigilator covers it fully. Report the unstaffed room, no crash."),
    }
    for name, (s, r, i, expected) in cases.items():
        d = os.path.join(base, name)
        raw(os.path.join(d, "students.csv"), s)
        raw(os.path.join(d, "rooms.csv"), r)
        raw(os.path.join(d, "invigilators.csv"), i)
        raw(os.path.join(d, "EXPECTED.txt"), expected + "\n")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--students", type=int)
    ap.add_argument("--rooms", type=int)
    ap.add_argument("--courses", type=int, default=8)
    ap.add_argument("--seed", type=int, default=42)
    ap.add_argument("--out", default=None)
    ap.add_argument("--edge", action="store_true", help="also write the edge-case folders")
    a = ap.parse_args()

    if a.students and a.rooms:
        make_set(a.out or "data/generated/custom", a.students, a.rooms, a.courses, a.seed)
        print("wrote", a.out or "data/generated/custom")
        return
    base = a.out or "data/generated"
    make_set(os.path.join(base, "small"), 20, 2, 3, a.seed, n_invigilators=3)
    make_set(os.path.join(base, "medium"), 300, 8, 6, a.seed)
    make_set(os.path.join(base, "campus"), 1200, 35, 8, a.seed)
    make_set(os.path.join(base, "stress"), 5000, 120, 10, a.seed)
    print("wrote normal datasets to", base)
    if a.edge:
        edge_cases(os.path.join(base, "edge"))
        print("wrote edge cases to", os.path.join(base, "edge"))


if __name__ == "__main__":
    main()
