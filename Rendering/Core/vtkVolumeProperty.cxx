/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vtkVolumeProperty.cxx

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/
#include "vtkVolumeProperty.h"

#include "vtkObjectFactory.h"
#include "vtkAbstractPiecewiseFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkGaussianPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkTwoDTransferFunction.h"

vtkStandardNewMacro(vtkVolumeProperty);

// Construct a new vtkVolumeProperty with default values
vtkVolumeProperty::vtkVolumeProperty()
  {

  this->useScalarGaussian = false;
  this->useGradientGaussian = false;

  this->IndependentComponents = 1;

  this->InterpolationType = VTK_NEAREST_INTERPOLATION;

  for (int i = 0; i < VTK_MAX_VRCOMP; i++)
	{
	this->ColorChannels[i] = 1;

	this->GrayTransferFunction[i] = NULL;
	this->RGBTransferFunction[i] = NULL;
	this->ScalarOpacity[i] = NULL;
	this->ScalarOpacityUnitDistance[i] = 1.0;
	this->ScalarGaussianOpacity[i] = NULL;
	this->GradientLinearOpacity[i] = NULL;
	this->GradientGaussianOpacity[i] = NULL;
	this->TwoDTransferFunction[i] = NULL;
	this->DefaultGradientGaussianOpacity[i] = NULL;
	this->DefaultGradientLinearOpacity[i] = NULL;
	this->DefaultTwoDTransferFunction[i] = NULL;
	this->DisableGradientLinearOpacity[i] = 0;
	this->DisableGradientGaussianOpacity[i] = 0;

	this->ComponentWeight[i] = 1.0;

	this->Shade[i] = 0;
	this->Ambient[i] = 0.1;
	this->Diffuse[i] = 0.7;
	this->Specular[i] = 0.2;
	this->SpecularPower[i] = 10.0;
	}

  this->useGradientGaussian = false;
  this->useScalarGaussian = false;
  }

// Destruct a vtkVolumeProperty
vtkVolumeProperty::~vtkVolumeProperty()
  {
  for (int i = 0; i < VTK_MAX_VRCOMP; i++)
	{
	if (this->GrayTransferFunction[i] != NULL)
	  {
	  this->GrayTransferFunction[i]->UnRegister(this);
	  }

	if (this->RGBTransferFunction[i] != NULL)
	  {
	  this->RGBTransferFunction[i]->UnRegister(this);
	  }

	if (this->ScalarOpacity[i] != NULL)
	  {
	  this->ScalarOpacity[i]->UnRegister(this);
	  }

	if (this->GradientLinearOpacity[i] != NULL)
	  {
	  this->GradientLinearOpacity[i]->UnRegister(this);
	  }
	if (this->ScalarGaussianOpacity[i] != NULL)
	  {
	  this->ScalarGaussianOpacity[i]->UnRegister(this);
	  }
	if (this->GradientGaussianOpacity[i] != NULL)
	  {
	  this->GradientGaussianOpacity[i]->UnRegister(this);
	  }
	if (this->TwoDTransferFunction[i] != NULL)
	  {
	  this->TwoDTransferFunction[i]->UnRegister(this);
	  }

	if (this->DefaultGradientLinearOpacity[i] != NULL)
	  {
	  this->DefaultGradientLinearOpacity[i]->UnRegister(this);
	  }
	if (this->DefaultGradientGaussianOpacity[i] != NULL)
	  {
	  this->DefaultGradientGaussianOpacity[i]->UnRegister(this);
	  }
	if (this->DefaultTwoDTransferFunction[i] != NULL)

	  this->DefaultTwoDTransferFunction[i]->UnRegister(this);
	}
  }

