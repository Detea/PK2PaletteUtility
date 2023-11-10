cmake_minimum_required(VERSION 3.16)

project(PaletteUtility VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

include_directories(${PROJECT_SOURCE_DIR}/src/colorchooser)
include_directories(${PROJECT_SOURCE_DIR}/src/ui)
include_directories(${PROJECT_SOURCE_DIR}/src/undo)
include_directories(${PROJECT_SOURCE_DIR}/src/utils)

qt_add_executable(PaletteUtility
    ${PROJECT_SOURCE_DIR}/src/colorchooser/ColorChooser.cpp
    ${PROJECT_SOURCE_DIR}/src/colorchooser/ColorComponent.h
    ${PROJECT_SOURCE_DIR}/src/colorchooser/ColorSelectionBar.cpp

    ${PROJECT_SOURCE_DIR}/src/ui/ImageView.cpp
    ${PROJECT_SOURCE_DIR}/src/ui/MainMenuBar.cpp
    ${PROJECT_SOURCE_DIR}/src/ui/PaletteRenderer.cpp
    ${PROJECT_SOURCE_DIR}/src/ui/PaletteViewWidget.cpp
    ${PROJECT_SOURCE_DIR}/src/ui/PK2PaletteUtility.cpp
    
    ${PROJECT_SOURCE_DIR}/src/undo/ColorsChangeCommand.cpp
    ${PROJECT_SOURCE_DIR}/src/undo/UndoAction.h

    ${PROJECT_SOURCE_DIR}/src/utils/PaletteFileParser.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/PaletteUtils.h

    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(PaletteUtility PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

set_target_properties(PaletteUtility PROPERTIES WIN32_EXECUTABLE ON)