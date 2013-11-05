

// .NAME
// .SECTION Description
// this program tests the GaussianPiecewiseFunction

#include "vtkGaussianPiecewiseFunction.h"
#include "vtkDebugLeaks.h"

#include <vtksys/ios/sstream>

int TestOGPF(ostream& strm)
{
  int i, j, k;
  vtkGaussianPiecewiseFunction *ctf1 = vtkGaussianPiecewiseFunction::New();

  // actual test
  strm << "Test vtkGaussianPiecewiseFunction Start" << endl;

  ctf1->AddGaussian (0.0, 1, 1, 0,0);
  strm << *ctf1;

//get values at 0, 1,-1

  double val;
  val = ctf1->GetValue(0);
  strm << "GetValue(0) = "
    << val << endl;


  val = ctf1->GetValue(1);
  strm << "GetValue(1) = "
    << val << endl;

  val = ctf1->GetValue(-1);
  strm << "GetValue(-1) = "
    << val << endl;
  val = ctf1->GetValue(-1.5);
  strm << "GetValue(-1.5) = "
    << val << endl;
  val = ctf1->GetValue(1.5);
  strm << "GetValue(1.5) = "
    << val << endl;




  strm << "GetRange = "
       << ctf1->GetRange()[0] << ","
       << ctf1->GetRange()[1] << endl;

  double table[256];

  ctf1->GetTable(-1.5, 1.5, 256, &table[0]);
  strm << "GetTable(0, 15, 256, &table[0][0])" << endl;
  for (i = 0; i < 256; i++)
    {
        strm << table[i] << " ";

    }
  strm << endl;

  strm << "BuildFunctionFrom(0, 15, 256, &table[0][0])" << endl;
  vtkGaussianPiecewiseFunction *ctf2 = vtkGaussianPiecewiseFunction::New();
  ctf2->BuildFunctionFromTable(-1.5, 1.5, 256, &table[0]);

  ctf2->GetTable(-1.5,1.5,256,table);



  ctf1->DeepCopy(ctf2);
  strm << "ctf1->DeepCopy(ctf2)" << endl;
  strm << *ctf1;

  ctf1->RemoveGaussian(10);
  strm << *ctf1;

  ctf1->RemoveAllPoints();
  strm << *ctf1;


  ctf1->Delete();
  ctf2->Delete();










  strm << "Test vtkColorTransferFunction End" << endl;
  return 0;
}


int otherGaussianPiecewiseFunction(int, char *[])
{
  vtksys_ios::ostringstream vtkmsg_with_warning_C4701;
  return TestOGPF(vtkmsg_with_warning_C4701);
}
