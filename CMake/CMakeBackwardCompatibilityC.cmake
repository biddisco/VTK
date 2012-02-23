# Nothing here yet
IF(CMAKE_GENERATOR MATCHES "Visual Studio 7")
  INCLUDE(CMakeVS7BackwardCompatibility)
  SET(CMAKE_SKIP_COMPATIBILITY_TESTS 1)
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 7")
IF(CMAKE_GENERATOR MATCHES "Visual Studio 6")
  INCLUDE(CMakeVS6BackwardCompatibility)
  SET(CMAKE_SKIP_COMPATIBILITY_TESTS 1)
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 6")

IF(NOT CMAKE_SKIP_COMPATIBILITY_TESTS)
  INCLUDE (CheckIncludeFiles)
  CHECK_INCLUDE_FILES("limits.h"       CMAKE_HAVE_LIMITS_H)
  CHECK_INCLUDE_FILES("unistd.h"       CMAKE_HAVE_UNISTD_H)
  CHECK_INCLUDE_FILES("pthread.h"      CMAKE_HAVE_PTHREAD_H)
  CHECK_INCLUDE_FILES("sys/types.h;sys/prctl.h"    CMAKE_HAVE_SYS_PRCTL_H)
  # include the following file to keep stuff which relies on the fact that it 
  # is included here, keeps working
  INCLUDE (CheckIncludeFile)

  INCLUDE (TestBigEndian)
  TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)
       
  IF(NOT VTK_DONT_INCLUDE_USE_X)
    IF(NOT APPLE OR VTK_USE_X)
      INCLUDE (FindX11)
    ENDIF(NOT APPLE OR VTK_USE_X)
  ENDIF(NOT VTK_DONT_INCLUDE_USE_X)

  IF("${X11_X11_INCLUDE_PATH}" MATCHES "^/usr/include$")
    SET (CMAKE_X_CFLAGS "" CACHE STRING "X11 extra flags.")
  ELSE("${X11_X11_INCLUDE_PATH}" MATCHES "^/usr/include$")
    SET (CMAKE_X_CFLAGS "-I${X11_X11_INCLUDE_PATH}" CACHE STRING
         "X11 extra flags.")
  ENDIF("${X11_X11_INCLUDE_PATH}" MATCHES "^/usr/include$")
  SET (CMAKE_X_LIBS "${X11_LIBRARIES}" CACHE STRING
       "Libraries and options used in X11 programs.")
  SET (CMAKE_HAS_X "${X11_FOUND}" CACHE INTERNAL "Is X11 around.")

  INCLUDE (FindThreads)

  SET (CMAKE_THREAD_LIBS        "${CMAKE_THREAD_LIBS_INIT}" CACHE STRING 
    "Thread library used.")

  SET (CMAKE_USE_PTHREADS       "${CMAKE_USE_PTHREADS_INIT}" CACHE BOOL
     "Use the pthreads library.")

  SET (CMAKE_USE_WIN32_THREADS  "${CMAKE_USE_WIN32_THREADS_INIT}" CACHE BOOL
       "Use the win32 thread library.")

  SET (CMAKE_HP_PTHREADS        ${CMAKE_HP_PTHREADS_INIT} CACHE BOOL
     "Use HP pthreads.")

  SET (CMAKE_USE_SPROC          ${CMAKE_USE_SPROC_INIT} CACHE BOOL 
     "Use sproc libs.")

  IF(MINGW AND CMAKE_USE_PTHREADS AND CMAKE_USE_WIN32_THREADS)
    MESSAGE(STATUS
      "Forcing CMAKE_USE_PTHREADS OFF as only one threading library should be used.")
    SET(CMAKE_USE_PTHREADS OFF CACHE BOOL
      "Force pthreads off if WIN32 threads are enabled" FORCE)
    SET(CMAKE_THREAD_LIBS "" CACHE STRING "Thread library used." FORCE)
  ELSE()
    SET(CMAKE_THREAD_LIBS "${CMAKE_THREAD_LIBS_INIT}" CACHE STRING "Thread library used.")
  ENDIF()
ENDIF(NOT CMAKE_SKIP_COMPATIBILITY_TESTS)

SET (VTK_MAX_THREADS          "64" CACHE STRING
     "Max number of threads vktMultiThreader will allocate.")

MARK_AS_ADVANCED(
CMAKE_HP_PTHREADS
CMAKE_THREAD_LIBS
CMAKE_USE_PTHREADS
CMAKE_USE_SPROC
CMAKE_USE_WIN32_THREADS
CMAKE_X_CFLAGS
CMAKE_X_LIBS
VTK_MAX_THREADS
)

