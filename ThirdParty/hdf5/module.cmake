
if(VTK_USE_SYSTEM_HDF5)
  #-----------------------------------------------
  # Check if HDF5 was compiled with Parallel support
  #-----------------------------------------------
  INCLUDE (${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake)
  UNSET(HAVE_H5_HAVE_PARALLEL CACHE)
  SET(CMAKE_REQUIRED_INCLUDES ${HDF5_INCLUDE_DIR} ${VTK_INCLUDE_DIRS})
  CHECK_SYMBOL_EXISTS(H5_HAVE_PARALLEL "H5pubconf.h" HAVE_H5_HAVE_PARALLEL)

  if (HAVE_H5_HAVE_PARALLEL)
    if (NOT PARAVIEW_USE_MPI)
      MESSAGE(ERROR "System HDF5 uses MPI but PARAVIEW_USE_MPI is ${PARAVIEW_USE_MPI}")
    endif(NOT PARAVIEW_USE_MPI)
       #
       # In here we need to say conditionally depend on some kind of find_package(MPI)
       #
       set(vtkhdf5_SYSTEM_INCLUDE_DIRS "${vtkhdf5_SYSTEM_INCLUDE_DIRS};${MPI_C_INCLUDE_PATH}")
  endif (HAVE_H5_HAVE_PARALLEL)

  include_directories(${MPI_INCLUDE_DIR})
endif(VTK_USE_SYSTEM_HDF5)

vtk_module(vtkhdf5
  DEPENDS
    vtkzlib
  EXCLUDE_FROM_WRAPPING
  )
