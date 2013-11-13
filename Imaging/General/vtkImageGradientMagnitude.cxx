/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageGradientMagnitude.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkImageGradientMagnitude.h"

#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"

#include <math.h>

vtkStandardNewMacro(vtkImageGradientMagnitude);

//----------------------------------------------------------------------------
// Construct an instance of vtkImageGradientMagnitude fitler.
vtkImageGradientMagnitude::vtkImageGradientMagnitude()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
  this->Dimensionality = 2;
  this->HandleBoundaries = 1;
}


//----------------------------------------------------------------------------
void vtkImageGradientMagnitude::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "HandleBoundaries: " << this->HandleBoundaries << "\n";
  os << indent << "Dimensionality: " << this->Dimensionality << "\n";
}

//----------------------------------------------------------------------------
// This method is passed a region that holds the image extent of this filters
// input, and changes the region to hold the image extent of this filters
// output.
int vtkImageGradientMagnitude::RequestInformation (
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  int extent[6];
  int idx;

  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

  // invalid setting, it has not been set, so default to whole Extent
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
              extent);

  if ( ! this->HandleBoundaries)
    {
    // shrink output image extent.
    for (idx = 0; idx < this->Dimensionality; ++idx)
      {
      extent[idx*2] += 1;
      extent[idx*2 + 1] -= 1;
      }
    }

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
               extent, 6);

  return 1;
}


//----------------------------------------------------------------------------
// This method computes the input extent necessary to generate the output.
int vtkImageGradientMagnitude::RequestUpdateExtent (
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  int wholeExtent[6];
  int idx;

  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

  // invalid setting, it has not been set, so default to whole Extent
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
              wholeExtent);
  int inUExt[6];
  outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), inUExt);

  // grow input whole extent.
  for (idx = 0; idx < this->Dimensionality; ++idx)
    {
    inUExt[idx*2] -= 1;
    inUExt[idx*2+1] += 1;
    if (this->HandleBoundaries)
      {
      // we must clip extent with whole extent is we hanlde boundaries.
      if (inUExt[idx*2] < wholeExtent[idx*2])
        {
        inUExt[idx*2] = wholeExtent[idx*2];
        }
      if (inUExt[idx*2 + 1] > wholeExtent[idx*2 + 1])
        {
        inUExt[idx*2 + 1] = wholeExtent[idx*2 + 1];
        }
      }
    }
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), inUExt, 6);

  return 1;
}

