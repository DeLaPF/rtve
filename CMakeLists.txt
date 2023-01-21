include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.11.0)
project(rtve VERSION 0.0.1)
message(STATUS "using ${CMAKE_GENERATOR}")

# Mingw
if (NOT MINGW_PATH) 
    message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
endif()
set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
set(CMAKE_CXX_COMPILER  ${MINGW_PATH}/bin/g++.exe)
message(STATUS "using mingw at: ${MINGW_PATH}")

# Vulkan
if (NOT VULKAN_SDK_PATH) 
    message(FATAL_ERROR "VULKAN_SDK_PATH not set in .env.cmake")
endif()
set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")

# Glfw
if (NOT GLFW_PATH) 
    message(FATAL_ERROR "GLFW_PATH not set in .env.cmake")
endif()
set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
set(GLFW_LIB "${GLFW_PATH}/lib-mingw-w64") # 2.1 make sure matches glfw mingw subdirectory
message(STATUS "Using glfw lib at: ${GLFW_LIB}")

# ImGui
set(LIB_NAME ImGui)
message(STATUS "ADDING ${LIB_NAME}")
file(GLOB ImGui ${PROJECT_SOURCE_DIR}/libs/imgui/*.cpp)
file(GLOB ImGui_headers ${PROJECT_SOURCE_DIR}/libs/imgui/*.h)
file(GLOB ImGui_vulkan ${PROJECT_SOURCE_DIR}/libs/imgui/backends/*vulkan.cpp)
file(GLOB ImGui_vulkan_headers ${PROJECT_SOURCE_DIR}/libs/imgui/backends/*vulkan.h)
file(GLOB ImGui_glfw ${PROJECT_SOURCE_DIR}/libs/imgui/backends/*glfw.cpp)
file(GLOB ImGui_glfw_headers ${PROJECT_SOURCE_DIR}/libs/imgui/backends/*glfw.h)
add_library(${LIB_NAME}
    ${ImGui}
    ${ImGui_headers}
    ${ImGui_vulkan}
    ${ImGui_vulkan_headers}
    ${ImGui_glfw}
    ${ImGui_glfw_headers}
)
target_compile_definitions(${LIB_NAME} PUBLIC
    WL_PLATFORM_WINDOWS
    WL_DIST
)
target_include_directories(${LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/libs/imgui
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
)
target_link_libraries(${LIB_NAME} PUBLIC
    vulkan-1
)

# Walnut
set(LIB_NAME Walnut)
message(STATUS "ADDING ${LIB_NAME}")
file(GLOB Walnut ${PROJECT_SOURCE_DIR}/libs/Walnut/*.cpp)
file(GLOB Walnut_headers ${PROJECT_SOURCE_DIR}/libs/Walnut/*.h)
add_library(${LIB_NAME}
    ${Walnut}
    ${Walnut_headers}
)
target_include_directories(${LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/libs/stb_image
    ${GLM_PATH}
)
target_link_directories(${LIB_NAME} PUBLIC
    ${Vulkan_LIBRARIES}
    ${GLFW_LIB}
)
target_link_libraries(${LIB_NAME} PUBLIC
    glfw3
    ImGui
)

# Rtve
message(STATUS "ADDING ${PROJECT_NAME}")
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME}
    ${SOURCES}
)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/libs
)
target_link_libraries(${PROJECT_NAME} PUBLIC
    -static
    Walnut
)
