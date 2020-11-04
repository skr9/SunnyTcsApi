cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(SunnyTcsApi)

set(CMAKE_CXX_STANDARD 11)

if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "C:/Qt/Qt5.14.2/5.14.2/msvc2017_64")
    include_directories(C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/include)
    include_directories(C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/include/QtCore)
    include_directories(C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/include/QtNetwork)
    include_directories(../third_party/ZeroMQ4.0.4/include)
    set(LINK_DIR C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib)
else()
    list(APPEND CMAKE_PREFIX_PATH "/opt/Qt5.14.0/5.14.0/gcc_64")
    include_directories(/opt/Qt5.14.0/5.14.0/gcc_64/include)
    include_directories(/opt/Qt5.14.0/5.14.0/gcc_64/include/QtCore)
    include_directories(/opt/Qt5.14.0/5.14.0/gcc_64/include/QtNetwork)
    set(LINK_DIR /opt/Qt5.14.0/5.14.0/gcc_64/lib)
endif()

find_package(Qt5 COMPONENTS Core REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

aux_source_directory(./src  SRC)
aux_source_directory(./src/controller CONTROLLER)
aux_source_directory(./third_party/QtWebApp/QtWebApp/httpserver HTTPSERVER_SRC)
aux_source_directory(./third_party/QtWebApp/QtWebApp/logging LOG_SRC)
aux_source_directory(./third_party/QtWebApp/QtWebApp/templateengine TEMPLATE_SRC)

link_directories(${LINK_DIR})

add_compile_options(-fPIC)
add_compile_options(-g)

add_executable(${PROJECT_NAME}  ${SRC} ${CONTROLLER} ${HTTPSERVER_SRC} ${LOG_SRC} ${TEMPLATE_SRC})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} Qt5Core.lib Qt5Network.lib)
else()
    target_link_libraries(${PROJECT_NAME} libQt5Core.so libQt5Network.so)
endif()