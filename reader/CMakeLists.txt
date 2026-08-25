find_package(OpenCV REQUIRED)

add_library(reader
    imageprocessor.cpp include/imageprocessor.h
    sudokureader.cpp include/sudokureader.h
)

target_include_directories(reader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(reader
    PUBLIC
        ${OpenCV_LIBS}
        /usr/lib/x86_64-linux-gnu/libtesseract.so
        # /usr/include/tesseract   # Tesseractin include-polku lisätään tällä tavalla, koska sitä ei välttämättä ole määritelty Target:iksi
)
# target_link_libraries(reader PUBLIC ${OpenCV_LIBS})
target_include_directories(reader PUBLIC /usr/include/tesseract ${OpenCV_INCLUDE_DIRS})
