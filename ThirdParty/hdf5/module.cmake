
if(VTK_USE_SYSTEM_HDF5)
  if (HDF5_ENABLE_PARALLEL)
    if (NOT PARAVIEW_USE_MPI)
      MESSAGE(ERROR "System HDF5 uses MPI but PARAVIEW_USE_MPI is ${PARAVIEW_USE_MPI}")
    endif(NOT PARAVIEW_USE_MPI)
       #
       # In here we need to say conditionally depend on some kind of find_package(MPI)
       #
       set(vtkhdf5_SYSTEM_INCLUDE_DIRS "${vtkhdf5_SYSTEM_INCLUDE_DIRS};${MPI_C_INCLUDE_PATH}")
  endif (HDF5_ENABLE_PARALLEL)
endif(VTK_USE_SYSTEM_HDF5)

vtk_module(vtkhdf5
  DEPENDS
    vtkzlib
  EXCLUDE_FROM_WRAPPING
  )
