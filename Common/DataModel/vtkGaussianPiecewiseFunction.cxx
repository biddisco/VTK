/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGaussianPiecewiseFunction.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkGaussianPiecewiseFunction.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include <vector>
#include <set>
#include <algorithm>
#include <iterator>

vtkStandardNewMacro(vtkGaussianPiecewiseFunction);

class vtkGaussianPiecewiseFunctionNode
    {
      public:
        double x;
        double h;
        double w;
        double bx;
        double by;
      public:
        vtkGaussianPiecewiseFunctionNode(double x_,double h_,double w_,double bx_,double by_) : x(x_),h(h_),w(w_),bx(bx_),by(by_) {};
        vtkGaussianPiecewiseFunctionNode() {};
        ~vtkGaussianPiecewiseFunctionNode() {};
    };

// A comparison method for sorting nodes in increasing order along the x axis
class vtkGaussianPiecewiseFunctionCompareNodes
{
public:
  bool operator () ( const vtkGaussianPiecewiseFunctionNode *node1,
                     const vtkGaussianPiecewiseFunctionNode *node2 )
    {
      return node1->x < node2->x;
    }
};

// A find method for finding a particular node in the function
class vtkGaussianPiecewiseFunctionFindNodeEqual
{
public:
  double x;
  bool operator () ( const vtkGaussianPiecewiseFunctionNode *node )
    {
      return node->x == this->x;
    }
};

// A find method for finding nodes inside a specified range
class vtkGaussianPiecewiseFunctionFindNodeInRange
{
public:
  double X1;
  double X2;
  bool operator () ( const vtkGaussianPiecewiseFunctionNode *node )
    {
      return ( node->x >= this->X1 &&
               node->x <= this->X2 );
    }
};

// A find method for finding nodes outside a specified range
class vtkGaussianPiecewiseFunctionFindNodeOutOfRange
{
public:
  double x1;
  double x2;
  bool operator () ( const vtkGaussianPiecewiseFunctionNode *node )
    {
      return ( node->x < this->x1 ||
               node->x > this->x2 );
    }
};

// The internal structure for containing the STL objects
class vtkGaussianPiecewiseFunctionInternals
{
public:
  std::vector<vtkGaussianPiecewiseFunctionNode*> Nodes;
  vtkGaussianPiecewiseFunctionCompareNodes        CompareNodes;
  vtkGaussianPiecewiseFunctionFindNodeEqual       FindNodeEqual;
  vtkGaussianPiecewiseFunctionFindNodeInRange     FindNodeInRange;
  vtkGaussianPiecewiseFunctionFindNodeOutOfRange  FindNodeOutOfRange;
};

// Construct a new vtkGaussianPiecewiseFunction with default values
vtkGaussianPiecewiseFunction::vtkGaussianPiecewiseFunction()
{
  this->Clamping = 1;
  this->Range[0] = 0;
  this->Range[1] = 0;

  this->Function = NULL;

  this->AllowMultipleGaussiansSamePoint = 0;

  this->Internal = new vtkGaussianPiecewiseFunctionInternals;
}

// Destruct a vtkGaussianPiecewiseFunction
vtkGaussianPiecewiseFunction::~vtkGaussianPiecewiseFunction()
{
  delete [] this->Function;

  for(unsigned int i=0;i<this->Internal->Nodes.size();i++)
    {
    delete this->Internal->Nodes[i];
    }
  this->Internal->Nodes.clear();
  delete this->Internal;
}

void vtkGaussianPiecewiseFunction::DeepCopy( vtkDataObject *o )
{
  vtkGaussianPiecewiseFunction *f = vtkGaussianPiecewiseFunction::SafeDownCast(o);

  if (f != NULL)
    {
    this->Clamping     = f->Clamping;
    int i;
    this->RemoveAllPoints();
    for ( i = 0; i < f->GetSize(); i++ )
      {
      double val[5];
      f->GetNodeValue(i, val);
      this->AddGaussian(val[0], val[1], val[2], val[3], val[4]);
      }
    this->Modified();
    }

  // Do the superclass
  this->Superclass::DeepCopy(o);
}

