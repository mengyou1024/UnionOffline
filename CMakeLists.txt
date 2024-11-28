cmake_minimum_required(VERSION 3.20)

project(UnionOffline LANGUAGES CXX)

set(PROJECT_NAME_ZH_CN "390N离线分析软件")

set(CMAKE_CXX_STANDARD 23)
set(NUMCPP_NO_USE_BOOST ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

find_package(Qt5 5.15 REQUIRED COMPONENTS Quick Widgets Charts Core SerialPort Network)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_program(WINDEPLOYQT_EXECUTABLE windeployqt.exe REQUIRED)
find_program(LUPDATE_EXECUTABLE lupdate.exe REQUIRED)
find_program(LRELEASE_EXECUTABLE lrelease.exe REQUIRED)

message(STATUS "WINDEPLOYQT_EXECUTABLE:${WINDEPLOYQT_EXECUTABLE}")
message(STATUS "LUPDATE_EXECUTABLE:${LUPDATE_EXECUTABLE}")
message(STATUS "LRELEASE_EXECUTABLE:${LRELEASE_EXECUTABLE}")

set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

add_subdirectory("components/QXlsx/QXlsx")

file(GLOB_RECURSE LUPDATE_SOURCES
    "src/*.h"
    "src/*.c"
    "src/*.hpp"
    "src/*.cpp"
    "src/*.cxx"
    "src/*.cc"
    "resource/*.qml"
    "components/union/include_private/*.h"
    "components/union/include_private/*.hpp"
    "components/union/src/*.h"
    "components/union/src/*.c"
    "components/union/src/*.hpp"
    "components/union/src/*.cpp"
    "components/union/src/*.cc"
    "components/union/src/*.cxx"
)

add_custom_target(
    lupdate
    COMMAND ${LUPDATE_EXECUTABLE} ${LUPDATE_SOURCES} -ts translations/UnionOffline_en.ts
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    lrelease
    COMMAND ${LRELEASE_EXECUTABLE} translations/UnionOffline_en.ts
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB_RECURSE ${PROJECT_NAME}_SRC "src/*.cpp" "src/*.c" "src/*.cc" "src/*.cxx")
file(GLOB_RECURSE ${PROJECT_NAME}_INC "src/*.h" "src/*.hpp")

set(${PROJECT_NAME}_INCLUDE_DIRS "")

foreach(HEAD_FILE ${${PROJECT_NAME}_INC})
    get_filename_component(directory_name ${HEAD_FILE} DIRECTORY)
    list(FIND ${PROJECT_NAME}_INCLUDE_DIRS ${directory_name} index)

    if(${index} EQUAL -1)
        list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${directory_name})
    endif()
endforeach(HEAD_FILE ${PROJECT_NAME})


add_executable(
    ${PROJECT_NAME}
    ${${PROJECT_NAME}_SRC}
    ${${PROJECT_NAME}_INC}
    resource/morose.qrc
    resource/icon.rc
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${${PROJECT_NAME}_INCLUDE_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    Qt5::Quick
    Qt5::Widgets
    Qt5::Charts
    Qt5::Core
    Qt5::SerialPort
    Qt5::Network
)
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)

target_compile_options(${PROJECT_NAME} PUBLIC
    "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<C_COMPILER_ID:MSVC>:/W4>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Morose)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    UnionLib
    misc_utils
    SingleApplication::SingleApplication
)

target_link_libraries(
    UnionLib
    PUBLIC
    Qt5::Quick
    Qt5::Core
    Qt5::SerialPort
    Qt5::Widgets
    misc_utils
    protobuf::libprotobuf
)
target_link_libraries(misc_utils PUBLIC QXlsx::QXlsx)

if(Morose_FOUND)
    set(PRODUCT_CONFIG_FILE "config/ProductConfig.json")

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/config/ProductConfig.json")
        message(WARNING "no ProductConfig.json file please see ${CMAKE_SOURCE_DIR}/config/ProductConfigTemplate.json")
        set(PRODUCT_CONFIG_FILE "config/DeployConfig.json")
    endif()

    morose_add_environment_config_file(
        TARGET ${PROJECT_NAME}
        DEPLOY "config/DeployConfig.json"
        PRODUCT ${PRODUCT_CONFIG_FILE}
        DIST "Config.json"
    )

    morose_copy(TARGET ${PROJECT_NAME} DIRS "excel_templates")
    morose_copy(TARGET ${PROJECT_NAME} FILES "components/ssl/libssl-1_1-x64.dll")
    morose_copy(TARGET ${PROJECT_NAME} FILES "components/ssl/libcrypto-1_1-x64.dll")
    morose_add_qml_dirs("resource/qml")
    morose_auto_release()
endif(Morose_FOUND)
