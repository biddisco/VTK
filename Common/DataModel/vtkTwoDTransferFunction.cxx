#define _USE_MATH_DEFINES
#include <cmath>
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

class vtkTwoDTransferFunctionRegion
  {
public:
  void setValues(double values[5], TransferFnMode Mo)
    {
    X = values[0];
    Y = values[1];
    Width = values[2];
    Height = values[3];
    Mode = Mo;
    Maximum = values[4];
    }
  double X;
  double Y;
  double Width;
  double Height;
  TransferFnMode Mode;
  double Maximum;
  };

// A comparison method for sorting Regions in increasing order
class vtkTwoDTransferFunctionCompareRegions
  {
public:
  bool operator ()(const vtkTwoDTransferFunctionRegion *Region1,
      const vtkTwoDTransferFunctionRegion *Region2)
    {
    return Region1->X < Region2->X;
    }
  };

// A find method for finding a particular Region in the function
class vtkTwoDTransferFunctionFindRegionEqual
  {
public:
  double X;
  double Y;
  double Height;
  double Width;
  bool operator ()(const vtkTwoDTransferFunctionRegion *Region)
    {
    return (Region->X == this->X && Region->Y == this->Y
        && Region->Height == this->Height && Region->Width == this->Width);
    }
  };

// A find method for finding Regions inside a specified range
class vtkTwoDTransferFunctionFindRegionInRange
  {
public:
  double X1;
  double X2;
  double Y1;
  double Y2;
  bool operator ()(const vtkTwoDTransferFunctionRegion *Region)
    {
    return (Region->X >= this->X1 && Region->X <= this->X2
        && Region->Y >= this->Y1 && Region->Y <= this->Y2);
    }
  };

// A find method for finding Regions outside a specified range
class vtkTwoDTransferFunctionFindRegionOutOfRange
  {
public:
  double X1;
  double X2;
  double Y1;
  double Y2;
  bool operator ()(const vtkTwoDTransferFunctionRegion *Region)
    {
    return (Region->X < this->X1 || Region->X + Region->Width > this->X2
        || Region->Y < this->Y1 || Region->Y + Region->Height > this->Y2);
    }
  };

// The internal structure for containing the STL objects
class vtkTwoDTransferFunctionInternals
  {
public:
  std::vector<vtkTwoDTransferFunctionRegion*> Regions;
  vtkTwoDTransferFunctionCompareRegions CompareRegions;
  vtkTwoDTransferFunctionFindRegionEqual FindRegionEqual;
  vtkTwoDTransferFunctionFindRegionInRange FindRegionInRange;
  vtkTwoDTransferFunctionFindRegionOutOfRange FindRegionOutOfRange;
  };

vtkTwoDTransferFunction::vtkTwoDTransferFunction()
  {
  this->Clamping = 1;
  this->Range[0] = 0;
  this->Range[1] = 1;
  this->Range[2] = 0;
  this->Range[3] = 1;

  this->Function = NULL;

  this->Internal = new vtkTwoDTransferFunctionInternals;

  this->Initialize();
  }

vtkTwoDTransferFunction::~vtkTwoDTransferFunction()
  {
  }

int vtkTwoDTransferFunction::AddRegion(double x, double y, double width,
    double height, double mode, double max)
  {
  vtkTwoDTransferFunctionRegion *Region = new vtkTwoDTransferFunctionRegion;
  Region->X = x;
  Region->Y = y;
  Region->Width = width;
  Region->Height = height;
  Region->Mode = (TransferFnMode) int(mode + 0.49);
  Region->Maximum = max;

  this->Internal->Regions.push_back(Region);

  return this->Internal->Regions.size() - 1;

  }

int vtkTwoDTransferFunction::AddRegion(double x, double y, double width,
    double height, TransferFnMode mode, double max)
  {
  vtkTwoDTransferFunctionRegion *Region = new vtkTwoDTransferFunctionRegion;
  Region->X = x;
  Region->Y = y;
  Region->Width = width;
  Region->Height = height;
  Region->Mode = mode;
  Region->Maximum = max;

  this->Internal->Regions.push_back(Region);

  return this->Internal->Regions.size() - 1;

  }

