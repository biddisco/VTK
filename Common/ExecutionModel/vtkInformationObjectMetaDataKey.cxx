/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInformationObjectMetaDataKey.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkInformationObjectMetaDataKey.h"

#include "vtkInformation.h"
#include "vtkStreamingDemandDrivenPipeline.h"

//----------------------------------------------------------------------------
vtkInformationObjectMetaDataKey::vtkInformationObjectMetaDataKey(const char* name, const char* location) :
  vtkInformationObjectKey(name, location)
{
}

//----------------------------------------------------------------------------
vtkInformationObjectMetaDataKey::~vtkInformationObjectMetaDataKey()
{
}

//----------------------------------------------------------------------------
void vtkInformationObjectMetaDataKey::CopyDefaultInformation(
  vtkInformation* request,
  vtkInformation* fromInfo,
  vtkInformation* toInfo)
{
  if (request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_INFORMATION()))
  {
    this->ShallowCopy(fromInfo, toInfo);
  }
}

//----------------------------------------------------------------------------
void vtkInformationObjectMetaDataKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