void vtkVolumeProperty::DeepCopy(vtkVolumeProperty *p)
  {
  if (!p)
	{
	return;
	}

  this->SetIndependentComponents(p->GetIndependentComponents());

  this->SetInterpolationType(p->GetInterpolationType());

  for (int i = 0; i < VTK_MAX_VRCOMP; i++)
	{
	this->SetComponentWeight(i, p->GetComponentWeight(i));

	// Force ColorChannels to the right value and/or create a default tfunc
	// then DeepCopy all the points

	if (p->GetColorChannels(i) > 1)
	  {
	  this->SetColor(i, this->GetRGBTransferFunction(i));
	  this->GetRGBTransferFunction(i)->DeepCopy(p->GetRGBTransferFunction(i));
	  }
	else
	  {
	  this->SetColor(i, this->GetGrayTransferFunction(i));
	  this->GetGrayTransferFunction(i)->DeepCopy(p->GetGrayTransferFunction(i));
	  }

	this->GetScalarOpacity(i)->DeepCopy(p->GetScalarOpacity(i));

	this->SetScalarOpacityUnitDistance(i, p->GetScalarOpacityUnitDistance(i));

	this->GetGradientLinearOpacity(i)->DeepCopy(p->GetGradientLinearOpacity(i));

	this->SetDisableGradientLinearOpacity(i, p->GetDisableGradientLinearOpacity(i));

	this->SetShade(i, p->GetShade(i));
	this->SetAmbient(i, p->GetAmbient(i));
	this->SetDiffuse(i, p->GetDiffuse(i));
	this->SetSpecular(i, p->GetSpecular(i));
	this->SetSpecularPower(i, p->GetSpecularPower(i));
	}

  this->Modified();
  }

void vtkVolumeProperty::UpdateMTimes()
  {
  this->Modified();

  for (int i = 0; i < VTK_MAX_VRCOMP; i++)
	{
	this->GrayTransferFunctionMTime[i].Modified();
	this->RGBTransferFunctionMTime[i].Modified();
	this->ScalarOpacityMTime[i].Modified();
	this->GradientLinearOpacityMTime[i].Modified();
	this->ScalarGaussianOpacityMTime[i].Modified();
	this->GradientGaussianOpacityMTime[i].Modified();
	this->TwoDTransferFunctionMTime[i].Modified();
	}
  }

unsigned long int vtkVolumeProperty::GetMTime()
  {
  unsigned long mTime = this->vtkObject::GetMTime();
  unsigned long time;

  for (int i = 0; i < VTK_MAX_VRCOMP; i++)
	{
	// Color MTimes
	if (this->ColorChannels[i] == 1)
	  {
	  if (this->GrayTransferFunction[i])
		{
		// time that Gray transfer function pointer was set
		time = this->GrayTransferFunctionMTime[i];
		mTime = (mTime > time ? mTime : time);

		// time that Gray transfer function was last modified
		time = this->GrayTransferFunction[i]->GetMTime();
		mTime = (mTime > time ? mTime : time);
		}
	  }
	else if (this->ColorChannels[i] == 3)
	  {
	  if (this->RGBTransferFunction[i])
		{
		// time that RGB transfer function pointer was set
		time = this->RGBTransferFunctionMTime[i];
		mTime = (mTime > time ? mTime : time);

		// time that RGB transfer function was last modified
		time = this->RGBTransferFunction[i]->GetMTime();
		mTime = (mTime > time ? mTime : time);
		}
	  }

	// Opacity MTimes
	if (this->ScalarOpacity[i])
	  {
	  // time that Scalar opacity transfer function pointer was set
	  time = this->ScalarOpacityMTime[i];
	  mTime = (mTime > time ? mTime : time);

	  // time that Scalar opacity transfer function was last modified
	  time = this->ScalarOpacity[i]->GetMTime();
	  mTime = (mTime > time ? mTime : time);
	  }

	if (this->GradientLinearOpacity[i])
	  {
	  // time that Gradient opacity transfer function pointer was set
	  time = this->GradientLinearOpacityMTime[i];
	  mTime = (mTime > time ? mTime : time);

	  if (!this->DisableGradientLinearOpacity[i])
		{
		// time that Gradient opacity transfer function was last modified
		time = this->GradientLinearOpacity[i]->GetMTime();
		mTime = (mTime > time ? mTime : time);
		}
	  }

	if (this->ScalarGaussianOpacity[i])
		  {
		  // time that ScalarGaussian opacity transfer function pointer was set
		  time = this->ScalarGaussianOpacityMTime[i];
		  mTime = (mTime > time ? mTime : time);

		  // time that ScalarGaussian opacity transfer function was last modified
		  time = this->ScalarGaussianOpacity[i]->GetMTime();
		  mTime = (mTime > time ? mTime : time);
		  }

	if (this->GradientGaussianOpacity[i])
	  {
	  time = this->GradientGaussianOpacityMTime[i];
	  mTime = (mTime > time ? mTime : time);

	  if (!this->DisableGradientGaussianOpacity[i])
		{
		// time that Gradient opacity transfer function was last modified
		time = this->GradientGaussianOpacity[i]->GetMTime();
		mTime = (mTime > time ? mTime : time);
		}
	  }
	if (this->TwoDTransferFunction[i])
	  {
	  time = this->TwoDTransferFunctionMTime[i];
	  mTime = (mTime > time ? mTime : time);

	  if (!this->DisableTwoDTransferFunction[i])
		{
		// time that Gradient opacity transfer function was last modified
		time = this->TwoDTransferFunction[i]->GetMTime();
		mTime = (mTime > time ? mTime : time);
		}
	  }

	}

  return mTime;
  }

