project(Matrix_lib)

set(HEADER_FILES
        hw1.h
        )

set(SOURCE_FILES
        hw1.cpp
        )

add_library(Matrix_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})