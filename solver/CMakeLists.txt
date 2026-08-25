add_library(solver
    DLXBuilder.cpp include/DLXBuilder.h
    sudokusolver.cpp include/sudokusolver.h
    include/solutionset.h solutionset.cpp
)

target_include_directories(solver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
