
#include "vtkTwoDTransferFunction.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include <cassert>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>

vtkStandardNewMacro(vtkTwoDTransferFunction);
//enum TransferFnMode {Uniform, Gaussian, RightHalf, LeftHalf, TopHalf, BottomHalf, Sine, RampRight, RampLeft};


class vtkTwoDTransferFunctionField
{
public:
  double X;
  double Y;
  double Width;
  double Height;
  TransferFnMode Mode;
};

// A comparison method for sorting Fields in increasing order
class vtkTwoDTransferFunctionCompareFields
{
public:
  bool operator () ( const vtkTwoDTransferFunctionField *Field1,
                     const vtkTwoDTransferFunctionField *Field2 )
    {
      return Field1->X < Field2->X;
    }
};

// A find method for finding a particular Field in the function
class vtkTwoDTransferFunctionFindFieldEqual
{
public:
  double X;
  double Y;
  double Height;
  double Width;
  bool operator () ( const vtkTwoDTransferFunctionField *Field )
    {
      return (Field->X == this->X &&
    		  Field->Y == this->Y &&
    		  Field->Height == this->Height &&
    		  Field->Width == this->Width);
    }
};

// A find method for finding Fields inside a specified range
class vtkTwoDTransferFunctionFindFieldInRange
{
public:
  double X1;
  double X2;
  double Y1;
  double Y2;
  bool operator () ( const vtkTwoDTransferFunctionField *Field )
    {
      return ( Field->X >= this->X1 &&
               Field->X <= this->X2 &&
               Field->Y >= this->Y1 &&
               Field->Y <= this->Y2);
    }
};

// A find method for finding Fields outside a specified range
class vtkTwoDTransferFunctionFindFieldOutOfRange
{
public:
  double X1;
  double X2;
  double Y1;
  double Y2;
  bool operator () ( const vtkTwoDTransferFunctionField *Field )
    {
      return ( Field->X < this->X1 ||
               Field->X > this->X2 ||
               Field->Y < this->Y1 ||
			   Field->Y > this->Y2);
    }
};

// The internal structure for containing the STL objects
class vtkTwoDTransferFunctionInternals
{
public:
  std::vector<vtkTwoDTransferFunctionField*> Fields;
  vtkTwoDTransferFunctionCompareFields        CompareFields;
  vtkTwoDTransferFunctionFindFieldEqual       FindFieldEqual;
  vtkTwoDTransferFunctionFindFieldInRange     FindFieldInRange;
  vtkTwoDTransferFunctionFindFieldOutOfRange  FindFieldOutOfRange;
};



vtkTwoDTransferFunction::vtkTwoDTransferFunction()
{

}


vtkTwoDTransferFunction::~vtkTwoDTransferFunction()
{
}


int vtkTwoDTransferFunction::AddField( double x, double y, double width, double height, TransferFnMode mode){
	vtkTwoDTransferFunctionField *field = new vtkTwoDTransferFunctionField;
	field->X =x;
	field->Y = y;
	field->Width = width;
	field->Height = height;
	field->Mode = mode;


	this->Internal->Fields.push_back(field);

	return this->Internal->Fields.size()-1;


}



void vtkTwoDTransferFunction::DeepCopy( vtkDataObject *f ){

}
 void vtkTwoDTransferFunction::ShallowCopy( vtkDataObject *f ){

 }


 void vtkTwoDTransferFunction::GetTable( double x1, double x2, int size, float *table, int stride ){

 }
  void vtkTwoDTransferFunction::GetTable( double x1, double x2, int size, double *table, int stride ){

  }


  // This is a legacy method that is no longer needed
  void vtkTwoDTransferFunction::Initialize()
  {
    this->RemoveAllPoints();
  }


  // Return the number of points which specify this function
  int vtkTwoDTransferFunction::GetSize()
  {
    return static_cast<int>(this->Internal->Fields.size());
  }

  // Return the type of function stored in object:
  // gaussian, only gaussian.
  //
  const char *vtkTwoDTransferFunction::GetType()
  {

        return "Gaussian";

  }

  // Since we no longer store the data in an array, we must
  // copy out of the vector into an array. No modified check -
  // could be added if performance is a problem
  double *vtkTwoDTransferFunction::GetDataPointer()
  {
    return 0;
  }

  // Returns the first point location which starts a non-zero segment of the
  // function. Note that the value at this point may be zero.
  double vtkTwoDTransferFunction::GetFirstNonZeroValue()
  {
  	return 0;
  }

  // For a specified index value, get the node parameters
  int vtkTwoDTransferFunction::GetNodeValue( int index, double val[5] )
  {
    return 1;
  }

  // For a specified index value, get the node parameters
  int vtkTwoDTransferFunction::SetNodeValue( int index, double val[5] )
  {

    return 1;
  }

  // Adds a point to the function. If a duplicate point is inserted
  // then the function value at that location is set to the new value.
  // This is the legacy version that assumes midpoint = 0.5 and
  // sharpness = 0.0
  /*int vtkTwoDTransferFunction::AddGaussian( double x, double y )
  {
    return this->AddGaussian( x, y, 0.5, 0.0 );
  }*/





  //----------------------------------------------------------------------------
  bool vtkTwoDTransferFunction::UpdateRange()
  {

    return true;
  }





  // Removes all points from the function.
  void vtkTwoDTransferFunction::RemoveAllPoints()
  {

  }



  // Return the value of the function at a position
  double vtkTwoDTransferFunction::GetValue( double x )
  {
return 0;
  }

  // Remove all points outside the range, and make sure a point
  // exists at each end of the range. Used as a convenience method
  // for transfer function editors
  int vtkTwoDTransferFunction::AdjustRange(double range[2])
  {
return 0;
  }


  // Given a table of values, build the piecewise function. Legacy method
  // that does not allow for midpoint and sharpness control
  void vtkTwoDTransferFunction::BuildFunctionFromTable( double xStart, double xEnd,
                                                     int size, double* table,
                                                     int stride )
  {
    //TBD
  }

  // Given a pointer to an array of values, build the piecewise function.
  // Legacy method that does not allow for midpoint and sharpness control
  void vtkTwoDTransferFunction::FillFromDataPointer(int nb, double *ptr)
  {

  }

  //----------------------------------------------------------------------------
  vtkTwoDTransferFunction* vtkTwoDTransferFunction::GetData(vtkInformation* info)
  { //TBD don't know if this is valid, so don't rely on it
return 0;
  }

  //----------------------------------------------------------------------------
  vtkTwoDTransferFunction* vtkTwoDTransferFunction::GetData(vtkInformationVector* v,
                                                      int i)
  {//TBD don't know if this is valid, so don't rely on it
    return vtkTwoDTransferFunction::GetData(v->GetInformationObject(i));
  }

  // Print method for tkPiecewiseFunction
  void vtkTwoDTransferFunction::PrintSelf(ostream& os, vtkIndent indent)
  {


  }

  void vtkTwoDTransferFunction::SortAndUpdateRange()
  {

  }





