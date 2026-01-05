cmake_minimum_required(VERSION 3.10)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

#Create a project with this name
project ("GRAPH-Master1")

include(FetchContent)

# Set directory of third party libraries
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty CACHE PATH "Base Directory of libs" FORCE)

#Get GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG        3.4 
)

# Disable building GLFW's extra tools to save time
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

#Set GLFW for this project
FetchContent_MakeAvailable(glfw)

#OpenGL should be preinstalled
find_package(OpenGL REQUIRED)

# Add source to this project's executable.
# Change these to your files
add_executable (GRAPH-Master1 "GRAPH-Master1.cpp" "GRAPH-Master1.h")

#Link the required libraries
target_link_libraries(GRAPH-Master1
						glfw
						OpenGL::GL
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GRAPH-Master1 PROPERTY CXX_STANDARD 20)
endif()