void vtkGaussianPiecewiseFunction::ShallowCopy( vtkDataObject *o )
{
  vtkGaussianPiecewiseFunction *f = vtkGaussianPiecewiseFunction::SafeDownCast(o);

  if (f != NULL)
    {
    this->Clamping     = f->Clamping;
    int i;
    this->RemoveAllPoints();
    for ( i = 0; i < f->GetSize(); i++ )
      {
      double val[4];
      f->GetNodeValue(i, val);
      this->AddGaussian(val[0], val[1], val[2], val[3], val[4]);
      }
    this->Modified();
    }

  // Do the superclass
  this->vtkDataObject::ShallowCopy(o);
}

// This is a legacy method that is no longer needed
void vtkGaussianPiecewiseFunction::Initialize()
{
  this->RemoveAllPoints();
}


// Return the number of points which specify this function
int vtkGaussianPiecewiseFunction::GetSize()
{
  return static_cast<int>(this->Internal->Nodes.size());
}

// Return the type of function stored in object:
// gaussian, only gaussian.
//
const char *vtkGaussianPiecewiseFunction::GetType()
{

      return "Gaussian";

}

// Since we no longer store the data in an array, we must
// copy out of the vector into an array. No modified check -
// could be added if performance is a problem
double *vtkGaussianPiecewiseFunction::GetDataPointer()
{
  int size = static_cast<int>(this->Internal->Nodes.size());

  delete [] this->Function;
  this->Function = NULL;

  if ( size > 0 )
    {
    this->Function = new double[size*5];
    for ( int i = 0; i < size; i++ )
      {
      this->Function[5*i  ] = this->Internal->Nodes[i]->x;
      this->Function[5*i+1] = this->Internal->Nodes[i]->h;
      this->Function[5*i+2] = this->Internal->Nodes[i]->w;
      this->Function[5*i+3] = this->Internal->Nodes[i]->bx;
      this->Function[5*i+4] = this->Internal->Nodes[i]->by;

      }
    }

  return this->Function;
}

// Returns the first point location which starts a non-zero segment of the
// function. Note that the value at this point may be zero.
double vtkGaussianPiecewiseFunction::GetFirstNonZeroValue()
{
	//TBD, I think this actually works though. needs testing. Please note it doesn't
	//return the first point where a value is above 0, but the first non zero node
  unsigned int   i;
  int   all_zero = 1;
  double x = 0.0;

  // Check if no points specified
  if( this->Internal->Nodes.size() == 0 )
    {
    return 0;
    }

  for( i=0; i < this->Internal->Nodes.size(); i++ )
    {
    if( this->Internal->Nodes[i]->h != 0.0 )
      {
      x = this->Internal->Nodes[i]->x;
      all_zero = 0;
      break;
      }
    }

  // If every specified point has a zero value then return
  // a large value
  if( all_zero )
    {
    x = VTK_DOUBLE_MAX;
    }
  else  // A point was found with a non-zero value
    {
    if( i > 0 )
      // Return the value of the point that precedes this one
      {
      x = this->Internal->Nodes[i]->x;
      }
    else
      // If this is the first point in the function, return its
      // value is clamping is off, otherwise VTK_DOUBLE_MIN if
      // clamping is on.
      {
      if ( this->Clamping )
        {
        x = VTK_DOUBLE_MIN;
        }
      else
        {
        x = this->Internal->Nodes[0]->x;
        }
      }
    }

  return x;
}

// For a specified index value, get the node parameters
int vtkGaussianPiecewiseFunction::GetNodeValue( int index, double val[5] )
{
  int size = static_cast<int>(this->Internal->Nodes.size());

  if ( index < 0 || index >= size )
    {
    vtkErrorMacro("Index out of range!");
    return -1;
    }

  val[0] = this->Internal->Nodes[index]->x;
  val[1] = this->Internal->Nodes[index]->h;
  val[2] = this->Internal->Nodes[index]->w;
  val[3] = this->Internal->Nodes[index]->bx;
  val[4] = this->Internal->Nodes[index]->by;

  return 1;
}

