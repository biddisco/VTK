/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDiscretizableColorTransferFunctionCollection.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkDiscretizableColorTransferFunctionCollection.h"

#include "vtkCommand.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPiecewiseFunction.h"
#include "vtkTuple.h"
#include "vtkGaussianPiecewiseFunction.h"
#include "vtkTwoDTransferFunction.h"

#include <vector>

vtkStandardNewMacro(vtkDiscretizableColorTransferFunctionCollection);
vtkCxxSetObjectMacro(vtkDiscretizableColorTransferFunctionCollection,
  GradientLinearOpacityFunction, vtkPiecewiseFunction);
vtkCxxSetObjectMacro(vtkDiscretizableColorTransferFunctionCollection,
  GradientGaussianOpacityFunction, vtkGaussianPiecewiseFunction);
vtkCxxSetObjectMacro(vtkDiscretizableColorTransferFunctionCollection,
  ScalarGaussianOpacityFunction, vtkGaussianPiecewiseFunction);
//-----------------------------------------------------------------------------
vtkDiscretizableColorTransferFunctionCollection::vtkDiscretizableColorTransferFunctionCollection()
{

  this->GradientLinearOpacityFunction = 0;
  this->ScalarGaussianOpacityFunction = 0;
  this->GradientGaussianOpacityFunction = 0;
  this->TwoDTransferFunction = 0;
  this->GradientLinearOpacityFunctionObserverId = 0;
  this->ScalarGaussianOpacityFunctionObserverId = 0;
  this->TwoDTransferFunctionObserverId = 0;
  this->GradientLinearOpacityFunctionObserverId = 0;
  this->GradientGaussianOpacityFunctionObserverId = 0;

}
//-----------------------------------------------------------------------------
vtkDiscretizableColorTransferFunctionCollection::~vtkDiscretizableColorTransferFunctionCollection()
{
  // this removes any observer we may have setup for the
  // ScalarOpacityFunction.
  this->SetGradientLinearOpacityFunction(NULL);
  this->SetScalarGaussianOpacityFunction(NULL);
  this->SetGradientGaussianOpacityFunction(NULL);
}
//-----------------------------------------------------------------------------
vtkPiecewiseFunction* vtkDiscretizableColorTransferFunctionCollection::GetGradientLinearOpacityFunction() const
{
  return this->GradientLinearOpacityFunction;
}
//-----------------------------------------------------------------------------

vtkGaussianPiecewiseFunction* vtkDiscretizableColorTransferFunctionCollection::GetScalarGaussianOpacityFunction() const
{
  return this->ScalarGaussianOpacityFunction;
}




vtkGaussianPiecewiseFunction* vtkDiscretizableColorTransferFunctionCollection::GetGradientGaussianOpacityFunction() const
{
  return this->GradientGaussianOpacityFunction;
}
//-----------------------------------------------------------------------------
void vtkDiscretizableColorTransferFunctionCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vtkDiscretizableColorTransferFunctionCollection::SetTwoDTransferFunction(vtkTwoDTransferFunction *function){
	this->TwoDTransferFunction = function;
}

vtkTwoDTransferFunction* vtkDiscretizableColorTransferFunctionCollection::GetTwoDTransferFunction() const{
	return this->TwoDTransferFunction;
}
/*
//-----------------------------------------------------------------------------
void vtkDiscretizableColorTransferFunction::SetUseLogScale(int useLogScale)
{
  if(this->UseLogScale != useLogScale)
    {
    this->UseLogScale = useLogScale;
    if(this->UseLogScale)
      {
      this->LookupTable->SetScaleToLog10();
      this->SetScaleToLog10();
      }
    else
      {
      this->LookupTable->SetScaleToLinear();
      this->SetScaleToLinear();
      }

    this->Modified();
    }
}
*/