int vtkVolumeProperty::GetColorChannels(int index)
  {
  if (index < 0 || index > 3)
	{
	vtkErrorMacro("Bad index - must be between 0 and 3");
	return 0;
	}

  return this->ColorChannels[index];
  }

// Set the color of a volume to a gray transfer function
void vtkVolumeProperty::SetColor(int index, vtkPiecewiseFunction *function)
  {
  if (this->GrayTransferFunction[index] != function)
	{
	if (this->GrayTransferFunction[index] != NULL)
	  {
	  this->GrayTransferFunction[index]->UnRegister(this);
	  }
	this->GrayTransferFunction[index] = function;
	if (this->GrayTransferFunction[index] != NULL)
	  {
	  this->GrayTransferFunction[index]->Register(this);
	  }

	this->GrayTransferFunctionMTime[index].Modified();
	this->Modified();
	}

  if (this->ColorChannels[index] != 1)
	{
	this->ColorChannels[index] = 1;
	this->Modified();
	}
  }

// Get the currently set gray transfer function. Create one if none set.
vtkPiecewiseFunction *vtkVolumeProperty::GetGrayTransferFunction(int index)
  {
  if (this->GrayTransferFunction[index] == NULL)
	{
	this->GrayTransferFunction[index] = vtkPiecewiseFunction::New();
	this->GrayTransferFunction[index]->Register(this);
	this->GrayTransferFunction[index]->Delete();
	this->GrayTransferFunction[index]->AddPoint(0, 0.0);
	this->GrayTransferFunction[index]->AddPoint(1024, 1.0);
	if (this->ColorChannels[index] != 1)
	  {
	  this->ColorChannels[index] = 1;
	  }
	this->Modified();
	}

  return this->GrayTransferFunction[index];
  }

// Set the color of a volume to an RGB transfer function
void vtkVolumeProperty::SetColor(int index, vtkColorTransferFunction *function)
  {
  if (this->RGBTransferFunction[index] != function)
	{
	if (this->RGBTransferFunction[index] != NULL)
	  {
	  this->RGBTransferFunction[index]->UnRegister(this);
	  }
	this->RGBTransferFunction[index] = function;
	if (this->RGBTransferFunction[index] != NULL)
	  {
	  this->RGBTransferFunction[index]->Register(this);
	  }
	this->RGBTransferFunctionMTime[index].Modified();
	this->Modified();
	}

  if (this->ColorChannels[index] != 3)
	{
	this->ColorChannels[index] = 3;
	this->Modified();
	}
  }

