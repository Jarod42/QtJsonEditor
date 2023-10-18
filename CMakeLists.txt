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

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets LinguistTools REQUIRED)

add_library(qt_json_widget ${QtJsonEditor_Widget_SRCS})
target_link_libraries(qt_json_widget
	PUBLIC
		Qt${QT_VERSION_MAJOR}::Widgets
		Qt${QT_VERSION_MAJOR}::Core
		Qt${QT_VERSION_MAJOR}::Gui
)
target_include_directories(qt_json_widget PUBLIC "src" "submodules/valijson/include")

add_executable(qt_json_editor ${QtJsonEditor_Ui_SRCS} src/main.cpp)
target_link_libraries(qt_json_editor PUBLIC qt_json_widget)

#if (QT_VERSION VERSION_LESS 5.15.0)
#    qt5_add_translation(QM_FILES ${QtJsonEditor_Ts})
#else()
#    qt_add_translation(QM_FILES ${QtJsonEditor_Ts})
#endif()

#set(Qt_LCONVERT_EXECUTABLE Qt${QT_VERSION_MAJOR}::lconvert)
#add_custom_command(OUTPUT ${_out_qm}
#                COMMAND ${Qt_LCONVERT_EXECUTABLE}
#                ARGS -i ${_infiles}_ts -o ${_out_qm}
#                COMMAND_EXPAND_LISTS VERBATIM)
#				
#add_custom_target(lupdate
#    COMMAND ${Qt${QT_VERSION_MAJOR}_LUPDATE_EXECUTABLE} -recursive ${PROJECT_SOURCE_DIR} -ts *.ts
#    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#    COMMENT "Updating translations"
#)
