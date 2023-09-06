cmake_minimum_required(VERSION 3.8)

macro(GroupSources curdir)
  file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

  foreach(child ${children})
    if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
      GroupSources(${curdir}/${child})
    else()
      string(REPLACE "/" "\\" groupname ${curdir})
      source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
    endif()
  endforeach()
endmacro()

project(QtJsonEditor)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE QtJsonEditor_Widget_SRCS "src/widgets/*.cpp" "src/widgets/*.h")
file(GLOB_RECURSE QtJsonEditor_Ui_SRCS "src/ui/*.cpp" "src/ui/*.h")
file(GLOB_RECURSE QtJsonEditor_Ts "src/ts/*.ts") # Translations

GroupSources(src)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)
#find_package(Qt5 COMPONENTS Widgets Core Gui LinguistTools REQUIRED)

add_library(qt_json_widget ${QtJsonEditor_Widget_SRCS})
target_link_libraries(qt_json_widget PUBLIC Qt5::Widgets Qt5::Core Qt5::Gui)
target_include_directories(qt_json_widget PUBLIC "src" "submodules/valijson/include")

add_executable(qt_json_editor ${QtJsonEditor_Ui_SRCS} src/main.cpp)
target_link_libraries(qt_json_editor PUBLIC qt_json_widget)

#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${QtJsonEditor_Ts})
#qt5_add_translation(qm_files ${QtJsonEditor_Ts})
