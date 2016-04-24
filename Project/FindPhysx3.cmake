
FIND_PATH(NVPX3_INCLUDE_DIR PxPhysics.h
	HINTS
	$ENV{NVPX3}
	PATH_SUFFIXES include
	PATHS
	/usr/lib
	d:/lib/NVIDIA/PhysX3
)

LIST(PX_LIBS APPEND PhysX3Common PhysX3Cooking PhysX3 PhysX3Extensions PhysX3Vehicle PxTask PhysX3Gpu)



FOREACH(PX_LIB ${PX_LIBS})

ENDFOREACH()

# Lookup the 64 bit libs on x64
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(NVPX3_LIBRARY_TEMP NVPX3
		HINTS
		$ENV{NVPX3}
		PATH_SUFFIXES lib64 lib
		lib/x64
		lib/Win64
		PATHS
		/sw
		/opt/local
		/opt/csw
		/opt
	)
# On 32bit build find the 32bit libs
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(NVPX3_LIBRARY_TEMP NVPX3
		HINTS
		$ENV{NVPX3}
		PATH_SUFFIXES lib
		lib/x86
		lib/Win32
		PATHS
		/usr/lib
		d:/lib/NVIDIA/PhysX3
	)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)

IF(NOT APPLE)
	FIND_PACKAGE(Threads)
ENDIF(NOT APPLE)

IF(MINGW)
	SET(MINGW32_LIBRARY mingw32 CACHE STRING "mwindows for MinGW")
ENDIF(MINGW)

SET(NVPX3_FOUND "NO")
	
		SET(NVPX3_FOUND "YES")
ENDIF(NVPX3_LIBRARY_TEMP)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NVPX3 REQUIRED_VARS NVPX3_LIBRARY NVPX3_INCLUDE_DIR)

