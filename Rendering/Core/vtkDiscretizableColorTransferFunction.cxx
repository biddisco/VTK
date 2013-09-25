/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDiscretizableColorTransferFunction.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkDiscretizableColorTransferFunction.h"

#include "vtkCommand.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPiecewiseFunction.h"
#include "vtkTuple.h"

#include <vector>

class vtkDiscretizableColorTransferFunction::vtkInternals
{
public:
  std::vector<vtkTuple<double, 3> > IndexedColors;
};

vtkStandardNewMacro(vtkDiscretizableColorTransferFunction);
vtkCxxSetObjectMacro(vtkDiscretizableColorTransferFunction,
  ScalarOpacityFunction, vtkPiecewiseFunction);
vtkCxxSetObjectMacro(vtkDiscretizableColorTransferFunction,
  GradientOpacityFunction, vtkPiecewiseFunction);
//-----------------------------------------------------------------------------
vtkDiscretizableColorTransferFunction::vtkDiscretizableColorTransferFunction()
  : Internals(new vtkInternals())
{
  this->LookupTable = vtkLookupTable::New();

  this->Discretize = 0;
  this->NumberOfValues = 256;

  this->Data = 0;
  this->UseLogScale = 0;

  this->ScalarOpacityFunction = 0;
  this->ScalarOpacityFunctionObserverId = 0;
  this->EnableOpacityMapping = false;

  this->GradientOpacityFunction = vtkPiecewiseFunction::New();
  this->GradientOpacityFunctionObserverId = 0;
  this->GradientOpacityFunction->AddPoint(37.0, 0.0);
  this->GradientOpacityFunction->AddPoint(150.0, 1.0);

}

//-----------------------------------------------------------------------------
vtkDiscretizableColorTransferFunction::~vtkDiscretizableColorTransferFunction()
{
  // this removes any observer we may have setup for the
  // ScalarOpacityFunction.
  this->SetScalarOpacityFunction(NULL);
  this->LookupTable->Delete();
  delete [] this->Data;

  delete this->Internals;
  this->Internals = NULL;
}


//-------------my stuff------------------
/*void vtkDiscretizableColorTransferFunction::SetGradientOpacityFunction(vtkPiecewiseFunction *function){
this->GradientOpacityFunction = function;
}*/


//-----------------------------------------------------------------------------
unsigned long vtkDiscretizableColorTransferFunction::GetMTime()
{
  unsigned long mtime = this->Superclass::GetMTime();
  if (this->ScalarOpacityFunction)
    {
    unsigned long somtime = this->ScalarOpacityFunction->GetMTime();
    mtime = somtime > mtime? somtime : mtime;
    }
  if (this->LookupTable)
    {
    unsigned ltmtime = this->LookupTable->GetMTime();
    mtime = ltmtime > mtime? ltmtime : mtime;
    }

  return mtime;
}

//-----------------------------------------------------------------------------
void vtkDiscretizableColorTransferFunction::SetNumberOfIndexedColors(
  unsigned int count)
{
  if (static_cast<unsigned int>(this->Internals->IndexedColors.size()) != count)
    {
    this->Internals->IndexedColors.resize(count);
    this->Modified();
    }
}

//-----------------------------------------------------------------------------
unsigned int vtkDiscretizableColorTransferFunction::GetNumberOfIndexedColors()
{
  return static_cast<unsigned int>(this->Internals->IndexedColors.size());
}

//-----------------------------------------------------------------------------
void vtkDiscretizableColorTransferFunction::SetIndexedColor(
  unsigned int index, double r, double g, double b)
{
  if (static_cast<unsigned int>(this->Internals->IndexedColors.size()) <= index)
    {
    // resize and fill all new colors with the same color as specified.
    size_t old_size = this->Internals->IndexedColors.size();
    size_t new_size = static_cast<size_t>(index+1);
    this->Internals->IndexedColors.resize(new_size);

    for (size_t cc = old_size; cc < new_size; cc++)
      {
      double *data = this->Internals->IndexedColors[cc].GetData();
      data[0] = r;
      data[1] = g;
      data[2] = b;
      }

    this->Modified();
    }
  else if (this->Internals->IndexedColors[index].GetData()[0] != r ||
    this->Internals->IndexedColors[index].GetData()[1] != g ||
    this->Internals->IndexedColors[index].GetData()[2] != b )
    {
    // color has changed, change it.
    double *data = this->Internals->IndexedColors[index].GetData();
    data[0] = r;
    data[1] = g;
    data[2] = b;

    this->Modified();
    }
}

