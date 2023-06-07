cmake_minimum_required(VERSION 3.5)

project(chat VERSION 0.1 LANGUAGES CXX)

set(CMAK
    INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets   )
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
include/addfrienddialog.hpp
include/userlist.hpp
 d:/QtProject/chat/include/login.h
d:/QtProject/chat/include/mainwindow.h
d:/QtProject/chat/include/net
d:/QtProject/chat/include/thread
d:/QtProject/chat/include/net/netutility.h
d:/QtProject/chat/include/net/regulation.h
d:/QtProject/chat/include/net/request.h
d:/QtProject/chat/include/net/requesthandler.h
d:/QtProject/chat/include/net/response.h
d:/QtProject/chat/include/thread/blockingqueue.h
d:/QtProject/chat/source/login.cpp
d:/QtProject/chat/source/main.cpp
d:/QtProject/chat/source/mainwindow.cpp
d:/QtProject/chat/source/net
d:/QtProject/chat/source/net/netutility.cpp
d:/QtProject/chat/source/net/requesthandler.cpp
d:/QtProject/chat/source/net/response.cpp
include/message.h
include/messagelist.hpp
)

include_directories(
    include  # 指定头文件目录的路径，可以是相对路径或绝对路径
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(chat
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET chat APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(chat SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(chat
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(chat PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
find_package(Qt6 COMPONENTS Network REQUIRED)
target_link_libraries(chat PRIVATE Qt6::Network)

set_target_properties(chat PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(chat)
endif()
