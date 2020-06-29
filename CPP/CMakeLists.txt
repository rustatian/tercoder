cmake_minimum_required(VERSION 3.14)
set (CMAKE_CXX_STANDARD 20)
project(recorder)

add_executable(recorder src/recorder.cpp include/pty_fork.hpp src/pty_fork.cpp include/tty_set_raw.hpp src/tty_set_raw.cpp include/pty_master.hpp src/pty_master.cpp)