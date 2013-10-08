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
  GradientOpacityFunction, vtkPiecewiseFunction);
vtkCxxSetObjectMacro(vtkDiscretizableColorTransferFunctionCollection,
  GaussianOpacityFunction, vtkGaussianPiecewiseFunction);
//-----------------------------------------------------------------------------
vtkDiscretizableColorTransferFunctionCollection::vtkDiscretizableColorTransferFunctionCollection()
{

  this->GradientOpacityFunction = vtkPiecewiseFunction::New();
  this->GradientOpacityFunctionObserverId = 0;
  this->GradientOpacityFunction->AddPoint(37.0, 0.0);
  this->GradientOpacityFunction->AddPoint(150.0, 1.0);

}
//-----------------------------------------------------------------------------
vtkDiscretizableColorTransferFunctionCollection::~vtkDiscretizableColorTransferFunctionCollection()
{
  // this removes any observer we may have setup for the
  // ScalarOpacityFunction.
  this->SetGradientOpacityFunction(NULL);
  this->SetGaussianOpacityFunction(NULL);
}
//-----------------------------------------------------------------------------
vtkPiecewiseFunction* vtkDiscretizableColorTransferFunctionCollection::GetGradientOpacityFunction() const
{
  return this->GradientOpacityFunction;
}
//-----------------------------------------------------------------------------

vtkGaussianPiecewiseFunction* vtkDiscretizableColorTransferFunctionCollection::GetGaussianOpacityFunction() const
{
  return this->GaussianOpacityFunction;
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




