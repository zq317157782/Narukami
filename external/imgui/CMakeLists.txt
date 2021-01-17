cmake_minimum_required(VERSION 3.0)
project(imgui)

include_directories(.)
include_directories(backends)
if(IMGUI_BACKEND_GLFW_OPENGL3)
include_directories(GL)
set(IMGUI_BACKEND backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp GL/gl3w.c)
endif()

add_library(imgui STATIC imgui.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp ${IMGUI_BACKEND})
