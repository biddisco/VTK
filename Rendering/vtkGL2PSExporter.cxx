/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGL2PSExporter.cxx
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

#include "vtkGL2PSExporter.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "gl2ps.h"

vtkCxxRevisionMacro(vtkGL2PSExporter, "1.1");
vtkStandardNewMacro(vtkGL2PSExporter);

vtkGL2PSExporter::vtkGL2PSExporter()
{
  this->FilePrefix = NULL;
  this->FileFormat = PS_FILE;
  this->DrawBackground = 0;
  this->SimpleLineOffset = 1;
  this->Silent = 0;
  this->BestRoot = 1;
  this->Text = 1;
  this->Landscape = 0;
  this->PS3Shading = 1;
  this->OcclusionCull = 1;
}

vtkGL2PSExporter::~vtkGL2PSExporter()
{
  if ( this->FilePrefix )
    {
    delete [] this->FilePrefix;
    }
}

void vtkGL2PSExporter::WriteData()
{
  FILE *fpObj;
  char *fName;
  GLint format;
  GLint options = GL2PS_NONE;

  // Set the options based on user's choices.   
  if (this->DrawBackground == 1)
    {
    options = options | GL2PS_DRAW_BACKGROUND;
    }
  if (this->SimpleLineOffset == 1)
    {
    options = options | GL2PS_SIMPLE_LINE_OFFSET;
    }
  if (this->Silent == 1)
    {
    options = options | GL2PS_SILENT;
    }
  if (this->BestRoot == 1)
    {
    options = options | GL2PS_BEST_ROOT;
    }
  if (this->Text == 0)
    {
    options = options | GL2PS_NO_TEXT;
    }
  if (this->Landscape == 1)
    {
    options = options | GL2PS_LANDSCAPE;
    }
  if (this->PS3Shading == 0)
    {
    options = options | GL2PS_NO_PS3_SHADING;
    }
  if (this->OcclusionCull == 1)
    {
    options = options | GL2PS_OCCLUSION_CULL;
    }
  

  // Setup the file.
  fName = new char [strlen(this->FilePrefix) + 4] ;
  if (this->FileFormat == PS_FILE)
    {
    sprintf(fName, "%s.ps", this->FilePrefix);
    format = GL2PS_PS;
    }
  else if (this->FileFormat == EPS_FILE)
    {
    sprintf(fName, "%s.eps", this->FilePrefix);
    format = GL2PS_EPS;
    }
  else if (this->FileFormat == TEX_FILE)
    {
    sprintf(fName, "%s.tex", this->FilePrefix);
    format = GL2PS_TEX;
    }
  
  fpObj = fopen(fName, "w");
  if (!fpObj)
    {
    vtkErrorMacro(<< "unable to open file: " << fName);
    return;
    }

  // Call gl2ps to generate the file.
  int buffsize = 0;
  int state = GL2PS_OVERFLOW;
  while(state == GL2PS_OVERFLOW)
    {
    buffsize += 1024*1024;
    gl2psBeginPage(this->RenderWindow->GetWindowName(), "VTK",
                   format, GL2PS_BSP_SORT, options,
                   GL_RGBA, 0, NULL, buffsize, fpObj, fName);
    this->RenderWindow->Render();
    state = gl2psEndPage();
    }
  fclose(fpObj);
}

void vtkGL2PSExporter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
 
  if (this->FilePrefix)
    {
    os << indent << "FilePrefix: " << this->FilePrefix << "\n";
    }
  else
    {
    os << indent << "FilePrefix: (null)\n";      
    }

  os << indent << "FileFormat: ";
  switch (this->FileFormat)
    {
    case PS_FILE:
      os << "PS";
      break;
    case EPS_FILE:
      os << "EPS";
      break;
    case TEX_FILE:
      os << "TeX";
      break;
    }
  os << endl;

  os << indent << "DrawBackground: "
     << (this->DrawBackground ? "On\n" : "Off\n");
  os << indent << "SimpleLineOffset: "
     << (this->SimpleLineOffset ? "On\n" : "Off\n");
  os << indent << "Silent: "
     << (this->Silent ? "On\n" : "Off\n");
  os << indent << "BestRoot: "
     << (this->BestRoot ? "On\n" : "Off\n");
  os << indent << "Text: "
     << (this->Text ? "On\n" : "Off\n");
  os << indent << "Landscape: "
     << (this->Landscape ? "On\n" : "Off\n");
  os << indent << "PS3Shading: "
     << (this->PS3Shading ? "On\n" : "Off\n");
  os << indent << "OcclusionCull: "
     << (this->OcclusionCull ? "On\n" : "Off\n");
}
