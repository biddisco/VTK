/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDiscretizableColorTransferFunctionCollection.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkDiscretizableColorTransferFunctionCollection - a combination of vtkColorTransferFunction and
// vtkLookupTable.
// .SECTION Description
// This is a cross between a vtkColorTransferFunction and a vtkLookupTable
// selectively combining the functionality of both. This class is a
// vtkColorTransferFunction allowing users to specify the RGB control points
// that control the color transfer function. At the same time, by setting
// \a Discretize to 1 (true), one can force the transfer function to only have
// \a NumberOfValues discrete colors.
//
// When \a IndexedLookup is true, this class behaves differently. The annotated
// valyes are considered to the be only valid values for which entries in the
// color table should be returned. The colors for annotated values are those
// specified using \a AddIndexedColors. Typically, there must be atleast as many
// indexed colors specified as the annotations. For backwards compatibility, if
// no indexed-colors are specified, the colors in the lookup \a Table are assigned
// to annotated values by taking the modulus of their index in the list
// of annotations. If a scalar value is not present in \a AnnotatedValues,
// then \a NanColor will be used.
//
// NOTE: One must call Build() after making any changes to the points
// in the ColorTransferFunction to ensure that the discrete and non-discrete
// versions match up.


#ifndef __vtkDiscretizableColorTransferFunctionCollection_h
#define __vtkDiscretizableColorTransferFunctionCollection_h

#include "vtkRenderingCoreModule.h" // For export macro
#include "vtkColorTransferFunction.h"
#include "vtkSmartPointer.h" // for vtkSmartPointer
#include "vtkDiscretizableColorTransferFunction.h"


class vtkLookupTable;
class vtkColorTransferFunction;
class vtkPiecewiseFunction;
class vtkGaussianPiecewiseFunction;

class VTKRENDERINGCORE_EXPORT vtkDiscretizableColorTransferFunctionCollection : public vtkDiscretizableColorTransferFunction
{
public:
  static vtkDiscretizableColorTransferFunctionCollection* New();
  vtkTypeMacro(vtkDiscretizableColorTransferFunctionCollection, vtkDiscretizableColorTransferFunction);
  void PrintSelf(ostream& os, vtkIndent indent);
  // Description:
  // Set/get the opacity function to use.
  virtual void SetGradientOpacityFunction(vtkPiecewiseFunction *function);
  virtual vtkPiecewiseFunction* GetGradientOpacityFunction() const;
  virtual void SetGaussianOpacityFunction(vtkGaussianPiecewiseFunction *function);
  virtual vtkGaussianPiecewiseFunction* GetGaussianOpacityFunction() const;

protected:
  vtkDiscretizableColorTransferFunctionCollection();
  ~vtkDiscretizableColorTransferFunctionCollection();


vtkSmartPointer<vtkPiecewiseFunction> GradientOpacityFunction;
vtkSmartPointer<vtkGaussianPiecewiseFunction> GaussianOpacityFunction;
  unsigned long GradientOpacityFunctionObserverId;

private:
  vtkDiscretizableColorTransferFunctionCollection(const vtkDiscretizableColorTransferFunctionCollection&); // Not implemented.
  void operator=(const vtkDiscretizableColorTransferFunctionCollection&); // Not implemented.


};

#endif