// Get the currently set RGB transfer function. Create one if none set.
vtkColorTransferFunction *vtkVolumeProperty::GetRGBTransferFunction(int index)
  {
  if (this->RGBTransferFunction[index] == NULL)
	{
	this->RGBTransferFunction[index] = vtkColorTransferFunction::New();
	this->RGBTransferFunction[index]->Register(this);
	this->RGBTransferFunction[index]->Delete();
	this->RGBTransferFunction[index]->AddRGBPoint(0, 0.0, 0.0, 0.0);
	this->RGBTransferFunction[index]->AddRGBPoint(1024, 1.0, 1.0, 1.0);
	if (this->ColorChannels[index] != 3)
	  {
	  this->ColorChannels[index] = 3;
	  }
	this->Modified();
	}

  return this->RGBTransferFunction[index];
  }

// Set the scalar opacity of a volume to a transfer function
void vtkVolumeProperty::SetScalarOpacity(int index,
	vtkPiecewiseFunction *function)
  {
  if (this->ScalarOpacity[index] != function)
	{
	if (this->ScalarOpacity[index] != NULL)
	  {
	  this->ScalarOpacity[index]->UnRegister(this);
	  }
	this->ScalarOpacity[index] = function;
	if (this->ScalarOpacity[index] != NULL)
	  {
	  this->ScalarOpacity[index]->Register(this);
	  }

	this->ScalarOpacityMTime[index].Modified();
	this->Modified();
	}
  }

// Get the scalar opacity transfer function. Create one if none set.
vtkPiecewiseFunction *vtkVolumeProperty::GetScalarOpacity(int index)
  {
  if (this->ScalarOpacity[index] == NULL)
	{
	this->ScalarOpacity[index] = vtkPiecewiseFunction::New();
	this->ScalarOpacity[index]->Register(this);
	this->ScalarOpacity[index]->Delete();
	this->ScalarOpacity[index]->AddPoint(0, 1.0);
	this->ScalarOpacity[index]->AddPoint(1024, 1.0);
	}

  return this->ScalarOpacity[index];
  }

void vtkVolumeProperty::SetScalarOpacityUnitDistance(int index, double distance)
  {
  if (index < 0 || index > 3)
	{
	vtkErrorMacro("Bad index - must be between 0 and 3");
	return;
	}

  if (this->ScalarOpacityUnitDistance[index] != distance)
	{
	this->ScalarOpacityUnitDistance[index] = distance;
	this->Modified();
	}
  }

double vtkVolumeProperty::GetScalarOpacityUnitDistance(int index)
  {
  if (index < 0 || index > 3)
	{
	vtkErrorMacro("Bad index - must be between 0 and 3");
	return 0;
	}

  return this->ScalarOpacityUnitDistance[index];
  }

// Set the gradient opacity transfer function
void vtkVolumeProperty::SetGradientLinearOpacity(int index,
	vtkPiecewiseFunction *function)
  {
  if (this->GradientLinearOpacity[index] != function)
	{
	if (this->GradientLinearOpacity[index] != NULL)
	  {
	  this->GradientLinearOpacity[index]->UnRegister(this);
	  }
	this->GradientLinearOpacity[index] = function;
	if (this->GradientLinearOpacity[index] != NULL)
	  {
	  this->GradientLinearOpacity[index]->Register(this);
	  }

	this->GradientLinearOpacityMTime[index].Modified();
	this->Modified();
	}
  }

// Set the gaussian gradient opacity transfer function
void vtkVolumeProperty::SetScalarGaussianOpacity(int index,
	vtkGaussianPiecewiseFunction *function)
  {
  if (this->ScalarGaussianOpacity[index] != function)
	{
	if (this->ScalarGaussianOpacity[index] != NULL)
	  {
	  this->ScalarGaussianOpacity[index]->UnRegister(this);
	  }
	this->ScalarGaussianOpacity[index] = function;
	if (this->ScalarGaussianOpacity[index] != NULL)
	  {
	  this->ScalarGaussianOpacity[index]->Register(this);
	  }

	this->ScalarGaussianOpacityMTime[index].Modified();
	this->Modified();
	}
  }