// For a specified index value, get the node parameters
int vtkGaussianPiecewiseFunction::SetNodeValue( int index, double val[5] )
{
  int size = static_cast<int>(this->Internal->Nodes.size());

  if ( index < 0 || index >= size )
    {
    vtkErrorMacro("Index out of range!");
    return -1;
    }

  double oldX = this->Internal->Nodes[index]->x;
  this->Internal->Nodes[index]->x = val[0];
  this->Internal->Nodes[index]->h = val[1];
  this->Internal->Nodes[index]->w = val[2];
  this->Internal->Nodes[index]->bx = val[3];
  this->Internal->Nodes[index]->by = val[4];

  if (oldX != val[0])
    {
    // The point has been moved, the order of points or the range might have
    // been modified.
    this->SortAndUpdateRange();
    // No need to call Modified() here because SortAndUpdateRange() has done it
    // already.
    }
  else
    {
    this->Modified();
    }

  return 1;
}

// Adds a point to the function. If a duplicate point is inserted
// then the function value at that location is set to the new value.
// This is the legacy version that assumes midpoint = 0.5 and
// sharpness = 0.0
/*int vtkGaussianPiecewiseFunction::AddGaussian( double x, double y )
{
  return this->AddGaussian( x, y, 0.5, 0.0 );
}*/

// Adds a point to the function and returns the array index of the point.
int vtkGaussianPiecewiseFunction::AddGaussian( double x_,double h_,double w_,double bx_,double by_)
{
if (x_ > this->Range[1] || x_ < this->Range[0])
	return -1;
//out of bounds / out of range



  // remove any node already at this X location
  if (!this->AllowMultipleGaussiansSamePoint)
    {
    this->RemoveGaussian( x_ );
    }

  // Create the new node
  vtkGaussianPiecewiseFunctionNode *node = new vtkGaussianPiecewiseFunctionNode(x_, h_, w_, bx_, by_);


  // Add it, then sort to get everything in order
  this->Internal->Nodes.push_back(node);
  this->SortAndUpdateRange();

  // Now find this node so we can return the index
  unsigned int i;
  for ( i = 0; i < this->Internal->Nodes.size(); i++ )
    {
    if ( this->Internal->Nodes[i]->x == x_ )
      {
      break;
      }
    }

  int retVal;

  // If we didn't find it, something went horribly wrong so
  // return -1
  if ( i < this->Internal->Nodes.size() )
    {
    retVal = i;
    }
  else
    {
    retVal = -1;
    }

  return retVal;
}

// Sort the vector in increasing order, then fill in
// the Range
void vtkGaussianPiecewiseFunction::SortAndUpdateRange()
{
  std::sort( this->Internal->Nodes.begin(),
                this->Internal->Nodes.end(),
                this->Internal->CompareNodes );
  bool modifiedInvoked = this->UpdateRange(false, this->GetRange());
  //TBD check if correct
  // If range is updated, Modified() has been called, don't call it again.
 /* if (!modifiedInvoked)
    {
    this->Modified();
    }*/
}

//----------------------------------------------------------------------------
bool vtkGaussianPiecewiseFunction::UpdateRange()
{
	//TBD

  double oldRange[2];
  oldRange[0] = this->Range[0];
  oldRange[1] = this->Range[1];

  int size = static_cast<int>(this->Internal->Nodes.size());
  if ( size )
    {
    this->Range[0] = this->Internal->Nodes[0]->x;
    this->Range[1] = this->Internal->Nodes[size-1]->x;
    }
  else
    {
    this->Range[0] = 0;
    this->Range[1] = 0;
    }
  // If the rage is the same, then no need to call Modified()
  if (oldRange[0] == this->Range[0] && oldRange[1] == this->Range[1])
    {
    return false;
    }

  this->Modified();
  return true;
}


bool vtkGaussianPiecewiseFunction::UpdateRange(bool toNodes, double *range)
{
	//TBD
	double oldRange[2];
	if (toNodes){
		return UpdateRange();
	}
	else{

		  oldRange[0] = this->Range[0];
		  oldRange[1] = this->Range[1];
		this->Range[0]=range[0];
		this->Range[1]=range[1];
	}

  if (oldRange[0] == this->Range[0] && oldRange[1] == this->Range[1])
    {
    return false;
    }

  this->Modified();
  return true;
}

