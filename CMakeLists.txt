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
    src/colorchooser/ColorChooser.cpp
    src/colorchooser/ColorComponent.h
    src/colorchooser/ColorSelectionBar.cpp

    src/ui/ImageView.cpp
    src/ui/MainMenuBar.cpp
    src/ui/PaletteRenderer.cpp
    src/ui/PaletteViewWidget.cpp
    src/ui/PK2PaletteUtility.cpp
    
    src/undo/ColorsChangeCommand.cpp
    src/undo/UndoAction.h

    src/utils/PaletteFileParser.cpp
    src/utils/PaletteUtils.h

    res/resources.qrc
    res/resources.rc

    src/main.cpp
)

set_target_properties(PaletteUtility PROPERTIES AUTORCC ON)
set_target_properties(PaletteUtility PROPERTIES WIN32_EXECUTABLE ON)

target_link_libraries(PaletteUtility PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets)