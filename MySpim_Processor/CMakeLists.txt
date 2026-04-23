cmake_minimum_required(VERSION 4.1)
project(MySpim_Processor)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(MySpim_Processor
        .idea/.gitignore
        .idea/editor.xml
        .idea/modules.xml
        .idea/MySpim_Processor.iml
        .idea/vcs.xml
        .idea/workspace.xml
        test_files/input_file.asc
        test_files/input_file2.asc
        test_files/test_results.txt
        project.c
        spimcore.c
        spimcore.h)
