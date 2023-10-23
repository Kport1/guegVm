cmake_minimum_required(VERSION 3.26)

project(GuegVm)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED 23)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lm -g -O0")

add_executable(gvm
    main.c
    file/file_reader.c
    error/error.c
    vm/vm.c

)