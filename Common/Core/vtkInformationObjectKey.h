/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInformationObjectKey.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkInformationObjectKey - Key for vtkObject values.
// .SECTION Description
// vtkInformationObjectKey is used to represent keys in
// vtkInformation for values that are vtkObject instances.

#ifndef __vtkInformationObjectKey_h
#define __vtkInformationObjectKey_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkInformationKey.h"

#include "vtkCommonInformationKeyManager.h" // Manage instances of this type.

class vtkObject;

class VTKCOMMONCORE_EXPORT vtkInformationObjectKey : public vtkInformationKey
{
public:
  vtkTypeMacro(vtkInformationObjectKey,vtkInformationKey);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkInformationObjectKey(const char* name, const char* location);
  ~vtkInformationObjectKey();

  //BTX
  // Description:
  // Get/Set the value associated with this key in the given
  // information object.
  void Set(vtkInformation* info, vtkObject*);
  vtkObject* Get(vtkInformation* info);
  //ETX

  // Description:
  // Copy the entry associated with this key from one information
  // object to another.  If there is no entry in the first information
  // object for this key, the value is removed from the second.
  virtual void ShallowCopy(vtkInformation* from, vtkInformation* to);

  // Description:
  // Report a reference this key has in the given information object.
  virtual void Report(vtkInformation* info, vtkGarbageCollector* collector);

private:
  vtkInformationObjectKey(const vtkInformationObjectKey&);  // Not implemented.
  void operator=(const vtkInformationObjectKey&);  // Not implemented.
};

#endif
