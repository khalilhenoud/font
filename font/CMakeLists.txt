cmake_minimum_required(VERSION 3.22)

project(font VERSION 1.0)

add_library(${PROJECT_NAME} SHARED
        ./source/font_asset.c
        ./include/font/internal/module.h
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC library)

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        )