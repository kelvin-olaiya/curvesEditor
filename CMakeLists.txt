cmake_minimum_required(VERSION 3.16)
project(CurvesEditor C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Dependencies (downloaded at configure time, nothing to install)
# ---------------------------------------------------------------------------
include(FetchContent)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
    URL https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz)
FetchContent_Declare(glm
    URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.tar.gz)
FetchContent_Declare(imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.8.tar.gz)
FetchContent_MakeAvailable(glfw glm imgui)

# OpenGL 3.3 core loader, generated with glad 0.1.36
add_subdirectory(third_party/glad)

find_package(OpenGL REQUIRED)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# ---------------------------------------------------------------------------
# Application
# ---------------------------------------------------------------------------
add_executable(CurvesEditor
    src/main.cpp
    src/CGInit.cpp
    src/Hermitte.cpp
    src/Models.cpp
    src/ShaderMaker.cpp
)
target_link_libraries(CurvesEditor PRIVATE glad glfw glm::glm imgui OpenGL::GL)
# glm/gtx/transform.hpp is an "experimental" extension
target_compile_definitions(CurvesEditor PRIVATE GLM_ENABLE_EXPERIMENTAL)
if(MSVC)
    target_compile_definitions(CurvesEditor PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Shaders are loaded by relative path at runtime: copy them next to the executable.
file(GLOB SHADERS ${CMAKE_SOURCE_DIR}/shaders/*.glsl)
add_custom_command(TARGET CurvesEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHADERS} $<TARGET_FILE_DIR:CurvesEditor>
)
