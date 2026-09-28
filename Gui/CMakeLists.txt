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
    # style.css
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
    #style.css
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

set(TESSDATA_SOURCE
    ${CMAKE_SOURCE_DIR}/tessdata/eng.traineddata
)

add_custom_command(TARGET SudoScan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:SudoScan>/tessdata"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TESSDATA_SOURCE}"
            "$<TARGET_FILE_DIR:SudoScan>/tessdata/eng.traineddata"
)

set(TESSDATA_SOURCE
    ${CMAKE_SOURCE_DIR}/tessdata/eng.traineddata
)

add_custom_command(TARGET SudoScan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:SudoScan>/tessdata"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TESSDATA_SOURCE}"
            "$<TARGET_FILE_DIR:SudoScan>/tessdata/eng.traineddata"
)

include(GNUInstallDirs)

install(TARGETS SudoScan
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    FILES
        ${CMAKE_SOURCE_DIR}/tessdata/eng.traineddata
    DESTINATION
        ${CMAKE_INSTALL_BINDIR}/tessdata
)