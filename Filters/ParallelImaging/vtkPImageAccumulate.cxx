#include "vtkPImageAccumulate.h"
#include "vtkObjectFactory.h"
#include "vtkMultiProcessController.h"

#include "vtkImageData.h"
#include "vtkImageStencilData.h"
#include "vtkImageStencilIterator.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkSmartPointer.h"






vtkStandardNewMacro(vtkPImageAccumulate);
vtkCxxSetObjectMacro(vtkPImageAccumulate, Controller, vtkMultiProcessController);
//-----------------------------------------------------------------------------
vtkPImageAccumulate::vtkPImageAccumulate()
{
  this->Controller = 0;
  this->SetController(vtkMultiProcessController::GetGlobalController());
}

//-----------------------------------------------------------------------------
vtkPImageAccumulate::~vtkPImageAccumulate()
{
  this->SetController(0);
}

//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// This method is passed a input and output Data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the Datas data types.
int vtkPImageAccumulate::RequestData(
  vtkInformation*  request ,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
this->Superclass::RequestData(request,inputVector,outputVector);

vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkImageData *outData = vtkImageData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  int dims[3];
  outData->GetDimensions(dims);
  outData->GetScalarPointer();


  int* temp = new int[dims[0]*dims[1]*dims[2]];

  memcpy(&temp[0],outData->GetScalarPointer(),dims[0]*dims[1]*dims[2]*sizeof(int));

  Controller->AllReduce(&temp[0], static_cast<int*>(outData->GetScalarPointer()), dims[0]*dims[1]*dims[2], vtkCommunicator::SUM_OP);






return 1;
}

void vtkPImageAccumulate::PrintSelf(ostream& os, vtkIndent indent)
  {

  }