// Set the gaussian gradient opacity transfer function
void vtkVolumeProperty::SetGradientGaussianOpacity(int index,
	vtkGaussianPiecewiseFunction *function)
  {
  if (this->GradientGaussianOpacity[index] != function)
	{
	if (this->GradientGaussianOpacity[index] != NULL)
	  {
	  this->GradientGaussianOpacity[index]->UnRegister(this);
	  }
	this->GradientGaussianOpacity[index] = function;
	if (this->GradientGaussianOpacity[index] != NULL)
	  {
	  this->GradientGaussianOpacity[index]->Register(this);
	  }

	this->GradientGaussianOpacityMTime[index].Modified();
	this->Modified();
	}
  }

void vtkVolumeProperty::SetTwoDTransferFunction(int index,
	vtkTwoDTransferFunction *function)
  {
  if (this->TwoDTransferFunction[index] != function)
	{
	if (this->TwoDTransferFunction[index] != NULL)
	  {
	  this->TwoDTransferFunction[index]->UnRegister(this);
	  }
	this->TwoDTransferFunction[index] = function;
	if (this->TwoDTransferFunction[index] != NULL)
	  {
	  this->TwoDTransferFunction[index]->Register(this);
	  }

	this->TwoDTransferFunctionMTime[index].Modified();
	this->Modified();
	}
  }

void vtkVolumeProperty::CreateDefaultGradientLinearOpacity(int index)
  {
  if (this->DefaultGradientLinearOpacity[index] == NULL)
	{
	this->DefaultGradientLinearOpacity[index] = vtkPiecewiseFunction::New();
	this->DefaultGradientLinearOpacity[index]->Register(this);
	this->DefaultGradientLinearOpacity[index]->Delete();
	}

  this->DefaultGradientLinearOpacity[index]->RemoveAllPoints();
  this->DefaultGradientLinearOpacity[index]->AddPoint(0, 1.0);
  this->DefaultGradientLinearOpacity[index]->AddPoint(255, 1.0);
  }



void vtkVolumeProperty::CreateDefaultGradientGaussianOpacity(int index)
  {
  if (this->DefaultGradientGaussianOpacity[index] == NULL)
	{
	this->DefaultGradientGaussianOpacity[index] = vtkGaussianPiecewiseFunction::New();
	this->DefaultGradientGaussianOpacity[index]->Register(this);
	this->DefaultGradientGaussianOpacity[index]->Delete();
	}
  this->DefaultGradientGaussianOpacity[index]->blankGaussian = true;
  this->DefaultGradientGaussianOpacity[index]->RemoveAllPoints();
  this->DefaultGradientGaussianOpacity[index]->AddGaussian(0, 0.5, 0.1, 0, 0);
  this->DefaultGradientGaussianOpacity[index]->AddGaussian(1, 0.5, 0.1, 0, 0);
  }

void vtkVolumeProperty::CreateDefaultTwoDTransferFunction(int index)
  {
  if (this->DefaultTwoDTransferFunction[index] == NULL)
	{
	this->DefaultTwoDTransferFunction[index] = vtkTwoDTransferFunction::New();
	this->DefaultTwoDTransferFunction[index]->Register(this);
	this->DefaultTwoDTransferFunction[index]->Delete();
	}

  this->DefaultTwoDTransferFunction[index]->RemoveAllRegions();
  this->DefaultTwoDTransferFunction[index]->AddRegion(0.5, 0.5, 0.25, 0.25, 0,
	  1);
  }

vtkAbstractPiecewiseFunction *vtkVolumeProperty::GetCurrentScalarOpacity(
	int index)
  {
  if (useScalarGaussian)
	{
	return GetScalarGaussianOpacity(index);
	}
  else
	{
	return GetScalarOpacity(index);
	}
  }

vtkAbstractPiecewiseFunction *vtkVolumeProperty::GetCurrentGradientOpacity(
	int index)
  {
  if (useGradientGaussian)
	{
	return GetGradientGaussianOpacity(index);
	}
  else
	{
	return GetGradientLinearOpacity(index);
	}
  }

