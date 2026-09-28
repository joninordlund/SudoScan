# find_package(OpenCV REQUIRED)

# add_library(reader
#     imageprocessor.cpp include/imageprocessor.h
#     sudokureader.cpp include/sudokureader.h
# )

# target_include_directories(reader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# target_link_libraries(reader
#     PUBLIC
#         ${OpenCV_LIBS}
#         /usr/lib/x86_64-linux-gnu/libtesseract.so
       
# # target_link_libraries(reader PUBLIC ${OpenCV_LIBS})
# target_include_directories(reader PUBLIC /usr/include/tesseract ${OpenCV_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)

find_library(TESSERACT_LIBRARY
    NAMES tesseract
    PATHS "$ENV{HOME}/tesseract/build-shared"
    NO_DEFAULT_PATH
    REQUIRED
)

add_library(reader
    imageprocessor.cpp include/imageprocessor.h
    sudokureader.cpp include/sudokureader.h
)

target_include_directories(reader
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
        $ENV{HOME}/tesseract/include
)

target_link_directories(reader
    PUBLIC
        $ENV{HOME}/tesseract/build-shared
)

target_link_libraries(reader
    PUBLIC
        ${OpenCV_LIBS}
        ${TESSERACT_LIBRARY}
)