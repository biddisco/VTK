/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInformationObjectMetaDataKey.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkInformationObjectMetaDataKey - key used to define meta-data of type vtkDataObject
// vtkInformationObjectMetaDataKey is a vtkInformationDataObjectKey
// that (shallow) copies itself downstream during the REQUEST_INFORMATION pass. Hence
// it can be used to provide meta-data of type vtkDataObject or any subclass.

#ifndef __vtkInformationObjectMetaDataKey_h
#define __vtkInformationObjectMetaDataKey_h

#include "vtkCommonExecutionModelModule.h" // For export macro
#include "vtkInformationObjectKey.h"

#include "vtkCommonInformationKeyManager.h" // Manage instances of this type.

class VTKCOMMONEXECUTIONMODEL_EXPORT vtkInformationObjectMetaDataKey : public vtkInformationObjectKey
{
public:
  vtkTypeMacro(vtkInformationObjectMetaDataKey,vtkInformationObjectKey);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkInformationObjectMetaDataKey(const char* name, const char* location);
  ~vtkInformationObjectMetaDataKey();

  // Description:
  // This method simply returns a new vtkInformationDataObjectMetaDataKey, given a
  // name and a location. This method is provided for wrappers. Use the
  // constructor directly from C++ instead.
  static vtkInformationDataObjectMetaDataKey* MakeKey(const char* name, const char* location)
    {
    return new vtkInformationDataObjectMetaDataKey(name, location);
    }

  // Description:
  // Simply shallow copies the key from fromInfo to toInfo if request
  // has the REQUEST_INFORMATION() key.
  // This is used by the pipeline to propagate this key downstream.
  virtual void CopyDefaultInformation(vtkInformation* request,
                                      vtkInformation* fromInfo,
                                      vtkInformation* toInfo);

private:
  vtkInformationObjectMetaDataKey(const vtkInformationObjectMetaDataKey&);  // Not implemented.
  void operator=(const vtkInformationObjectMetaDataKey&);  // Not implemented.
};

#endif
