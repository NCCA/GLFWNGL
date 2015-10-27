cmake_minimum_required(VERSION 2.8.11)
# Name of the project
set(PROJECT_NAME GLFWNGL)
project(${PROJECT_NAME})
#Bring the headers into the project (local ones)
include_directories(include $ENV{HOME}/NGL/include)
find_package(PkgConfig REQUIRED)
set( ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig" )
# use C++ 11
set(CMAKE_CXX_STANDARD 11)

pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

#the file(GLOB...) allows for wildcard additions of our src dir
set(SOURCES src/main.cpp  src/NGLDraw.cpp include/NGLDraw.h  )

# see what platform we are on and set platform defines
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	add_definitions(-DDARWIN)
	find_library(MACGL OpenGL)
	find_library(MacIOKit IOKit) 
	find_library(MacCocoa Cocoa)
	find_library(MacCoreVideo CoreVideo)
	set ( PROJECT_LINK_LIBS -lNGL ${MACGL} ${MacIOKit}  ${MacCocoa} ${MacCoreVideo} )

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_definitions(-DLINUX)
	set ( PROJECT_LINK_LIBS -lNGL -lGL -lX11 -lXxf86vm -lXrandr -lXi -lXinerama -lXcursor)

endif()

# now add NGL specific values
link_directories( ~/NGL/lib /usr/lib64 )
include_directories(~/NGL/include/)
# as NGL uses Qt we need to define this flag
add_definitions(-DQT5BUILD)

# NGL also needs the OpenGL framework from Qt so add it
find_package(Qt5OpenGL)

# add exe and link libs this must be after the other defines
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LINK_LIBS} Qt5::OpenGL  ${GLFW_LIBRARIES}   )