vtkPiecewiseFunction *vtkVolumeProperty::GetGradientLinearOpacity(int index)
  {
  if (this->DisableGradientLinearOpacity[index])
	{
	if (this->DefaultGradientLinearOpacity[index] == NULL)
	  {
	  this->CreateDefaultGradientLinearOpacity(index);
	  }
	return this->DefaultGradientLinearOpacity[index];
	}

  return this->GetStoredGradientLinearOpacity(index);
  }

vtkGaussianPiecewiseFunction *vtkVolumeProperty::GetScalarGaussianOpacity(
	int index)
  {
if (this->ScalarGaussianOpacity[index] == NULL)
	{
	this->ScalarGaussianOpacity[index] = vtkGaussianPiecewiseFunction::New();
	this->ScalarGaussianOpacity[index]->Register(this);
	this->ScalarGaussianOpacity[index]->Delete();
	this->ScalarGaussianOpacity[index]->AddGaussian(0.5, 0.5, 0.25,0,0);
	}

  return this->ScalarGaussianOpacity[index];
  }

vtkGaussianPiecewiseFunction *vtkVolumeProperty::GetGradientGaussianOpacity(int index)
  {
  if (this->DisableGradientGaussianOpacity[index])
	{
	if (this->DefaultGradientGaussianOpacity[index] == NULL)
	  {
	  this->CreateDefaultGradientGaussianOpacity(index);
	  }
	return this->DefaultGradientGaussianOpacity[index];
	}

  return this->GetStoredGradientGaussianOpacity(index);
  }

vtkTwoDTransferFunction *vtkVolumeProperty::GetTwoDTransferFunction(int index)
  {
  if (this->DisableTwoDTransferFunction[index])
	{
	if (this->DefaultTwoDTransferFunction[index] == NULL)
	  {
	  this->CreateDefaultTwoDTransferFunction(index);
	  }
	return this->DefaultTwoDTransferFunction[index];
	}

  return this->GetStoredTwoDTransferFunction(index);
  }

// Get the gradient opacity transfer function. Create one if none set.
vtkPiecewiseFunction *vtkVolumeProperty::GetStoredGradientLinearOpacity(int index)
  {
  if (this->GradientLinearOpacity[index] == NULL)
	{
	this->GradientLinearOpacity[index] = vtkPiecewiseFunction::New();
	this->GradientLinearOpacity[index]->Register(this);
	this->GradientLinearOpacity[index]->Delete();
	this->GradientLinearOpacity[index]->AddPoint(0, 1.0);
	this->GradientLinearOpacity[index]->AddPoint(255, 1.0);
	}

  return this->GradientLinearOpacity[index];
  }

vtkGaussianPiecewiseFunction *vtkVolumeProperty::GetStoredScalarGaussianOpacity(
	int index)
  {
  if (this->ScalarGaussianOpacity[index] == NULL)
	{
	this->ScalarGaussianOpacity[index] = vtkGaussianPiecewiseFunction::New();
	this->ScalarGaussianOpacity[index]->Register(this);
	this->ScalarGaussianOpacity[index]->Delete();
	this->ScalarGaussianOpacity[index]->AddGaussian(0, 1, 0.1, 0, 0);
	this->ScalarGaussianOpacity[index]->AddGaussian(1, 1, 0.1, 0, 0);
	}

  return this->ScalarGaussianOpacity[index];
  }

vtkGaussianPiecewiseFunction *vtkVolumeProperty::GetStoredGradientGaussianOpacity(
	int index)
  {
  if (this->GradientGaussianOpacity[index] == NULL)
	{
	this->GradientGaussianOpacity[index] = vtkGaussianPiecewiseFunction::New();
	this->GradientGaussianOpacity[index]->Register(this);
	this->GradientGaussianOpacity[index]->Delete();
	this->GradientGaussianOpacity[index]->AddGaussian(0, 1, 0.1, 0, 0);
	this->GradientGaussianOpacity[index]->AddGaussian(1, 1, 0.1, 0, 0);
	}

  return this->GradientGaussianOpacity[index];
  }

