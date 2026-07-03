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
/**
 * @class   vtkInformationObjectMetaDataKey
 * @brief   key used to define meta-data of type vtkObject
 * vtkInformationObjectMetaDataKey is a vtkInformationObjectKey
 * that (shallow) copies itself downstream during the REQUEST_INFORMATION pass. Hence
 * it can be used to provide meta-data of type vtkDataObject or any subclass.
*/

#ifndef vtkInformationObjectMetaDataKey_h
#define vtkInformationObjectMetaDataKey_h

#include "vtkCommonExecutionModelModule.h" // For export macro
#include "vtkInformationObjectKey.h"

#include "vtkCommonInformationKeyManager.h" // Manage instances of this type.

class VTKCOMMONEXECUTIONMODEL_EXPORT vtkInformationObjectMetaDataKey : public vtkInformationObjectKey
{
public:
  vtkTypeMacro(vtkInformationObjectMetaDataKey,vtkInformationObjectKey);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkInformationObjectMetaDataKey(const char* name, const char* location);
  ~vtkInformationObjectMetaDataKey() override;

  /**
   * This method simply returns a new vtkInformationObjectMetaDataKey, given a
   * name and a location. This method is provided for wrappers. Use the
   * constructor directly from C++ instead.
   */
  static vtkInformationObjectMetaDataKey* MakeKey(const char* name, const char* location)
  {
    return new vtkInformationObjectMetaDataKey(name, location);
  }

  /**
   * Simply shallow copies the key from fromInfo to toInfo if request
   * has the REQUEST_INFORMATION() key.
   * This is used by the pipeline to propagate this key downstream.
   */
  void CopyDefaultInformation(vtkInformation* request,
                                      vtkInformation* fromInfo,
                                      vtkInformation* toInfo) override;

private:
  vtkInformationObjectMetaDataKey(const vtkInformationObjectMetaDataKey&) = delete;
  void operator=(const vtkInformationObjectMetaDataKey&) = delete;
};

#endif
