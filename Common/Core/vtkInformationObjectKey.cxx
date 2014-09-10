/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInformationObjectKey.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkInformationObjectKey.h"

//----------------------------------------------------------------------------
vtkInformationObjectKey::vtkInformationObjectKey(const char* name, const char* location):
  vtkInformationKey(name, location)
{
  vtkCommonInformationKeyManager::Register(this);
}

//----------------------------------------------------------------------------
vtkInformationObjectKey::~vtkInformationObjectKey()
{
}

//----------------------------------------------------------------------------
void vtkInformationObjectKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkInformationObjectKey::Set(vtkInformation* info,
                                      vtkObject* value)
{
  this->SetAsObjectBase(info, value);
}

//----------------------------------------------------------------------------
vtkObject* vtkInformationObjectKey::Get(vtkInformation* info)
{
  return static_cast<vtkObject *>(this->GetAsObjectBase(info));
}

//----------------------------------------------------------------------------
void vtkInformationObjectKey::ShallowCopy(vtkInformation* from,
                                              vtkInformation* to)
{
  this->Set(to, this->Get(from));
}

//----------------------------------------------------------------------------
void vtkInformationObjectKey::Report(vtkInformation* info,
                                         vtkGarbageCollector* collector)
{
  this->ReportAsObjectBase(info, collector);
}
