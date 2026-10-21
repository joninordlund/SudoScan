find_package(OpenCV REQUIRED)

if(WIN32)
    find_package(Tesseract CONFIG REQUIRED
        PATHS "C:/tessbuild/tesseract-install/lib/cmake/tesseract"
        NO_DEFAULT_PATH
    )
else()
    find_library(TESSERACT_LIBRARY
        NAMES tesseract
        PATHS "$ENV{HOME}/tesseract/build-shared"
        NO_DEFAULT_PATH
        REQUIRED
    )
endif()

add_library(reader
    imageprocessor.cpp include/imageprocessor.h
    sudokureader.cpp include/sudokureader.h
)

target_include_directories(reader
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
)

if(WIN32)
    target_include_directories(reader
        PUBLIC
            C:/tessbuild/tesseract-install/include
    )

    target_link_libraries(reader
        PUBLIC
            ${OpenCV_LIBS}
            Tesseract::libtesseract
    )
else()
    target_include_directories(reader
        PUBLIC
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
endif()