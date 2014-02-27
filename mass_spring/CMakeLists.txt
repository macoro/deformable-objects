cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(mass_spring)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/glm-0.9.3.0")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/gli-0.3.1.0")
include_directories("/usr/include" "/usr/local/include")

set(CMAKE_CXX_COMPILER g++)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wextra -DDEBUG -D_DEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -DDEBUG -D_DEBUG")
endif(CMAKE_COMPILER_IS_GNUCXX)

################################
# Add GLEXT 
find_package(OpenGL)
if(NOT OPENGL_FOUND)
	message("ERROR: OpenGL not found")
endif(NOT OPENGL_FOUND)

################################
# Add GL lib
if(UNIX)
	set(GL_LIBRARY GL)
	link_directories("/usr/lib")
	include_directories("/usr/include/GL")	
endif(UNIX)
################################
# Add GLEW
if(UNIX)
	set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,/usr/lib64")
	set(GLEW_LIBRARY GLEW)	
endif(UNIX)
################################
# Add FreeGLUT
add_definitions(-DFREEGLUT_STATIC)
if(UNIX)
	set(FREEGLUT_LIBRARY glut)
endif(UNIX)
#################################
#Add X11 lib
if(UNIX)
	link_directories("/usr/X11R6/lib" "/usr/X11R6/lib64")
	set(X11_LIBRARY X11)
endif(UNIX)

################################
# Add libraries to executables
set(BINARY_FILES 
	${GL_LIBRARY} 
	${FREEGLUT_LIBRARY} 
	${GLEW_LIBRARY}
	${GLEW_LIBRARIES}
	${X11_LIBRARY})

################################
# Add sources
set(massSpring_SRCS mass_spring.cpp camera.cpp tetramesh.cpp trimesh.cpp)

################################
set(CMAKE_BUILD_TYPE Debug)
set(NAME mass_spring)
add_executable(${NAME} ${massSpring_SRCS})
target_link_libraries(${NAME} ${BINARY_FILES})

