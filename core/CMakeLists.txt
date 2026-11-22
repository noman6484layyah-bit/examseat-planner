# All logic lives in this library. It never reads the keyboard or prints menus,
# so a console app (now) and a Qt GUI (later) can both use it.
add_library(examseat_core STATIC
  src/StringUtil.cpp
  src/TimeSlot.cpp
  src/Person.cpp
  src/Student.cpp
  src/Invigilator.cpp
  src/Room.cpp
  src/ExamSession.cpp
  src/SeatingPlan.cpp
  src/SeatingStrategy.cpp
  src/PlanRepository.cpp
)
target_include_directories(examseat_core PUBLIC include)