double vtkTwoDTransferFunction::getValue(int index, regionvalue value)
  {
  if (!(index < this->Internal->Regions.size()))
    return -1;

  if (value == REGION_X)
    return this->Internal->Regions[index]->X;
  else if (value == REGION_Y)
    return this->Internal->Regions[index]->Y;
  else if (value == REGION_W)
    return this->Internal->Regions[index]->Width;
  else if (value == REGION_H)
    return this->Internal->Regions[index]->Height;
  else if (value == REGION_MAX)
    return this->Internal->Regions[index]->Maximum;
  else
    return -1;

  }

TransferFnMode vtkTwoDTransferFunction::getRegionMode(int index)
  {
  if (index < this->Internal->Regions.size())
    return this->Internal->Regions[index]->Mode;
  }

void vtkTwoDTransferFunction::DeepCopy(vtkDataObject *f)
  {

  }
void vtkTwoDTransferFunction::ShallowCopy(vtkDataObject *f)
  {

  }

void vtkTwoDTransferFunction::GetTable(double x1, double x2, double y1,
    double y2, int sizeX, int sizeY, float *table, int strideX, int strideY)
  {

  double *tmpTable = new double[sizeX * sizeY];

  this->GetTable(x1, x2, y1, y2, sizeX, sizeY, tmpTable, 1, 1);

  for (int i = 0; i < sizeX; i++)
    {
    for (int j = 0; i < sizeY; j++)
      {
      table[i * sizeY * strideX + j * strideY] = static_cast<float>(tmpTable[i
          * sizeY + j]);
      }

    delete[] tmpTable;

    }
  }

