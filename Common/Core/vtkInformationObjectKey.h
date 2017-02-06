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
/**
 * @class   vtkInformationObjectKey
 * @brief   Key for vtkObject values.
 *
 * vtkInformationObjectKey is used to represent keys in
 * vtkInformation for values that are vtkObject instances.
*/

#ifndef vtkInformationObjectKey_h
#define vtkInformationObjectKey_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkInformationKey.h"

#include "vtkCommonInformationKeyManager.h" // Manage instances of this type.

class vtkObject;

class VTKCOMMONCORE_EXPORT vtkInformationObjectKey : public vtkInformationKey
{
public:
  vtkTypeMacro(vtkInformationObjectKey,vtkInformationKey);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  vtkInformationObjectKey(const char* name, const char* location);
  ~vtkInformationObjectKey() VTK_OVERRIDE;

  /**
   * This method simply returns a new vtkInformationObjectKey, given a
   * name, location. This method is provided
   * for wrappers. Use the constructor directly from C++ instead.
   */
  static vtkInformationObjectKey* MakeKey(const char* name, const char* location)
  {
    return new vtkInformationObjectKey(name, location);
  }

  //@{
  /**
   * Get/Set the value associated with this key in the given
   * information object.
   */
  void Set(vtkInformation* info, vtkObject*);
  vtkObject* Get(vtkInformation* info);
  //@}

  /**
   * Copy the entry associated with this key from one information
   * object to another.  If there is no entry in the first information
   * object for this key, the value is removed from the second.
   */
  void ShallowCopy(vtkInformation* from, vtkInformation* to) VTK_OVERRIDE;

  /**
   * Report a reference this key has in the given information object.
   */
  void Report(vtkInformation* info, vtkGarbageCollector* collector) VTK_OVERRIDE;

private:
  vtkInformationObjectKey(const vtkInformationObjectKey&) VTK_DELETE_FUNCTION;
  void operator=(const vtkInformationObjectKey&) VTK_DELETE_FUNCTION;
};

#endif
