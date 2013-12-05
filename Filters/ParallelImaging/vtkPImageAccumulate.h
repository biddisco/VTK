
// .NAME vtkPImageAccumulate - Extract histogram for parallel dataset.
// .SECTION Description
// vtkPImageAccumulate is vtkImageAccumulate subclass for parallel datasets.
// It gathers the histogram data on the root node.

#ifndef __vtkPImageAccumulate_h
#define __vtkPImageAccumulate_h

#include "vtkFiltersParallelImagingModule.h" // For export macro
#include "vtkImageAccumulate.h"

class vtkMultiProcessController;

class VTKFILTERSPARALLELIMAGING_EXPORT vtkPImageAccumulate : public vtkImageAccumulate
{
public:
  static vtkPImageAccumulate* New();
  vtkTypeMacro(vtkPImageAccumulate, vtkImageAccumulate);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the multiprocess controller. If no controller is set,
  // single process is assumed.
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);

protected:
  vtkPImageAccumulate();
  ~vtkPImageAccumulate();



 /* virtual int RequestUpdateExtent(vtkInformation*,
                                     vtkInformationVector**,
                                     vtkInformationVector*);
    virtual int RequestInformation (vtkInformation*,
                                    vtkInformationVector**,
                                    vtkInformationVector*);*/
    virtual int RequestData(vtkInformation* request,
                            vtkInformationVector** inputVector,
                            vtkInformationVector* outputVector);

  vtkMultiProcessController* Controller;
private:
  vtkPImageAccumulate(const vtkPImageAccumulate&); // Not implemented.
  void operator=(const vtkPImageAccumulate&); // Not implemented.
};

#endif

