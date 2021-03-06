cmake_minimum_required(VERSION 2.8.5 FATAL_ERROR)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW) # remove if CMake >= 2.8.11 required
endif()

PROJECT (Modelling)

if(NOT VTK_BINARY_DIR)
  find_package(VTK COMPONENTS
    vtkCommonCore
    vtkCommonDataModel
    vtkFiltersCore
    vtkFiltersGeneral
    vtkIOXML
    vtkImagingHybrid
    vtkRenderingCore
    vtkRenderingOpenGL
    vtkInteractionStyle
)
  include(${VTK_USE_FILE})
endif()

add_executable(finance MACOSX_BUNDLE finance.cxx)
target_link_libraries(finance ${VTK_LIBRARIES})

add_executable(Delaunay3D MACOSX_BUNDLE Delaunay3D.cxx)
target_link_libraries(Delaunay3D ${VTK_LIBRARIES})

add_executable(Delaunay3DAlpha MACOSX_BUNDLE Delaunay3DAlpha.cxx)
target_link_libraries(Delaunay3DAlpha ${VTK_LIBRARIES})

if(BUILD_TESTING)
  if(vtkTestingRendering_LOADED)
    ######## Regression Testing ########
    set(vtk-example ModellingExamples)
    set(TestFinance_ARGS "DATA{${VTK_TEST_INPUT_DIR}/financial.txt}")
    set(TestDelaunay3D_ARGS
      "DATA{${VTK_TEST_INPUT_DIR}/cow.vtp}"
      ${VTK_TEST_OUTPUT_DIR}/Delaunay3D.vtu
      )
    set(TestDelaunay3DAlpha_ARGS
      .5 "DATA{${VTK_TEST_INPUT_DIR}/cow.vtp}"
      ${VTK_TEST_OUTPUT_DIR}/Delaunay3DAlpha.vtu
      )
    vtk_add_test_cxx(
      TestFinance.cxx
      )
    vtk_add_test_cxx(NO_DATA NO_VALID NO_OUTPUT
      TestDelaunay3D.cxx
      TestDelaunay3DAlpha.cxx
      )
    vtk_test_cxx_executable(${vtk-example}CxxTests RENDERING_FACTORY)
  endif()
endif()
