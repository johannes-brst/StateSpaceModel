find_package(ur_rtde REQUIRED)
add_library(moveur5e MoveUr5e.cpp MoveUr5e.h)
target_link_libraries(moveur5e PRIVATE ur_rtde::rtde)