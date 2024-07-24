cmake_minimum_required(VERSION 3.10)
project(CarControlAutomation)

set(CMAKE_CXX_STANDARD 20)


set(SOURCES
    main.cpp
)

add_executable(CarControlAutomation ${SOURCES})


if(WIN32)
    target_link_libraries(CarControlAutomation PRIVATE 
        user32 
        gdi32 
        kernel32
    )
endif()
