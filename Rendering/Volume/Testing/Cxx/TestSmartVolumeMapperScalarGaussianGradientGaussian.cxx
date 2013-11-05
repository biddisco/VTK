/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestSmartVolumeMapper.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// This test covers the smart volume mapper and composite method.
// This test volume renders a synthetic dataset with unsigned char values,
// with the composite method.

#include "vtkSphere.h"
#include "vtkSampleFunction.h"

#include "vtkSmartVolumeMapper.h"
#include "vtkTestUtilities.h"
#include "vtkColorTransferFunction.h"
#include "vtkGaussianPiecewiseFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolumeProperty.h"
#include "vtkCamera.h"
#include "vtkRegressionTestImage.h"
#include "vtkImageShiftScale.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

int TestSmartVolumeMapperScalarGaussianGradientGaussian(int argc,
                          char *argv[])
{
  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;

  // Create a spherical implicit function.
  vtkSphere *shape=vtkSphere::New();
  shape->SetRadius(0.1);
  shape->SetCenter(0.0,0.0,0.0);

  vtkSampleFunction *source=vtkSampleFunction::New();
  source->SetImplicitFunction(shape);
  shape->Delete();
  source->SetOutputScalarTypeToDouble();
  source->SetSampleDimensions(127,127,127); // intentional NPOT dimensions.
  source->SetModelBounds(-1.0,1.0,-1.0,1.0,-1.0,1.0);
  source->SetCapping(false);
  source->SetComputeNormals(false);
  source->SetScalarArrayName("values");

  source->Update();

  vtkDataArray *a=source->GetOutput()->GetPointData()->GetScalars("values");
  double range[2];
  a->GetRange(range);

  vtkImageShiftScale *t=vtkImageShiftScale::New();
  t->SetInputConnection(source->GetOutputPort());
  source->Delete();
  t->SetShift(-range[0]);
  double magnitude=range[1]-range[0];
  if(magnitude==0.0)
    {
    magnitude=1.0;
    }
  t->SetScale(255.0/magnitude);
  t->SetOutputScalarTypeToUnsignedChar();

  t->Update();

  vtkRenderWindow *renWin=vtkRenderWindow::New();
  vtkRenderer *ren1=vtkRenderer::New();
  ren1->SetBackground(0.1,0.4,0.2);

  renWin->AddRenderer(ren1);
  ren1->Delete();
  renWin->SetSize(301,300); // intentional odd and NPOT  width/height

  vtkRenderWindowInteractor *iren=vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  renWin->Delete();

  renWin->Render(); // make sure we have an OpenGL context.

  vtkSmartVolumeMapper *volumeMapper;
  vtkVolumeProperty *volumeProperty;
  vtkVolume *volume;

  volumeMapper=vtkSmartVolumeMapper::New();
  volumeMapper->SetBlendModeToComposite(); // composite first
  volumeMapper->SetInputConnection(
    t->GetOutputPort());

  volumeProperty=vtkVolumeProperty::New();
  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);



  vtkGaussianPiecewiseFunction *scalargaussianOpacity = vtkGaussianPiecewiseFunction::New();
  scalargaussianOpacity->SetRange(0,255);
 // scalargaussianOpacity->AddGaussian(20.0,0.2,11,0.0,0.0);
  scalargaussianOpacity->AddGaussian(40.0,0.5,20,0.0,0.0);
  scalargaussianOpacity->AddGaussian(70.0,0.8,10,0.0,0.0);
 // scalargaussianOpacity->AddGaussian(180.0,1.0,1,0.0,2.0);
  volumeProperty->SetScalarGaussianOpacity(scalargaussianOpacity);
  volumeProperty->SwitchScalarOpacity(true);

  vtkGaussianPiecewiseFunction *gradientGaussianOpacity = vtkGaussianPiecewiseFunction::New();
  gradientGaussianOpacity->SetRange(0,255);
 // gradientLinearOpacity->AddGaussian(0.0,1.0);
  gradientGaussianOpacity->RemoveAllGaussians();
  gradientGaussianOpacity->AddGaussian(2,1.0,0.01,0.0,0.0);
 // gradientGaussianOpacity->AddGaussian(149.0,1.0,150.0,0.0,2.0);
  volumeProperty->SetGradientGaussianOpacity(gradientGaussianOpacity); // composite first.
  volumeProperty->SwitchGradientOpacity(true);
  volumeProperty->SetDisableGradientGaussianOpacity(false);



  vtkColorTransferFunction *color=vtkColorTransferFunction::New();
  color->AddRGBPoint(0.0 ,0.0,0.0,1.0);
  color->AddRGBPoint(40.0 ,1.0,0.0,0.0);
  color->AddRGBPoint(255.0,1.0,1.0,1.0);
  volumeProperty->SetColor(color);
  color->Delete();

  volume=vtkVolume::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  ren1->AddViewProp(volume);

  int retVal;

  ren1->ResetCamera();

  // Render composite. In default mode. For coverage.
  renWin->Render();

  // 3D texture mode. For coverage.
  volumeMapper->SetRequestedRenderModeToRayCastAndTexture();
  renWin->Render();

  // Software mode, for coverage. It also makes sure we will get the same
  // regression image on all platforms.
  volumeMapper->SetRequestedRenderModeToRayCast();
  renWin->Render();

  retVal = vtkTesting::Test(argc, argv, renWin, 75);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    iren->Start();
    }

    volumeProperty->SwitchScalarOpacity(true);

  //volumeProperty->SwitchScalarOpacity(true);
  renWin->Render();









  volumeMapper->Delete();
  volumeProperty->Delete();
  volume->Delete();
  iren->Delete();
  t->Delete();
  scalargaussianOpacity->Delete();
  gradientGaussianOpacity->Delete();



return !(retVal == vtkTesting::DO_INTERACTOR || retVal == vtkTesting::PASSED);

}