//-----------------------------------------------------------------------------
void vtkDiscretizableColorTransferFunction::GetIndexedColor(vtkIdType i, double rgba[4])
{
  if (this->IndexedLookup || this->Discretize)
    {
    this->LookupTable->GetIndexedColor(i, rgba);
    }
  else
    {
    this->Superclass::GetIndexedColor(i, rgba);
    }
}

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

//-----------------------------------------------------------------------------
int vtkDiscretizableColorTransferFunction::IsOpaque()
{
  return !this->EnableOpacityMapping;
}

//-----------------------------------------------------------------------------
void vtkDiscretizableColorTransferFunction::Build()
{
  this->Superclass::Build();

  if (this->BuildTime > this->GetMTime())
    {
    // no need to rebuild anything.
    return;
    }

  this->LookupTable->SetVectorMode(this->VectorMode);
  this->LookupTable->SetVectorComponent(this->VectorComponent);
  this->LookupTable->SetIndexedLookup(this->IndexedLookup);

  // this  is essential since other the LookupTable doesn't update the
  // annotations map. That's a bug in the implementation of
  // vtkScalarsToColors::SetAnnotations(..,..);
  this->LookupTable->SetAnnotations(NULL, NULL);
  this->LookupTable->SetAnnotations(this->AnnotatedValues, this->Annotations);

  if (this->IndexedLookup)
    {
    if (this->GetNumberOfIndexedColors() > 0)
      {
      // Use the specified indexed-colors.
      vtkIdType count = this->GetNumberOfAnnotatedValues();
      this->LookupTable->SetNumberOfTableValues(count);
      for (size_t cc=0; cc < this->Internals->IndexedColors.size() &&
                        cc < static_cast<size_t>(count); cc++)
        {
        double rgba[4];
        rgba[0] = this->Internals->IndexedColors[cc].GetData()[0];
        rgba[1] = this->Internals->IndexedColors[cc].GetData()[1];
        rgba[2] = this->Internals->IndexedColors[cc].GetData()[2];
        rgba[3] = 1.0;
        this->LookupTable->SetTableValue(static_cast<int>(cc), rgba);
        }
      }
    else
      {
      // old logic for backwards compatibility.
      int nv = this->GetSize();
      this->LookupTable->SetNumberOfTableValues( nv );
      double nodeVal[6];
      for ( int i = 0; i < nv; ++ i )
        {
        this->GetNodeValue( i, nodeVal );
        nodeVal[4] = 1.;
        this->LookupTable->SetTableValue( i, &nodeVal[1] );
        }
      }
    }
  else if (this->Discretize)
    {
    // Do not omit the LookupTable->SetNumberOfTableValues call:
    // WritePointer does not update the NumberOfColors ivar.
    this->LookupTable->SetNumberOfTableValues(this->NumberOfValues);
    unsigned char* lut_ptr = this->LookupTable->WritePointer(0,
      this->NumberOfValues * 3);
    double* table = new double[this->NumberOfValues * 3];
    double range[2];
    this->GetRange(range);
    bool logRangeValid = true;
    if (this->UseLogScale)
      {
      logRangeValid = range[0] > 0.0 || range[1] < 0.0;
      if(!logRangeValid && this->LookupTable->GetScale() == VTK_SCALE_LOG10)
        {
        this->LookupTable->SetScaleToLinear();
        }
      }

    this->LookupTable->SetRange(range);
    if(this->UseLogScale && logRangeValid &&
        this->LookupTable->GetScale() == VTK_SCALE_LINEAR)
      {
      this->LookupTable->SetScaleToLog10();
      }

    this->GetTable(range[0], range[1], this->NumberOfValues, table);
    // Now, convert double to unsigned chars and fill the LUT.
    for (int cc=0; cc < this->NumberOfValues; cc++)
      {
      lut_ptr[4*cc]   = (unsigned char)(255.0*table[3*cc] + 0.5);
      lut_ptr[4*cc+1] = (unsigned char)(255.0*table[3*cc+1] + 0.5);
      lut_ptr[4*cc+2] = (unsigned char)(255.0*table[3*cc+2] + 0.5);
      lut_ptr[4*cc+3] = 255;
      }
    delete [] table;
    }

  this->BuildTime.Modified();
}

//-----------------------------------------------------------------------------
void vtkDiscretizableColorTransferFunction::SetAlpha(double alpha)
{
  this->LookupTable->SetAlpha(alpha);
  this->Superclass::SetAlpha(alpha);
}

