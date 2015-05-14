cmake_minimum_required(VERSION 3.0)

SET(sources alarm.c  event.c  execution.c  idle.c  proc.c  resource.c)

add_library(kernel ${sources})
target_include_directories(kernel PUBLIC ../include)
