cmake_minimum_required(VERSION 3.0)
project(stm32f C)


####
set(KERNEL_INCLUDE ../../include)
include_directories(${KERNEL_INCLUDE} ${OPENLIBCM3_INCLUDE} ./ ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DSTM32F4 )

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -MD -Wall -Wundef -I${KERNEL_INCLUDE}")
set(MAKE_C_FLAGS_DEBUG "-g -ggdb")

set( CMAKE_EXE_LINKER_FLAGS "--static -nostartfiles")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${OPENLIBCM3_LIB}")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${PROJECT_SOURCE_DIR}/stm32f429i-discovery.ld")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=myapp.map")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-gc-sections")
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group")

add_custom_command(
	OUTPUT myapp_init.c 
	MAIN_DEPENDENCY myapp.oil 
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND ${OIL} -I${KERNEL_INCLUDE} -a stm234f myapp.oil -o ${CMAKE_CURRENT_BINARY_DIR}/myapp
) 

add_executable( myapp.elf myapp.c myapp_init.c)
target_link_libraries( myapp.elf kernel stm32f disco libopencm3_stm32f4.a )

## flash the 
add_custom_command(
	TARGET myapp.elf POST_BUILD
	MAIN_DEPENDENCY myapp.elf
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
       	COMMAND arm-none-eabi-size ${EXECUTABLE_OUTPUT_PATH}/myapp.elf	
	COMMAND ${OOCD} ${OOCD_ARGS}
) 