//-----------------------------------------------------------------------------
void vtkDiscretizableColorTransferFunction::SetNanColor(
                                                   double r, double g, double b)
{
  this->LookupTable->SetNanColor(r, g, b, 1.0);
  this->Superclass::SetNanColor(r, g, b);
}

//-----------------------------------------------------------------------------
unsigned char* vtkDiscretizableColorTransferFunction::MapValue(double v)
{
  this->Build();
  if (this->Discretize || this->IndexedLookup)
    {
    return this->LookupTable->MapValue(v);
    }

  return this->Superclass::MapValue(v);
}

//-----------------------------------------------------------------------------
void vtkDiscretizableColorTransferFunction::GetColor(double v, double rgb[3])
{
  this->Build();
  if (this->Discretize || this->IndexedLookup)
    {
    this->LookupTable->GetColor(v, rgb);
    }
  else
    {
    this->Superclass::GetColor(v, rgb);
    }
}

//-----------------------------------------------------------------------------
double vtkDiscretizableColorTransferFunction::GetOpacity(double v)
{
  if (
    this->IndexedLookup ||
    !this->EnableOpacityMapping ||
    !this->ScalarOpacityFunction)
    {
    return this->Superclass::GetOpacity(v);
    }
  return this->ScalarOpacityFunction->GetValue(v);
}

//-----------------------------------------------------------------------------
vtkUnsignedCharArray* vtkDiscretizableColorTransferFunction::MapScalars(vtkDataArray *scalars,
  int colorMode, int component)
{
  this->Build();

  bool scalars_are_mapped = !(colorMode == VTK_COLOR_MODE_DEFAULT) &&
                             vtkUnsignedCharArray::SafeDownCast(scalars);

  vtkUnsignedCharArray *colors = (this->Discretize || this->IndexedLookup) ?
    this->LookupTable->MapScalars(scalars, colorMode, component):
    this->Superclass::MapScalars(scalars, colorMode, component);

  // calculate alpha values
  if (colors &&
     colors->GetNumberOfComponents() == 4 &&
     !scalars_are_mapped &&
     !this->IndexedLookup && //  we don't change alpha for IndexedLookup.
     this->EnableOpacityMapping &&
     this->ScalarOpacityFunction.GetPointer())
    {
    for(vtkIdType i = 0; i < scalars->GetNumberOfTuples(); i++)
      {
      double value = scalars->GetTuple1(i);
      double alpha = this->ScalarOpacityFunction->GetValue(value);
      colors->SetValue(4*i+3, static_cast<unsigned char>(alpha * 255.0 + 0.5));
      }
    }

  return colors;
}

//-----------------------------------------------------------------------------
double* vtkDiscretizableColorTransferFunction::GetRGBPoints()
{
  delete [] this->Data;
  this->Data = 0;

  int num_points = this->GetSize();
  if (num_points > 0)
    {
    this->Data = new double[num_points*4];
    for (int cc=0; cc < num_points; cc++)
      {
      double values[6];
      this->GetNodeValue(cc, values);
      this->Data[4*cc] = values[0];
      this->Data[4*cc+1] = values[0];
      this->Data[4*cc+2] = values[1];
      this->Data[4*cc+3] = values[2];
      }
    }
  return this->Data;
}

//----------------------------------------------------------------------------
vtkIdType vtkDiscretizableColorTransferFunction::GetNumberOfAvailableColors()
{
  if(this->Discretize == false)
    {
    return 16777216; // 2^24
    }
  return this->NumberOfValues;
}

//----------------------------------------------------------------------------
vtkPiecewiseFunction* vtkDiscretizableColorTransferFunction::GetScalarOpacityFunction() const
{
  return this->ScalarOpacityFunction;
}

vtkPiecewiseFunction* vtkDiscretizableColorTransferFunction::GetGradientOpacityFunction() const
{
  return this->GradientOpacityFunction;
}

//-----------------------------------------------------------------------------
void vtkDiscretizableColorTransferFunction::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Discretize: " << this->Discretize << endl;
  os << indent << "NumberOfValues: " << this->NumberOfValues << endl;
  os << indent << "UseLogScale: " << this->UseLogScale << endl;
  os << indent << "EnableOpacityMapping: " << this->EnableOpacityMapping << endl;
  os << indent << "ScalarOpacityFunction: " << this->ScalarOpacityFunction << endl;
}