void vtkTwoDTransferFunction::GetTable(double x1, double x2, double y1,
    double y2, int sizeX, int sizeY, double *table, int strideX, int strideY)
  {
  //check if 2d widget square intersects with range. If yes, add it to the table
  //??

  int count = this->Internal->Regions.size();

  for (int j = 0; j < sizeX; j++)
    {
    for (int i = 0; i < sizeY; i++)
      {
      table[j * sizeY * strideX + i * strideY] = 0;
      }
    }

  for (int r = 0; r < count; r++)
    {
    if (this->Internal->Regions[r]->Width <= 0
        || this->Internal->Regions[r]->Height <= 0)
      {
      continue; //at best one dimensional. If width or height is negative, something went very wrong
      }

    if (((this->Internal->Regions[r]->X < x1
        && this->Internal->Regions[r]->X + this->Internal->Regions[r]->Width
            < x1) || (this->Internal->Regions[r]->X > x2))
        || ((this->Internal->Regions[r]->Y < y1
            && this->Internal->Regions[r]->Y + this->Internal->Regions[r]->Width
                < y1) || (this->Internal->Regions[r]->Y > y2)))
      {
      continue; //not inside the table range
      }

    //find the starting and end positions on the table
    int startX, startY, endX, endY;
    if (x1 > this->Internal->Regions[r]->X)
      {
      startX = 0;
      }
    else
      {
      startX = std::max(
          int((this->Internal->Regions[r]->X - x1) / (x2 - x1) * double(sizeX)),
          0); //max in case of rounding or numerical
      }
    if (x2 < this->Internal->Regions[r]->X + this->Internal->Regions[r]->Width)
      {
      endX = sizeX;
      }
    else
      {
      endX = std::min(
          int(
              (this->Internal->Regions[r]->X + this->Internal->Regions[r]->Width
                  - x1) / (x2 - x1) * double(sizeX) + 0.5), sizeX);
      //min in case of rounding or numerical
      //the +0.5 is so we can use < instead of <= later
      }

    if (y1 > this->Internal->Regions[r]->Y)
      {
      startY = 0;
      }
    else
      {
      startY = std::max(
          int((this->Internal->Regions[r]->Y - y1) / (y2 - y1) * double(sizeY)),
          0);
      }

    if (y2 < this->Internal->Regions[r]->Y + this->Internal->Regions[r]->Height)
      {
      endY = sizeY;
      }
    else
      {
      endY = std::min(
          int(
              (this->Internal->Regions[r]->Y + this->Internal->Regions[r]->Height
                  - y1) / (y2 - y1) * double(sizeY) + 0.5), sizeY); //min in case of rounding or numerical
      }

    double xx, yy, intensity, cintensity;

    double wx = this->Internal->Regions[r]->Width;
    double wy = this->Internal->Regions[r]->Height;
    double cx = wx / 2.0;
    double sx = wx * wx;
    double cy = wy / 2.0;
    double sy = wy * wy;
    double dx = (x2-x1)/double(sizeX);
    double dy = (y2-y1)/double(sizeY);
    double bx = 0;

    for (int i = startX; i < endX; i += 1, bx+= dx)
      {
      double by = 0;
      for (int j = startY; j < endY; j += 1, by+=dy)
        {

        switch (this->Internal->Regions[r]->Mode)
          {
        case Uniform:
          intensity = 1.0;
          break;
        case Gaussian:
          xx = (bx - cx) * (bx - cx) / sx;
          yy = (by - cy) * (by - cy) / sy;
          intensity = exp((-8.0) * (xx + yy));
          break;
        case Sine:
          intensity = 0.5 * (1.0 + cos(7.0 * M_PI * (bx - cx) / cx));
          break;
        case RightHalf:
          xx = (bx - 2 * cx) * (bx - 2 * cx) / sx;
          intensity = exp(-4.0 * xx);
          break;
        case LeftHalf:
          xx = (bx) * (bx) / sx;
          intensity = exp(-4.0 * xx);
          break;
        case TopHalf:
          yy = (by - 2 * cy) * (by - 2 * cy) / sy;
          intensity = exp(-4.0 * yy);
          break;
        case BottomHalf:
          yy = (by) * (by) / sy;
          intensity = exp(-4.0 * yy);
          break;
        case RampRight:
          intensity = 1.0 + (bx - wx) / wx;
          break;
        case RampLeft:
          intensity = (wx - bx) / wx;
          break;
          }
        intensity = this->Internal->Regions[r]->Maximum * intensity;
        cintensity = table[i * sizeY * strideX + j * strideY];
        table[i * sizeY * strideX + j * strideY] =
            intensity > cintensity ? intensity : cintensity;
        }

      }

    }
  }

void vtkTwoDTransferFunction::GetOneDScalarTable(double x1, double x2,
    int sizeX, float *table, int strideX)
  {

  /*double *tmpTable = new double[sizeX];

    this->GetOneDScalarTable(x1, x2, sizeX, tmpTable, 1);

    for (int i = 0; i < sizeX; i++)
      {
        table[i*strideX] = static_cast<float>(tmpTable[i]);
      }
    delete[] tmpTable;*/
  for (int i = 0; i< sizeX; i++)
      {
      table[i] = 1;
      }
  }

