cmake_minimum_required(VERSION 3.0)

####

add_definitions( -DSTM32F4 )

set(CMAKE_C_FLAGS "-g -ggdb -Os -mcpu=cortex-m4 -mthumb -Wextra -Wshadow -Wimplicit-function-declaration")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fno-common -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -MD -Wall -Wundef -I${KERNEL_INCLUDE}")
set(MAKE_C_FLAGS_DEBUG "-g -ggdb")

SET(sources machdep.c )

add_library(stm32f ${sources})
target_include_directories(stm32f PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