// Removes a point from the function. If no point is found then function
// remains the same.
int vtkGaussianPiecewiseFunction::RemoveGaussian( double x )
{
  // First find the node since we need to know its
  // index as our return value
  unsigned int i;
  for ( i = 0; i < this->Internal->Nodes.size(); i++ )
    {
    if ( this->Internal->Nodes[i]->x == x )
      {
      break;
      }
    }

  int retVal;

  // If the node doesn't exist, we return -1
  if ( i < this->Internal->Nodes.size() )
    {
    retVal = i;
    }
  else
    {
    return -1;
    }

  // Now use STL to find it, so that we can remove it
  this->Internal->FindNodeEqual.x = x;

  std::vector<vtkGaussianPiecewiseFunctionNode*>::iterator iter =
    std::find_if(this->Internal->Nodes.begin(),
                    this->Internal->Nodes.end(),
                    this->Internal->FindNodeEqual );

  // Actually delete it
  if ( iter != this->Internal->Nodes.end() )
    {
    delete *iter;
    this->Internal->Nodes.erase(iter);
    // No need to sort here as the order of points hasn't changed.
    bool modifiedInvoked = false;

    if (!modifiedInvoked)
      {
      this->Modified();
      }
    }
  else
     {
     // This should never happen - we already returned if the node
     // didn't exist...
     return -1;
     }


  return retVal;
}

// Removes all points from the function.
void vtkGaussianPiecewiseFunction::RemoveAllPoints()
{
	//doesn't update range, since range is usually independent of nodes.
  for(unsigned int i=0;i<this->Internal->Nodes.size();i++)
    {
    delete this->Internal->Nodes[i];
    }
  this->Internal->Nodes.clear();

}



// Return the value of the function at a position
double vtkGaussianPiecewiseFunction::GetValue( double x )
{
  double table[1];
  this->GetTable( x, x, 1, table );
  return table[0];
}

// Remove all points outside the range, and make sure a point
// exists at each end of the range. Used as a convenience method
// for transfer function editors
int vtkGaussianPiecewiseFunction::AdjustRange(double range[2])
{
	//TBD

  if (!range)
    {
    return 0;
    }

  double *function_range = this->GetRange();

  function_range[0] = range[0];
  function_range[1] = range[1];

  // Make sure we have points at each end of the range



  // Remove all points out-of-range
  int done;

  done = 0;
  while ( !done )
    {
    done = 1;

    this->Internal->FindNodeOutOfRange.x1 = range[0];
    this->Internal->FindNodeOutOfRange.x2 = range[1];

    std::vector<vtkGaussianPiecewiseFunctionNode*>::iterator iter =
      std::find_if(this->Internal->Nodes.begin(),
                      this->Internal->Nodes.end(),
                      this->Internal->FindNodeOutOfRange );

    if ( iter != this->Internal->Nodes.end() )
      {
      delete *iter;
      this->Internal->Nodes.erase(iter);
      this->Modified();
      done = 0;
      }
    }

  this->SortAndUpdateRange();
  return 1;

	return 0;
}

