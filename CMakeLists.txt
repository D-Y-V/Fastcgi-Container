cmake_minimum_required (VERSION 2.10)

include(CheckLibraryExists)
include(CheckIncludeFileCXX)

project(FastcgiContainer CXX)

set(FASTCGICONTAINER_MAJOR_VERSION 1)
set(FASTCGICONTAINER_MINOR_VERSION 0)
set(FASTCGICONTAINER_PATCH_VERSION 0)
set(FASTCGICONTAINER_VERSION
  ${FASTCGICONTAINER_MAJOR_VERSION}.${FASTCGICONTAINER_MINOR_VERSION}.${FASTCGICONTAINER_PATCH_VERSION}
)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
	message("Compiler ${CMAKE_CXX_COMPILER} has C++17 support.")
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has NO C++17 support.")
endif()

# around libxml2
find_package(LibXml2 REQUIRED)
message(STATUS LIBXML2_INCLUDE_DIR = ${LIBXML2_INCLUDE_DIR})
include_directories(${LIBXML2_INCLUDE_DIR})




if(NOT CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE Release)
   set(CMAKE_BUILD_TYPE Release CACHE STRING
			"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
			FORCE)
endif(NOT CMAKE_BUILD_TYPE)
message("-- ${CMAKE_BUILD_TYPE} mode")

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/FastcgiContainer)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# set up include-directories
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++17") # -g !!! DEBUG
set(LIBS "${LIBS} -lpthread")

if(NOT CMAKE_BUILD_TYPE MATCHES DEBUG)
    if(CMAKE_COMPILER_IS_GNUCXX)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s") 	# Strip binary
	endif(CMAKE_COMPILER_IS_GNUCXX)
endif(NOT CMAKE_BUILD_TYPE MATCHES DEBUG)


# Add sub-directories
add_subdirectory(main)
add_subdirectory(library)
add_subdirectory(request-cache)
add_subdirectory(file-logger)
add_subdirectory(syslog)
add_subdirectory(statistics)
add_subdirectory(session-manager)
add_subdirectory(authenticator)
###add_subdirectory(page-compiler) ## TODO temporary not work
###add_subdirectory(example)
###add_subdirectory(logging)


# Add all targets to the build-tree export set

###		fastcgi3-page-compiler  ## TODO temporary not work

export(
	TARGETS 
		fastcgi3-container
		fastcgi3-syslog
		fastcgi3-filelogger
		fastcgi3-statistics
		fastcgi3-request-cache
		fastcgi3-session-manager
		fastcgi3-authenticator
		fastcgi3-daemon
  	FILE "${PROJECT_BINARY_DIR}/FastcgiContainerTargets.cmake"
)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE FastcgiContainer)

# Create the FastcgiContainerConfig.cmake and FastcgiContainerConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(FastcgiContainerConfig.cmake.in "${PROJECT_BINARY_DIR}/FastcgiContainerConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${FASTCGICONTAINER_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(FastcgiContainerConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FastcgiContainerConfig.cmake" @ONLY)
# ... for both
configure_file(FastcgiContainerConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/FastcgiContainerConfigVersion.cmake" @ONLY)

# Install the FastcgiContainerConfig.cmake and FastcgiContainerConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FastcgiContainerConfig.cmake"
  "${PROJECT_BINARY_DIR}/FastcgiContainerConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT FastcgiContainerTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)



