/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPiecewiseFunction.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkPiecewiseFunction - Defines a 1D piecewise function.
//
// .SECTION Description
// Defines a piecewise function mapping. This mapping allows the addition
// of control points, and allows the user to control the function between
// the control points. A piecewise hermite curve is used between control
// points, based on the sharpness and midpoint parameters. A sharpness of
// 0 yields a piecewise linear function and a sharpness of 1 yields a
// piecewise constant function. The midpoint is the normalized distance
// between control points at which the curve reaches the median Y value.
// The midpoint and sharpness values specified when adding a node are used
// to control the transition to the next node (the last node's values are
// ignored) Outside the range of nodes, the values are 0 if Clamping is off,
// or the nearest node point if Clamping is on. Using the legacy methods for
// adding points  (which do not have Sharpness and Midpoint parameters)
// will default to Midpoint = 0.5 (halfway between the control points) and
// Sharpness = 0.0 (linear).

#ifndef __vtkAbstractPiecewiseFunction_h
#define __vtkAbstractPiecewiseFunction_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkDataObject.h"


class VTKCOMMONDATAMODEL_EXPORT vtkAbstractPiecewiseFunction : public vtkDataObject
{
public:

 vtkTypeMacro(vtkAbstractPiecewiseFunction,vtkDataObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void DeepCopy( vtkDataObject *f ) = 0;
  virtual void ShallowCopy( vtkDataObject *f ) = 0;

  // Description:
  // Return what type of dataset this is.
  int GetDataObjectType() {return VTK_PIECEWISE_FUNCTION;};

  // Description:
  // Get the number of points used to specify the function
  virtual int  GetSize() = 0;



  // Description:
  // Removes all points from the function.
  virtual void RemoveAllPoints() = 0;


  // Description:
  // Returns the value of the function at the specified location using
  // the specified interpolation.
  virtual double GetValue( double x ) = 0;


  // Description:
  // Returns a pointer to the data stored in the table.
  // Fills from a pointer to data stored in a similar table. These are
  // legacy methods which will be maintained for compatibility - however,
  // note that the vtkPiecewiseFunction no longer stores the nodes
  // in a double array internally.
  virtual double *GetDataPointer() = 0;


  // Description:
  // Returns the min and max node locations of the function.
  vtkGetVector2Macro( Range, double );

  // Description:
  // Remove all points out of the new range, and make sure there is a point
  // at each end of that range.
  // Return 1 on success, 0 otherwise.
  virtual int AdjustRange(double range[2]) = 0;

  // Description:
  // Fills in an array of function values evaluated at regular intervals.
  // Parameter "stride" is used to step through the output "table".
  virtual void GetTable( double x1, double x2, int size, float *table, int stride=1 ) = 0;
  virtual void GetTable( double x1, double x2, int size, double *table, int stride=1 ) = 0;

  // Description:
  // Constructs a piecewise function from a table.  Function range is
  // is set to [x1, x2], function size is set to size, and function points
  // are regularly spaced between x1 and x2.  Parameter "stride" is
  // is step through the input table.


  // Description:
  // When zero range clamping is Off, GetValue() returns 0.0 when a
  // value is requested outside of the points specified.
  // When zero range clamping is On, GetValue() returns the value at
  // the value at the lowest point for a request below all points
  // specified and returns the value at the highest point for a request
  // above all points specified. On is the default.
  vtkSetMacro( Clamping, int );
  vtkGetMacro( Clamping, int );
  vtkBooleanMacro( Clamping, int );

  // Description:
  // Return the type of function:
  // Function Types:
  //    0 : Constant        (No change in slope between end points)
  //    1 : NonDecreasing   (Always increasing or zero slope)
  //    2 : NonIncreasing   (Always decreasing or zero slope)
  //    3 : Varied          (Contains both decreasing and increasing slopes)
  virtual const char  *GetType() = 0;

  // Description:
  // Returns the first point location which precedes a non-zero segment of the
  // function. Note that the value at this point may be zero.
  virtual double GetFirstNonZeroValue() = 0;

  // Description:
  // Clears out the current function. A newly created vtkPiecewiseFunction
  // is alreay initialized, so there is no need to call this method which
  // in turn simply calls RemoveAllPoints()
  virtual void Initialize() = 0;

  //BTX
  // Description:
  // Retrieve an instance of this class from an information object.
  //virtual static vtkAbstractPiecewiseFunction* GetData(vtkInformation* info) = 0;
  // virtual static vtkAbstractPiecewiseFunction* GetData(vtkInformationVector* v, int i=0) = 0;
  //ETX

  /*
  // Description:
  // Toggle whether to allow duplicate scalar values in the piecewise
  // function (off by default).
  vtkSetMacro(AllowDuplicateScalars, int);
  vtkGetMacro(AllowDuplicateScalars, int);
  vtkBooleanMacro(AllowDuplicateScalars, int);
*/
protected:
  vtkAbstractPiecewiseFunction();
  ~vtkAbstractPiecewiseFunction();



  // Determines the function value outside of defined points
  // Zero = always return 0.0 outside of defined points
  // One  = clamp to the lowest value below defined points and
  //        highest value above defined points
  int   Clamping;

  // Array of points ((X,Y) pairs)
  double *Function;

  // Min and max range of function point locations
  double Range[2];

  // Internal method to sort the vector and update the
  // Range whenever a node is added, edited or removed.
  // It always calls Modified().
  virtual void SortAndUpdateRange() = 0;
  // Returns true if the range has been updated and Modified() has been called
  virtual bool UpdateRange() = 0;

//  int AllowDuplicateScalars;

private:
  vtkAbstractPiecewiseFunction(const vtkAbstractPiecewiseFunction&);  // Not implemented.
  void operator=(const vtkAbstractPiecewiseFunction&);  // Not implemented.
};

#endif


