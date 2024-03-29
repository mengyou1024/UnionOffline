cmake_minimum_required(VERSION 3.20)

project(UnionOffline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(NUMCPP_NO_USE_BOOST ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Widgets Charts Core LinguistTools)
set(CMAKE_AUTORCC ON)
qt_standard_project_setup()
file(GLOB_RECURSE PROJECT_SOURCES "src/*.h" "src/*.hpp" "src/*.cpp" "src/*.c")
set(
    PROJECT_RES
    resource/morose.qrc
    icon.rc
)

set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

add_subdirectory("components/QXlsx/QXlsx")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_RES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick Qt6::Widgets Qt6::Charts Qt6::Core)
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)

target_compile_options(${PROJECT_NAME} PUBLIC
    "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<C_COMPILER_ID:MSVC>:/W4>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Morose)

target_link_libraries(${PROJECT_NAME} PRIVATE UnionLib misc_utils SingleApplication::SingleApplication)

target_link_libraries(UnionLib PRIVATE Qt6::Core misc_utils)
target_link_libraries(UnionLib PUBLIC NumCpp)
target_link_libraries(misc_utils PUBLIC QXlsx::QXlsx)

file(GLOB_RECURSE PROJECT_QML_SOURCES "resource/qml/*.qml")
qt_add_lupdate(${PROJECT_NAME} TS_FILES "translations/${PROJECT_NAME}_en.ts" SOURCES ${PROJECT_QML_SOURCES} ${PROJECT_SOURCES})
set_source_files_properties(
    "translations/${PROJECT_NAME}_en.ts"
    PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/translations"
)
qt_add_lrelease(${PROJECT_NAME} TS_FILES "translations/${PROJECT_NAME}_en.ts")


if (Morose_FOUND)
    if((NOT EXISTS "${CMAKE_SOURCE_DIR}/config/ProductConfig.json") AND (NOT CMAKE_BUILD_TYPE STREQUAL "Release"))
        message(WARNING "no ProductConfig.json file please see ${CMAKE_SOURCE_DIR}/config/ProductConfigTemplate.json")
    else()
        morose_add_environment_config_file(
            TARGET ${PROJECT_NAME}
            DEPLOY "config/DeployConfig.json"
            PRODUCT "config/ProductConfig.json"
            DIST "Config.json"
        )
    endif()

    morose_copy(TARGET ${PROJECT_NAME} DIRS "excel_templates")

    morose_add_qml_dirs("resource/qml")

    morose_auto_release()
endif(Morose_FOUND)