void vtkTwoDTransferFunction::GetOneDScalarTable(double x1, double x2,
    int sizeX, double *table, int strideX)
  {
  double* tableptr;
  for (int i = 0; i < sizeX; i++)
    {
    tableptr = table + i * strideX;
    *tableptr = 0;
    }

  int count = this->Internal->Regions.size();

  for (int r = 0; r < count; r++)
    {
    if (this->Internal->Regions[r]->Width <= 0
        || this->Internal->Regions[r]->Height <= 0)
      {
      continue; //at best one dimensional. If width or height is negative, something went very wrong
      }

    if (((this->Internal->Regions[r]->X < x1
        && this->Internal->Regions[r]->X + this->Internal->Regions[r]->Width
            < x1) || (this->Internal->Regions[r]->X > x2)))
      {
      continue; //not inside the table range
      }

    //find the starting and end positions on the table
    int startX, startY, endX, endY;
    if (x1 > this->Internal->Regions[r]->X)
      {
      startX = 0;
      }
    else
      {
      startX = std::max(
          int((this->Internal->Regions[r]->X - x1) / (x2 - x1) * double(sizeX)),
          0); //max in case of rounding or numerical
      }
    if (x2 < this->Internal->Regions[r]->X + this->Internal->Regions[r]->Width)
      {
      endX = sizeX;
      }
    else
      {
      endX = std::min(
          int(
              (this->Internal->Regions[r]->X + this->Internal->Regions[r]->Width
                  - x1) / (x2 - x1) * double(sizeX) + 0.5), sizeX);
      //min in case of rounding or numerical
      //the +0.5 is so we can use < instead of <= later
      }

    double xx, intensity, cintensity;

    double wx = this->Internal->Regions[r]->Width;
    double wy = this->Internal->Regions[r]->Height;
    double cx = wx / 2.0;
    double sx = wx * wx;
    double cy = wy / 2.0;
    double sy = wy * wy;

    for (int i = startX; i < endX; i += strideX)
      {

      switch (this->Internal->Regions[r]->Mode)
        {
      case Uniform:
        intensity = 1.0;
        break;
      case Gaussian:
        xx = (i - cx) * (i - cx) / sx;
        intensity = exp((-8.0) * (xx));
        break;
      case Sine:
        intensity = 0.5 * (1.0 + cos(7.0 * M_PI * (i - cx) / cx));
        break;
      case RightHalf:
        xx = (i - 2 * cx) * (i - 2 * cx) / sx;
        intensity = exp(-4.0 * xx);
        break;
      case LeftHalf:
        xx = (i) * (i) / sx;
        intensity = exp(-4.0 * xx);
        break;
      case TopHalf:
        intensity = 1.0;
        break;
      case BottomHalf:
        intensity = 1.0;
        break;
      case RampRight:
        intensity = 1.0 + (i - wx) / wx;
        break;
      case RampLeft:
        intensity = (wx - i) / wx;
        break;
        }
      intensity = this->Internal->Regions[r]->Maximum * intensity;
      cintensity = table[strideX * i];
      table[strideX * i] = cintensity > intensity ? cintensity : intensity;

      }

    }
  }
void vtkTwoDTransferFunction::GetOneDGradientTable(double x1, double x2,
    int sizeX, float *table, int strideX)
  {
  for (int i = 0; i< sizeX; i++)
    {
    table[i] = 1;
    }

  }
void vtkTwoDTransferFunction::GetOneDGradientTable(double x1, double x2,
    int sizeX, double *table, int strideX)
  {
  //don't let the variable names confuse you, this is all gradient (y axis)
  double* tableptr;
  for (int i = 0; i < sizeX; i++)
    {
    tableptr = table + i * strideX;
    *tableptr = 0;
    }

  int count = this->Internal->Regions.size();

  for (int r = 0; r < count; r++)
    {
    if (this->Internal->Regions[r]->Width <= 0
        || this->Internal->Regions[r]->Height <= 0)
      {
      continue; //at best one dimensional. If width or height is negative, something went very wrong
      }

    if (((this->Internal->Regions[r]->Y < x1
        && this->Internal->Regions[r]->Y + this->Internal->Regions[r]->Height
            < x1) || (this->Internal->Regions[r]->Y > x2)))
      {
      continue; //not inside the table range
      }

    //find the starting and end positions on the table
    int startX, startY, endX, endY;
    if (x1 > this->Internal->Regions[r]->Y)
      {
      startX = 0;
      }
    else
      {
      startX = std::max(
          int((this->Internal->Regions[r]->Y - x1) / (x2 - x1) * double(sizeX)),
          0); //max in case of rounding or numerical
      }
    if (x2 < this->Internal->Regions[r]->Y + this->Internal->Regions[r]->Height)
      {
      endX = sizeX;
      }
    else
      {
      endX = std::min(
          int(
              (this->Internal->Regions[r]->Y + this->Internal->Regions[r]->Height
                  - x1) / (x2 - x1) * double(sizeX) + 0.5), sizeX);
      //min in case of rounding or numerical
      //the +0.5 is so we can use < instead of <= later
      }

    double yy, intensity, cintensity;


    double wy = this->Internal->Regions[r]->Height;
    double cy = wy / 2.0;
    double sy = wy * wy;

    for (int i = startX; i < endX; i += strideX)
      {

      switch (this->Internal->Regions[r]->Mode)
        {
      case Uniform:
        intensity = 1.0;
        break;
      case Gaussian:
        yy = (i - cy) * (i - cy) / sy;
        intensity = exp((-8.0) * (yy));
        break;
      case Sine:
        intensity = 1;
        break;
      case RightHalf:;
        intensity = 1;
        break;
      case LeftHalf:
        intensity = 1;
        break;
      case TopHalf:
        yy = (i) * (i) / sy;
        intensity = exp(-4.0 * yy);
        break;
      case BottomHalf:
        yy = (i - 2 * cy) * (i - 2 * cy) / sy;
        intensity = exp(-4.0 * yy);
        break;
      case RampRight:
        intensity = 1;
        break;
      case RampLeft:
        intensity = 1;
        break;
        }
      intensity = this->Internal->Regions[r]->Maximum * intensity;
      cintensity = table[strideX * i];
      table[strideX * i] = cintensity > intensity ? cintensity : intensity;

      }

    }
  }

