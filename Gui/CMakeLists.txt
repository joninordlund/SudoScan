find_package(Qt6 REQUIRED COMPONENTS PrintSupport Concurrent Network)

add_executable(SudoScan
    main.cpp
    cell.cpp include/cell.h
    grid.cpp include/grid.h
    include/mainwindow.h mainwindow.cpp
    include/toggleswitch.h toggleswitch.cpp
    include/sudokuboard.h sudokuboard.cpp
    include/sudokuimage.h sudokuimage.cpp
    include/history.h history.cpp
    include/cellchange.h
    resources.qrc
    include/sudokuserializer.h sudokuserializer.cpp
    include/countlabel.h countlabel.cpp
    style.css
    include/qrmanager.h qrmanager.cpp
    include/qrcodegen.hpp qrcodegen.cpp
    help.html
    helpdialog.cpp include/helpdialog.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    main.cpp
    cell.cpp include/cell.h
    grid.cpp include/grid.h
    include/mainwindow.h mainwindow.cpp
    include/toggleswitch.h toggleswitch.cpp
    include/sudokuboard.h sudokuboard.cpp
    include/sudokuimage.h sudokuimage.cpp
    include/history.h history.cpp
    include/cellchange.h
    resources.qrc
    include/sudokuserializer.h sudokuserializer.cpp
    include/countlabel.h countlabel.cpp
    style.css
    include/qrmanager.h qrmanager.cpp
    include/qrcodegen.hpp qrcodegen.cpp
    help.html
    helpdialog.cpp include/helpdialog.h
)

target_include_directories(SudoScan
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SudoScan
    PRIVATE
        Qt6::Widgets
        Qt6::Network
        Qt6::PrintSupport
        Qt6::Concurrent
        solver
        reader
)
