set(Module_SRCS
  vtkSILBuilder.cxx
  vtkXdmfDataArray.cxx
  vtkXdmfHeavyData.cxx
  vtkXdmfReader.cxx
  vtkXdmfReaderInternal.cxx
  vtkXdmfWriter.cxx
  )

set_source_files_properties(
  vtkXdmfDataArray
  vtkXdmfHeavyData
  vtkXdmfReaderInternal
  WRAP_EXCLUDE
  )
find_package(HDF5 REQUIRED NO_MODULE)
include_directories(${HDF5_INCLUDE_DIR})
#set(${vtk-module}_NO_HeaderTest 1) # TODO: helper classes and reenable
vtk_module_library(vtkIOXdmf2 ${Module_SRCS})
