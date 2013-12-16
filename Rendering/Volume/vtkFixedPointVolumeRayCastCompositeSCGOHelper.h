/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkFixedPointVolumeRayCastCompositeSCGOHelper.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkFixedPointVolumeRayCastCompositeSCGOHelper - A helper that generates composite images for the volume ray cast mapper
// .SECTION Description
// This is one of the helper classes for the vtkFixedPointVolumeRayCastMapper.
// It will generate composite images using an alpha blending operation.
// This class should not be used directly, it is a helper class for
// the mapper and has no user-level API.
//
// .SECTION see also
// vtkFixedPointVolumeRayCastMapper

#ifndef __vtkFixedPointVolumeRayCastCompositeSCGOHelper_h
#define __vtkFixedPointVolumeRayCastCompositeSCGOHelper_h

#include "vtkRenderingVolumeModule.h" // For export macro
#include "vtkFixedPointVolumeRayCastHelper.h"

class vtkFixedPointVolumeRayCastMapper;
class vtkVolume;

class VTKRENDERINGVOLUME_EXPORT vtkFixedPointVolumeRayCastCompositeSCGOHelper : public vtkFixedPointVolumeRayCastHelper
{
public:
  static vtkFixedPointVolumeRayCastCompositeSCGOHelper *New();
  vtkTypeMacro(vtkFixedPointVolumeRayCastCompositeSCGOHelper,vtkFixedPointVolumeRayCastHelper);
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual void  GenerateImage( int threadID,
                               int threadCount,
                               vtkVolume *vol,
                               vtkFixedPointVolumeRayCastMapper *mapper);

protected:
  vtkFixedPointVolumeRayCastCompositeSCGOHelper();
  ~vtkFixedPointVolumeRayCastCompositeSCGOHelper();

private:
  vtkFixedPointVolumeRayCastCompositeSCGOHelper(const vtkFixedPointVolumeRayCastCompositeSCGOHelper&);  // Not implemented.
  void operator=(const vtkFixedPointVolumeRayCastCompositeSCGOHelper&);  // Not implemented.
};

#endif
