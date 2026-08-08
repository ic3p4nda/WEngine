cmake_minimum_required(VERSION 3.20)
project(shaders NONE)

find_program(GLSLC glslc HINTS "$ENV{VULKAN_SDK}/Bin")

file(GLOB SHADERS "*.vert" "*.frag")

foreach(SHADER ${SHADERS})
    get_filename_component(NAME ${SHADER} NAME)
    add_custom_target(${NAME}_spv ALL
            COMMAND ${GLSLC} ${SHADER} -o ${SHADER}.spv
    )
endforeach()