// Returns a table of function values evaluated at regular intervals
void vtkGaussianPiecewiseFunction::GetTable( double xStart, double xEnd,
                                     int size, double* table,
                                     int stride )
{
	//TBD
	//copy from gaussianopacitybar and modify
	for (int i=0; i<size; i++) table[i] = 0;

	double* tableptr = 0;
	for (int p=0; p<this->Internal->Nodes.size(); p++)
	{
		double _pos    = Internal->Nodes[p]->x;
		double _width  = Internal->Nodes[p]->w;
		double _height = Internal->Nodes[p]->h;
		double xbias  = Internal->Nodes[p]->bx;
		double ybias  = Internal->Nodes[p]->by;
		for (int i=0; i<size; i++)
		{
			tableptr = table + i*stride;
			double _x = xStart + (double(i)/double(size-1))*(xEnd-xStart);

			// clamp non-zero values to _pos +/- _width
			if (_x > _pos+_width || _x < _pos-_width)
			{
				table[i] = table[i]>0.0?table[i]:0.0;
				continue;
			}

			// non-zero _width
			if (_width == 0)
				_width = .00001f;

			// translate the original x to a new x based on the xbias
			double x0;
			if (xbias==0 || _x == _pos+xbias)
			{
				x0 = _x;
			}
			else if (_x > _pos+xbias)
			{
				if (_width == xbias)
					x0 = _pos;
				else
					x0 = _pos+(_x-_pos-xbias)*(_width/(_width-xbias));
			}
			else // (_x < _pos+xbias)
			{
				if (-_width == xbias)
					x0 = _pos;
				else
					x0 = _pos-(_x-_pos-xbias)*(_width/(_width+xbias));
			}

			// center around 0 and normalize to -1,1
			double x1 = (x0-_pos)/_width;

			// do a linear interpolation between:
			//    a gaussian and a parabola        if 0<ybias<1
			//    a parabola and a step function   if 1<ybias<2
			double h0a = exp(-(4*x1*x1));
			double h0b = 1. - x1*x1;
			double h0c = 1.;
			double h1;
			if (ybias < 1)
				h1 = ybias*h0b + (1-ybias)*h0a;
			else
				h1 = (2-ybias)*h0b + (ybias-1)*h0c;
			double h2 = _height * h1;

			// perform the MAX over different guassians, not the sum
			table[i] = table[i]>0.0?table[i]:h2;
		}
	}
}

// Copy from double table to float
void vtkGaussianPiecewiseFunction::GetTable( double xStart, double xEnd,
                                     int size, float* table,
                                     int stride )
{
	//tbd, make everything float stuff, also use the above function
  double *tmpTable = new double [size];

  this->GetTable( xStart, xEnd, size, tmpTable, 1 );

  double *tmpPtr = tmpTable;
  float *tPtr = table;

  for ( int i = 0; i < size; i++ )
    {
    *tPtr = static_cast<float>(*tmpPtr);
    tPtr   += stride;
    tmpPtr ++;
    }

  delete[] tmpTable;
}

// Given a table of values, build the piecewise function. Legacy method
// that does not allow for midpoint and sharpness control
void vtkGaussianPiecewiseFunction::BuildFunctionFromTable( double xStart, double xEnd,
                                                   int size, double* table,
                                                   int stride )
{
  //TBD
}

// Given a pointer to an array of values, build the piecewise function.
// Legacy method that does not allow for midpoint and sharpness control
void vtkGaussianPiecewiseFunction::FillFromDataPointer(int nb, double *ptr)
{
	/*
  if (nb <= 0 || !ptr)
    {
    return;
    }

  this->RemoveAllPoints();

  double *inPtr = ptr;

  int i;
  for (i=0; i < nb; i++)
    {
    vtkGaussianPiecewiseFunctionNode *node = new vtkGaussianPiecewiseFunctionNode;
    node->X  = inPtr[0];
    node->Y  = inPtr[1];
    node->Sharpness = 0.0;
    node->Midpoint  = 0.5;

    this->Internal->Nodes.push_back(node);
    inPtr += 2;
    }

  this->SortAndUpdateRange();
  */
	//TBD
}

//----------------------------------------------------------------------------
vtkGaussianPiecewiseFunction* vtkGaussianPiecewiseFunction::GetData(vtkInformation* info)
{
  return
    info? vtkGaussianPiecewiseFunction::SafeDownCast(info->Get(DATA_OBJECT())) : 0;
}

//----------------------------------------------------------------------------
vtkGaussianPiecewiseFunction* vtkGaussianPiecewiseFunction::GetData(vtkInformationVector* v,
                                                    int i)
{
  return vtkGaussianPiecewiseFunction::GetData(v->GetInformationObject(i));
}

// Print method for tkPiecewiseFunction
void vtkGaussianPiecewiseFunction::PrintSelf(ostream& os, vtkIndent indent)
{
	//TBD;
  this->Superclass::PrintSelf(os, indent);

  unsigned int i;

  os << indent << "not done" << endl;

}