//----------------------------------------------------------------------------
// This execute method handles boundaries.
// it handles boundaries. Pixels are just replicated to get values
// out of extent.
template <class T>
void vtkImageGradientMagnitudeExecute(vtkImageGradientMagnitude *self,
                                      vtkImageData *inData, T *inPtr,
                                      vtkImageData *outData, double *outPtr,
                                      int outExt[6], int id)
{
  int idxC, idxX, idxY, idxZ;
  int maxC, maxX, maxY, maxZ;
  vtkIdType inIncX, inIncY, inIncZ;
  vtkIdType outIncX, outIncY, outIncZ;
  unsigned long count = 0;
  unsigned long target;
  int axesNum;
  int *wholeExtent;
  vtkIdType *inIncs;
  double r[3], d, sum;
  int useZMin, useZMax, useYMin, useYMax, useXMin, useXMax;
  int *inExt = inData->GetExtent();

  // find the region to loop over
  maxC = outData->GetNumberOfScalarComponents();
  maxX = outExt[1] - outExt[0];
  maxY = outExt[3] - outExt[2];
  maxZ = outExt[5] - outExt[4];
  target = static_cast<unsigned long>((maxZ+1)*(maxY+1)/50.0);
  target++;

  // Get the dimensionality of the gradient.
  axesNum = self->GetDimensionality();

  // Get increments to march through data
  inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  // The data spacing is important for computing the gradient.
  inData->GetSpacing(r);
  r[0] = 0.5 / r[0];
  r[1] = 0.5 / r[1];
  r[2] = 0.5 / r[2];

  // get some other info we need
  inIncs = inData->GetIncrements();
  wholeExtent = inData->GetExtent();

  // Move the starting pointer to the correct location.
  inPtr += (outExt[0]-inExt[0])*inIncs[0] +
           (outExt[2]-inExt[2])*inIncs[1] +
           (outExt[4]-inExt[4])*inIncs[2];

  // Loop through output pixels
  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
    useZMin = ((idxZ + outExt[4]) <= wholeExtent[4]) ? 0 : -inIncs[2];
    useZMax = ((idxZ + outExt[4]) >= wholeExtent[5]) ? 0 : inIncs[2];
    for (idxY = 0; !self->AbortExecute && idxY <= maxY; idxY++)
      {
      if (!id)
        {
        if (!(count%target))
          {
          self->UpdateProgress(count/(50.0*target));
          }
        count++;
        }
      useYMin = ((idxY + outExt[2]) <= wholeExtent[2]) ? 0 : -inIncs[1];
      useYMax = ((idxY + outExt[2]) >= wholeExtent[3]) ? 0 : inIncs[1];
      for (idxX = 0; idxX <= maxX; idxX++)
        {
        useXMin = ((idxX + outExt[0]) <= wholeExtent[0]) ? 0 : -inIncs[0];
        useXMax = ((idxX + outExt[0]) >= wholeExtent[1]) ? 0 : inIncs[0];
        for (idxC = 0; idxC < maxC; idxC++)
          {
          // do X axis
          d = static_cast<double>(inPtr[useXMin]);
          d -= static_cast<double>(inPtr[useXMax]);
          d *= r[0]; // multiply by the data spacing
          sum = d * d;
          // do y axis
          d = static_cast<double>(inPtr[useYMin]);
          d -= static_cast<double>(inPtr[useYMax]);
          d *= r[1]; // multiply by the data spacing
          sum += (d * d);
          if (axesNum == 3)
            {
            // do z axis
            d = static_cast<double>(inPtr[useZMin]);
            d -= static_cast<double>(inPtr[useZMax]);
            d *= r[2]; // multiply by the data spacing
            sum += (d * d);
            }
          *outPtr = static_cast<double>(sqrt(sum));
          outPtr++;
          inPtr++;
          }
        }
      outPtr += outIncY;
      inPtr += inIncY;
      }
    outPtr += outIncZ;
    inPtr += inIncZ;
    }
}





//----------------------------------------------------------------------------
// This method contains a switch statement that calls the correct
// templated function for the input data type.  This method does handle
// boundary conditions.
void vtkImageGradientMagnitude::ThreadedRequestData(vtkInformation*,
                                           vtkInformationVector** inputVector,
                                           vtkInformationVector*,
                                           vtkImageData*** inData,
                                           vtkImageData** outData,
                                           int outExt[6],
                                           int threadId)
{
  // Get the input and output data objects.
  vtkImageData* input = inData[0][0];
  vtkImageData* output = outData[0];

  // The ouptut scalar type must be double to store proper gradients.
  if(output->GetScalarType() != VTK_DOUBLE)
    {
    vtkErrorMacro("Execute: output ScalarType is "
                  << output->GetScalarType() << "but must be double.");
    return;
    }

  vtkDataArray* inputArray = this->GetInputArrayToProcess(0, inputVector);
  if (!inputArray)
    {
    vtkErrorMacro("No input array was found. Cannot execute");
    return;
    }

  // Gradient makes sense only with one input component.  This is not
  // a Jacobian filter.
  if(inputArray->GetNumberOfComponents() != 1)
    {
    vtkErrorMacro(
      "Execute: input has more than one component. "
      "The input to gradient should be a single component image. "
      "Think about it. If you insist on using a color image then "
      "run it though RGBToHSV then ExtractComponents to get the V "
      "components. That's probably what you want anyhow.");
    return;
    }

  void* inPtr = inputArray->GetVoidPointer(0);
  double* outPtr = static_cast<double *>(
    output->GetScalarPointerForExtent(outExt));
  switch(inputArray->GetDataType())
    {
    vtkTemplateMacro(
      vtkImageGradientMagnitudeExecute(this, input, static_cast<VTK_TT*>(inPtr),
                              output, outPtr, outExt, threadId)
      );
    default:
      vtkErrorMacro("Execute: Unknown ScalarType " << input->GetScalarType());
      return;
    }
}









