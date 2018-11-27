cmake_minimum_required(VERSION 3.12)
project(TECMFS_Controller)

set(CMAKE_CXX_STANDARD 11)
add_executable(TECMFS_Controller main.cpp Server/Singleton.cpp Server/Singleton.h DataStructures/LinkedList.h DataStructures/Node.h Controller/DiskManager.cpp Controller/DiskManager.h Controller/DataBase.cpp Controller/DataBase.h Controller/FaultTolerance.cpp Controller/FaultTolerance.h Controller/MediaManager.cpp Controller/MediaManager.h)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package (SFML REQUIRED system window graphics network audio)

target_link_libraries(TECMFS_Controller ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})