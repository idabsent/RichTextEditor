cmake_minimum_required(VERSION 3.16)

project(RichTextEditor
    VERSION 0.1
    DESCRIPTION "Simple rich text editor"
    LANGUAGES CXX
)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5
    REQUIRED COMPONENTS
        Gui Widgets DBus LinguistTools
    OPTIONAL_COMPONENTS
        PrintSupport
)

set(TS_FILES
    translations/RichTextEditor_en_US.ts
)

set(HEADER_FILES
    include/richtexteditor.hpp
    include/menubarbuilder.hpp
    include/actions.hpp
    include/formatactions.hpp
    include/editorobservers.hpp
    include/texteditoractions.hpp
    include/dbussession.hpp
    include/tools.hpp
    include/config.in
)

set(SOURCE_FILES
    src/main.cpp
    src/richtexteditor.cpp
    src/menubarbuilder.cpp
    src/texteditoractions.cpp
    src/formatactions.cpp
    src/dbussession.cpp
    src/editorobservers.cpp
)

qt5_add_translation(QM_FILES ${TS_FILES})
qt5_add_resources(RESOURCES res/resources.qrc)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${TS_FILES}
        ${RESOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        include/
        ${CMAKE_BINARY_DIR}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
)


target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt5::Gui Qt5::Widgets Qt5::DBus
)

configure_file(${CMAKE_SOURCE_DIR}/include/config.in ${CMAKE_BINARY_DIR}/config.h)
