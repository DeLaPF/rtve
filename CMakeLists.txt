if (WIN32)
    include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
    message(STATUS "Local .env.cmake: ${LOCAL_ENV}")
endif()

cmake_minimum_required(VERSION 3.11.0)
project(rtve VERSION 0.0.1)

# Mingw
message(STATUS "using ${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()
    set(USE_MINGW "True")
    set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)
    message(STATUS "using mingw at: ${MINGW_PATH}")
endif()

# Vulkan
if (DEFINED VULKAN_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

# Glfw
if (DEFINED GLFW_PATH)
    message(STATUS "Using GLFW path specified in .env")
    set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
    set(GLFW_LIB "${GLFW_PATH}/lib-mingw-w64") # 2.1 make sure matches glfw mingw subdirectory
else()
    find_package(glfw3 3.3 REQUIRED)
    set(GLFW_LIB glfw)
    message(STATUS "Found GLFW")
endif()
if (NOT GLFW_LIB)
	message(FATAL_ERROR "Could not find glfw library!")
else()
	message(STATUS "Using glfw lib at: ${GLFW_LIB}")
endif()

# ImGui
set(LIB_NAME ImGui)
message(STATUS "ADDING ${LIB_NAME}")
file(GLOB ImGui ${PROJECT_SOURCE_DIR}/libs/Walnut/vendor/imgui/*.cpp)
file(GLOB ImGui_headers ${PROJECT_SOURCE_DIR}/libs/Walnut/vendor/imgui/*.h)
file(GLOB ImGui_vulkan ${PROJECT_SOURCE_DIR}/libs/Walnut/vendor/imgui/backends/*vulkan.cpp)
file(GLOB ImGui_vulkan_headers ${PROJECT_SOURCE_DIR}/libs/Walnut/vendor/imgui/backends/*vulkan.h)
file(GLOB ImGui_glfw ${PROJECT_SOURCE_DIR}/libs/Walnut/vendor/imgui/backends/*glfw.cpp)
file(GLOB ImGui_glfw_headers ${PROJECT_SOURCE_DIR}/libs/Walnut/vendor/imgui/backends/*glfw.h)
add_library(${LIB_NAME}
    ${ImGui}
    ${ImGui_headers}
    ${ImGui_vulkan}
    ${ImGui_vulkan_headers}
    ${ImGui_glfw}
    ${ImGui_glfw_headers}
)
if (WIN32)
    target_compile_definitions(${LIB_NAME} PUBLIC
        WL_PLATFORM_WINDOWS
        WL_DIST
    )
endif()
target_include_directories(${LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/libs/Walnut/vendor/imgui
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
)
if (WIN32)
    target_link_libraries(${LIB_NAME} PUBLIC
        vulkan-1
    )
else()
    target_link_libraries(${LIB_NAME} PUBLIC
        ${Vulkan_LIBRARIES}
    )
endif()

# Walnut
set(LIB_NAME Walnut)
message(STATUS "ADDING ${LIB_NAME}")
file(GLOB_RECURSE Walnut ${PROJECT_SOURCE_DIR}/libs/Walnut/Walnut/src/Walnut/*.cpp)
file(GLOB_RECURSE Walnut_headers ${PROJECT_SOURCE_DIR}/libs/Walnut/Walnut/src/Walnut/*.h)
add_library(${LIB_NAME}
    ${Walnut}
    ${Walnut_headers}
)
target_include_directories(${LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/libs/Walnut/Walnut/src
    ${PROJECT_SOURCE_DIR}/libs/Walnut/vendor/stb_image
    ${GLM_PATH}
)
target_link_directories(${LIB_NAME} PUBLIC
    ${Vulkan_LIBRARIES}
    ${GLFW_LIB}
)
if (WIN32)
    target_link_libraries(${LIB_NAME} PUBLIC
        glfw3
        ImGui
    )
else()
    target_link_libraries(${LIB_NAME} PUBLIC
        glfw
        ImGui
    )
endif()

# Rtve
message(STATUS "ADDING ${PROJECT_NAME}")
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME}
    ${SOURCES}
)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/libs/Walnut/Walnut/src
)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        -static
        Walnut
    )
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC
        Walnut
    )
endif()
