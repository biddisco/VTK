/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPiecewiseFunction.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkAbstractPiecewiseFunction.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include <vector>
#include <set>
#include <algorithm>
#include <iterator>



// The internal structure for containing the STL objects

vtkAbstractPiecewiseFunction::vtkAbstractPiecewiseFunction(){

}

vtkAbstractPiecewiseFunction::~vtkAbstractPiecewiseFunction(){

}



// This is a legacy method that is no longer needed
void vtkAbstractPiecewiseFunction::Initialize()
{
  this->RemoveAllPoints();
}








// Print method for tkPiecewiseFunction
void vtkAbstractPiecewiseFunction::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  unsigned int i;
  os << indent << "currently not implemented for abstractpiecewisefunction" << endl;
  /*
  os << indent << "Clamping: " << this->Clamping << endl;
  os << indent << "Range: [" << this->Range[0] << ","
     << this->Range[1] << "]" << endl;
  os << indent << "Function Points: " << this->Internal->Nodes.size() << endl;
  for( i = 0; i < this->Internal->Nodes.size(); i++ )
    {
    os << indent << "  " << i << " X: "
       << this->Internal->Nodes[i]->X << " Y: "
       << this->Internal->Nodes[i]->Y << " Sharpness: "
       << this->Internal->Nodes[i]->Sharpness << " Midpoint: "
       << this->Internal->Nodes[i]->Midpoint << endl;
    }
  os << indent << "AllowDuplicateScalars: " << this->AllowDuplicateScalars
     << endl;
     */
}

/*
int  vtkAbstractPiecewiseFunction::GetSize(){
return 0;

}

void vtkAbstractPiecewiseFunction::DeepCopy( vtkDataObject *f ){

}


void vtkAbstractPiecewiseFunction::ShallowCopy( vtkDataObject *f ){

}

void vtkAbstractPiecewiseFunction::RemoveAllPoints(){


}

double vtkAbstractPiecewiseFunction::GetValue( double x ){
	return 0;

}

double * vtkAbstractPiecewiseFunction::GetDataPointer(){
return 0;

}

int vtkAbstractPiecewiseFunction::AdjustRange(double range[2]){
return 0;
}

void vtkAbstractPiecewiseFunction::GetTable( double x1, double x2, int size, float *table, int stride ){

}

void vtkAbstractPiecewiseFunction::GetTable( double x1, double x2, int size, double *table, int stride ){

}

const char * vtkAbstractPiecewiseFunction::GetType(){
return NULL;
}

double vtkAbstractPiecewiseFunction::GetFirstNonZeroValue(){
return 0;
}


void vtkAbstractPiecewiseFunction::SortAndUpdateRange(){

}
  // Returns true if the range has been updated and Modified() has been called
bool vtkAbstractPiecewiseFunction::UpdateRange(){
	  return false;
  }
*/