double vtkTwoDTransferFunction::getXRange()
  {
  return Range[1] - Range[0];
  }

double vtkTwoDTransferFunction::getYRange()
  {
  return Range[3] - Range[2];
  }

// This is a legacy method that is no longer needed
void vtkTwoDTransferFunction::Initialize()
  {
  this->RemoveAllRegions();
  }

// Return the number of points which specify this function
int vtkTwoDTransferFunction::GetSize()
  {
  return static_cast<int>(this->Internal->Regions.size());
  }

// Return the type of function stored in object:
// gaussian, only gaussian.
//
const char *vtkTwoDTransferFunction::GetType()
  {

  return "TwoDTransferFunction";

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

// Adds a point to the function. If a duplicate point is inserted
// then the function value at that location is set to the new value.
// This is the legacy version that assumes midpoint = 0.5 and
// sharpness = 0.0
/*int vtkTwoDTransferFunction::AddGaussian( double x, double y )
 {
 return this->AddGaussian( x, y, 0.5, 0.0 );
 }*/

void vtkTwoDTransferFunction::scaleAndShift(double oldXRange[2],
    double oldYRange[2], double newXRange[2], double newYRange[2])
  {

  double ax = oldXRange[0];
  double bx = oldXRange[1];
  double cx = newXRange[0];
  double dx = newXRange[1];

  double ay = oldYRange[0];
  double by = oldYRange[1];
  double cy = newYRange[0];
  double dy = newYRange[1];

  double xscale = (dx - cx) / (bx - ax);
  double yscale = (dy - cy) / (by - ay);

  int count = this->Internal->Regions.size();

  for (int i = 0; i < count; i++)
    {
    this->Internal->Regions[i]->X = (this->Internal->Regions[i]->X - ax)
        * xscale + cx;
    this->Internal->Regions[i]->Y = (this->Internal->Regions[i]->Y - ay)
        * yscale + cy;
    this->Internal->Regions[i]->Width = (this->Internal->Regions[i]->Width)
        * xscale;
    this->Internal->Regions[i]->Height = (this->Internal->Regions[i]->Height)
        * yscale;
    }

  }

//----------------------------------------------------------------------------
bool vtkTwoDTransferFunction::UpdateRange()
  {  //adapt range to Regions

  if (this->Internal->Regions.size() == 0)
    {
    return true;
    }
  double minX, minY, maxX, maxY;
  minX = this->Internal->Regions[0]->X;
  maxX = minX + this->Internal->Regions[0]->Width;
  minY = this->Internal->Regions[0]->Y;
  maxY = minY + this->Internal->Regions[0]->Height;

  for (int i = 1; i < this->Internal->Regions.size(); i++)
    {
    minX = std::min(minX, this->Internal->Regions[i]->X);
    minY = std::min(minX, this->Internal->Regions[i]->Y);
    maxX = std::max(maxX,
        this->Internal->Regions[i]->X + this->Internal->Regions[i]->Width);
    maxY = std::max(maxX,
        this->Internal->Regions[i]->Y + this->Internal->Regions[i]->Height);
    }

  Range[0] = minX;
  Range[1] = maxX;
  Range[2] = minY;
  Range[3] = maxY;

  return true;
  }

void vtkTwoDTransferFunction::RemoveAllRegions()
  {

  for (unsigned int i = 0; i < this->Internal->Regions.size(); i++)
    {
    delete this->Internal->Regions[i];
    }
  this->Internal->Regions.clear();

  }

void vtkTwoDTransferFunction::RemoveRegionAtIndex(int index)
  {

  delete this->Internal->Regions[index];

  this->Internal->Regions.erase(this->Internal->Regions.begin() + index);

  }

// Removes all points from the function.
void vtkTwoDTransferFunction::RemoveAllPoints()
  {
  RemoveAllRegions();
  }

// Return the value of the function at a position
double vtkTwoDTransferFunction::GetValue(double x)
  {
  return 0;
  }

// Remove all points outside the range, and make sure a point
// exists at each end of the range. Used as a convenience method
// for transfer function editors

void vtkTwoDTransferFunction::eliminateOutOfRange()
  {
  int done;

  done = 0;
  while (!done)
    {
    done = 1;

    this->Internal->FindRegionOutOfRange.X1 = Range[0];
    this->Internal->FindRegionOutOfRange.X2 = Range[1];
    this->Internal->FindRegionOutOfRange.Y1 = Range[2];
    this->Internal->FindRegionOutOfRange.Y2 = Range[3];

    std::vector<vtkTwoDTransferFunctionRegion*>::iterator iter = std::find_if(
        this->Internal->Regions.begin(), this->Internal->Regions.end(),
        this->Internal->FindRegionOutOfRange);

    if (iter != this->Internal->Regions.end())
      {
      delete *iter;
      this->Internal->Regions.erase(iter);
      this->Modified();
      done = 0;
      }
    }

  }

int vtkTwoDTransferFunction::SetRange(double xmin, double xmax, double ymin,
    double ymax)
  {

  double range[4] =
    {
    xmin, xmax, ymin, ymax
    };
  return SetRange(range);
  }

int vtkTwoDTransferFunction::SetRange(double range[4])
  {
  if (!Range || !range)
    {
    return 0;
    }

  if (range[1] < range[0])
    {
    double temp = range[1];
    range[1] = range[0];
    range[0] = temp;
    }
  if (range[3] < range[2])
    {
    double temp = range[3];
    range[3] = range[2];
    range[2] = temp;
    }

  double oldXRange[2] =
    {
    Range[0], Range[1]
    };
  double oldYRange[2] =
    {
    Range[2], Range[3]
    };
  double newXRange[2] =
    {
    range[0], range[1]
    };
  double newYRange[2] =
    {
    range[2], range[3]
    };

  scaleAndShift(oldXRange, oldYRange, newXRange, newYRange);

  this->Range[0] = range[0];
  this->Range[1] = range[1];
  this->Range[2] = range[2];
  this->Range[3] = range[3];

  eliminateOutOfRange();

  this->Modified();

  return 1;
  }

double vtkTwoDTransferFunction::GetRangeAtIndex(int index)
  {
  if (index >= 0 && index <= 3)
    {
    return Range[index];
    }
  else
    return 0;

  }

int vtkTwoDTransferFunction::SetXRange(double range[2])
  {
//TBD
  if (!Range || !range)
    {
    return 0;
    }

  this->Range[0] = range[0];
  this->Range[1] = range[1];

  eliminateOutOfRange();

  this->Modified();

  return 1;
  }

int vtkTwoDTransferFunction::SetYRange(double range[2])
  {
  //TBD

  if (!Range || !range)
    {
    return 0;
    }

  this->Range[2] = range[0];
  this->Range[3] = range[1];

  eliminateOutOfRange();

  this->Modified();

  return 1;
  }

int vtkTwoDTransferFunction::GetRegionValues(int index, double* val)
  {
  if (!(index < this->Internal->Regions.size()))
    {
    return -1;
    }
  else
    {
    val[0] = this->Internal->Regions[index]->X;
    val[1] = this->Internal->Regions[index]->Y;
    val[2] = this->Internal->Regions[index]->Width;
    val[3] = this->Internal->Regions[index]->Height;
    val[4] = double(this->Internal->Regions[index]->Mode);
    val[5] = this->Internal->Regions[index]->Maximum;
    return 0;
    }
  }

int vtkTwoDTransferFunction::GetRegionValues(int index, double* val,
    TransferFnMode &mo)
  {
  if (!(index < this->Internal->Regions.size()))
    {
    return -1;
    }
  else
    {
    val[0] = this->Internal->Regions[index]->X;
    val[1] = this->Internal->Regions[index]->Y;
    val[2] = this->Internal->Regions[index]->Width;
    val[3] = this->Internal->Regions[index]->Height;
    val[4] = this->Internal->Regions[index]->Maximum;
    mo = this->Internal->Regions[index]->Mode;
    return 0;
    }
  }

int vtkTwoDTransferFunction::SetRegionValues(int index, double val[5],
    TransferFnMode mo)
  {
  if (!(index < this->Internal->Regions.size()))
    {
    return -1;
    }
  else
    {
    this->Internal->Regions[index]->setValues(val, mo);
    return 0;
    }
  }

int vtkTwoDTransferFunction::SetRegionValue(int index, double val,
    regionvalue v)
  {
  if (!(index < this->Internal->Regions.size()))
    return -1;
  else if (vtkTwoDTransferFunction::REGION_X == v)
    this->Internal->Regions[index]->X = val;
  else if (vtkTwoDTransferFunction::REGION_Y == v)
    this->Internal->Regions[index]->Y = val;
  else if (vtkTwoDTransferFunction::REGION_W == v)
    this->Internal->Regions[index]->Width = val;
  else if (vtkTwoDTransferFunction::REGION_H == v)
    this->Internal->Regions[index]->Height = val;
  else if (vtkTwoDTransferFunction::REGION_MAX == v)
    this->Internal->Regions[index]->Maximum = val;
  else
    return -1;

  return index;
  }

int vtkTwoDTransferFunction::SetRegionMode(int index, TransferFnMode mo)
  {
  if (!(index < this->Internal->Regions.size()))
    return -1;
  else
    this->Internal->Regions[index]->Mode = mo;
  return index;
  }

// Given a table of values, build the piecewise function. Legacy method
// that does not allow for midpoint and sharpness control
void vtkTwoDTransferFunction::BuildFunctionFromTable(double xStart, double xEnd,
    int size, double* table, int stride)
  {
  //TBD
  }

// Given a pointer to an array of values, build the piecewise function.
// Legacy method that does not allow for midpoint and sharpness control
void vtkTwoDTransferFunction::FillFromDataPointer(int nb, double *ptr)
  {

  }

//----------------------------------------------------------------------------
vtkTwoDTransferFunction * vtkTwoDTransferFunction::GetData(
    vtkInformation * info)
  { //TBD don't know if this is valid, so don't rely on it
  return 0;
  }

//----------------------------------------------------------------------------
vtkTwoDTransferFunction* vtkTwoDTransferFunction::GetData(
    vtkInformationVector* v, int i)
  { //TBD don't know if this is valid, so don't rely on it
  return vtkTwoDTransferFunction::GetData(v->GetInformationObject(i));
  }

// Print method for vtkTwoDTransferFunction
void vtkTwoDTransferFunction::PrintSelf(ostream& os, vtkIndent indent)
  {

  }

void vtkTwoDTransferFunction::SortAndUpdateRange()
  { //not plans to sort this stuff. no good way of sorting it anyway

  }