vtkTwoDTransferFunction *vtkVolumeProperty::GetStoredTwoDTransferFunction(
	int index)
  {
  if (this->TwoDTransferFunction[index] == NULL)
	{
	this->TwoDTransferFunction[index] = vtkTwoDTransferFunction::New();
	this->TwoDTransferFunction[index]->Register(this);
	this->TwoDTransferFunction[index]->Delete();
	this->TwoDTransferFunction[index]->AddRegion(0.5, 0.5, 0.25, 0.25, 0, 1);
	}

  return this->TwoDTransferFunction[index];
  }

void vtkVolumeProperty::SetDisableGradientLinearOpacity(int index, int value)
  {
  if (this->DisableGradientLinearOpacity[index] == value)
	{
	return;
	}

  this->DisableGradientLinearOpacity[index] = value;

  // Make sure the default function is up-to-date (since the user
  // could have modified the default function)

  if (value)
	{
	this->CreateDefaultGradientLinearOpacity(index);
	}

  // Since this Ivar basically "sets" the gradient opacity function to be
  // either a default one or the user-specified one, update the MTime
  // accordingly

  this->GradientLinearOpacityMTime[index].Modified();

  this->Modified();
  }

int vtkVolumeProperty::GetDisableGradientLinearOpacity(int index)
  {
  return this->DisableGradientLinearOpacity[index];
  }



void vtkVolumeProperty::SetDisableGradientGaussianOpacity(int index, int value)
  {
  if (this->DisableGradientGaussianOpacity[index] == value)
	{
	return;
	}

  this->DisableGradientGaussianOpacity[index] = value;

  // Make sure the default function is up-to-date (since the user
  // could have modified the default function)

  if (value)
	{
	this->CreateDefaultGradientGaussianOpacity(index);
	}

  // Since this Ivar basically "sets" the Gaussian opacity function to be
  // either a default one or the user-specified one, update the MTime
  // accordingly

  this->GradientGaussianOpacityMTime[index].Modified();

  this->Modified();
  }



int vtkVolumeProperty::GetDisableGradientGaussianOpacity(int index)
  {
  return this->DisableGradientGaussianOpacity[index];
  }

void vtkVolumeProperty::SetDisableTwoDTransferFunction(int index, int value)
  {
  if (this->DisableTwoDTransferFunction[index] == value)
	{
	return;
	}

  this->DisableTwoDTransferFunction[index] = value;

  // Make sure the default function is up-to-date (since the user
  // could have modified the default function)

  if (value)
	{
	this->CreateDefaultTwoDTransferFunction(index);
	}

  // Since this Ivar basically "sets" the Gaussian opacity function to be
  // either a default one or the user-specified one, update the MTime
  // accordingly

  this->TwoDTransferFunctionMTime[index].Modified();

  this->Modified();
  }

int vtkVolumeProperty::GetDisableTwoDTransferFunction(int index)
  {
  return this->DisableTwoDTransferFunction[index];
  }

void vtkVolumeProperty::SetComponentWeight(int index, double value)
  {
  if (index < 0 || index >= VTK_MAX_VRCOMP)
	{
	vtkErrorMacro("Invalid index");
	return;
	}

  if (this->ComponentWeight[index] == value)
	{
	return;
	}

  this->ComponentWeight[index] = value;
  this->Modified();
  }

double vtkVolumeProperty::GetComponentWeight(int index)
  {
  if (index < 0 || index >= VTK_MAX_VRCOMP)
	{
	vtkErrorMacro("Invalid index");
	return 0.0;
	}

  return this->ComponentWeight[index];
  }

void vtkVolumeProperty::SetShade(int index, int value)
  {
  if (value != 0 && value != 1)
	{
	vtkErrorMacro("SetShade accepts values 0 or 1");
	return;
	}

  if (this->Shade[index] != value)
	{
	this->Shade[index] = value;
	this->Modified();
	}
  }

void vtkVolumeProperty::ShadeOn(int index)
  {
  this->SetShade(index, 1);
  }

