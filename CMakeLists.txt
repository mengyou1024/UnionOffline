cmake_minimum_required(VERSION 3.20)

project(UnionOffline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(NUMCPP_NO_USE_BOOST ON)

find_package(Qt5 5.15 REQUIRED COMPONENTS Quick Widgets Charts Core SerialPort Network)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt.exe)
message(STATUS "WINDEPLOYQT_EXECUTABLE:${WINDEPLOYQT_EXECUTABLE}")

file(GLOB_RECURSE PROJECT_SOURCES "src/*.h" "src/*.hpp" "src/*.cpp" "src/*.c")
set(
    PROJECT_RES
    resource/morose.qrc
    resource/icon.rc
)

set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

add_subdirectory("components/QXlsx/QXlsx")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_RES})
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
