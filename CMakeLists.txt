cmake_minimum_required(VERSION 3.15)

project(TaskApiService)

set(CMAKE_CXX_STANDARD 17)

find_package(cpprestsdk CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(TaskApiService
    main.cpp
    controllers/task_controller.cpp
    services/task_service.cpp
    models/task.cpp
    db/db.cpp
)

target_include_directories(TaskApiService PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/controllers
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/services
    ${CMAKE_CURRENT_SOURCE_DIR}/db

    # This adds sqlite-modern-cpp headers from vcpkg
    C:/Users/conta/Vivek/ProjectsC++/vcpkg/installed/x64-windows/include
)

target_link_libraries(TaskApiService PRIVATE 
    cpprestsdk::cpprest
    unofficial::sqlite3::sqlite3
)
    