void vtkVolumeProperty::ShadeOff(int index)
  {
  this->SetShade(index, 0);
  }

int vtkVolumeProperty::GetShade(int index)
  {
  return this->Shade[index];
  }

void vtkVolumeProperty::SetAmbient(int index, double value)
  {
  if (this->Ambient[index] != value)
	{
	this->Ambient[index] = value;
	this->Modified();
	}
  }

double vtkVolumeProperty::GetAmbient(int index)
  {
  return this->Ambient[index];
  }

void vtkVolumeProperty::SetDiffuse(int index, double value)
  {
  if (this->Diffuse[index] != value)
	{
	this->Diffuse[index] = value;
	this->Modified();
	}
  }

double vtkVolumeProperty::GetDiffuse(int index)
  {
  return this->Diffuse[index];
  }

void vtkVolumeProperty::SetSpecular(int index, double value)
  {
  if (this->Specular[index] != value)
	{
	this->Specular[index] = value;
	this->Modified();
	}
  }

double vtkVolumeProperty::GetSpecular(int index)
  {
  return this->Specular[index];
  }

void vtkVolumeProperty::SetSpecularPower(int index, double value)
  {
  if (this->SpecularPower[index] != value)
	{
	this->SpecularPower[index] = value;
	this->Modified();
	}
  }

double vtkVolumeProperty::GetSpecularPower(int index)
  {
  return this->SpecularPower[index];
  }

vtkTimeStamp vtkVolumeProperty::GetScalarOpacityMTime(int index)
  {
  return this->ScalarOpacityMTime[index];
  }

vtkTimeStamp vtkVolumeProperty::GetGradientLinearOpacityMTime(int index)
  {
  return this->GradientLinearOpacityMTime[index];
  }

vtkTimeStamp vtkVolumeProperty::GetRGBTransferFunctionMTime(int index)
  {
  return this->RGBTransferFunctionMTime[index];
  }

vtkTimeStamp vtkVolumeProperty::GetGrayTransferFunctionMTime(int index)
  {
  return this->GrayTransferFunctionMTime[index];
  }

// Print the state of the volume property.
void vtkVolumeProperty::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Independent Components: "
	  << (this->IndependentComponents ? "On\n" : "Off\n");

  os << indent << "Interpolation Type: " << this->GetInterpolationTypeAsString()
	  << "\n";

  for (int i = 0; i < VTK_MAX_VRCOMP; i++)
	{
	os << indent << "Properties for material " << i << endl;

	os << indent << "Color Channels: " << this->ColorChannels[i] << "\n";

	if (this->ColorChannels[i] == 1)
	  {
	  os << indent << "Gray Color Transfer Function: "
		  << this->GrayTransferFunction[i] << "\n";
	  }
	else if (this->ColorChannels[i] == 3)
	  {
	  os << indent << "RGB Color Transfer Function: "
		  << this->RGBTransferFunction[i] << "\n";
	  }

	os << indent << "Scalar Opacity Transfer Function: "
		<< this->ScalarOpacity[i] << "\n";

	os << indent << "Gradient Opacity Transfer Function: "
		<< this->GradientLinearOpacity[i] << "\n";

	os << indent << "DisableGradientOpacity: "
		<< (this->DisableGradientLinearOpacity[i] ? "On" : "Off") << "\n";

	os << indent << "ComponentWeight: " << this->ComponentWeight[i] << "\n";

	os << indent << "Shade: " << this->Shade[i] << "\n";
	os << indent << indent << "Ambient: " << this->Ambient[i] << "\n";
	os << indent << indent << "Diffuse: " << this->Diffuse[i] << "\n";
	os << indent << indent << "Specular: " << this->Specular[i] << "\n";
	os << indent << indent << "SpecularPower: " << this->SpecularPower[i]
		<< "\n";
	}

  // These variables should not be printed to the user:
  // this->GradientOpacityMTime
  // this->GrayTransferFunctionMTime
  // this->RGBTransferFunctionMTime
  // this->ScalarOpacityMTime

  }

