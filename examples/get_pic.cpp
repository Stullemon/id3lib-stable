// $Id: get_pic.cpp,v 1.1 2000/05/28 22:03:11 eldamitri Exp $

#include <iostream.h>
#include <getopt.h>
#include <stdlib.h>

#include <id3/tag.h>
#include <id3/misc_support.h>

int main( int argc, char *argv[])
{
  try
  {
    cerr << "*** argc = " << argc << endl;
    if (argc != 3)
    {
      cout << "Usage: get_pic <tagfile> <picfilename>" << endl;
      exit(1);
    }

    ID3_Tag tag(argv[1]);
    const ID3_Frame* frame = tag.Find(ID3FID_PICTURE);
    if (frame && frame->Contains(ID3FN_DATA))
    {
      cout << "*** extracting picture to file \"" << argv[2] << "\"" << endl;
      frame->Field(ID3FN_DATA).ToFile(argv[2]);
    }
    else
    {
      cout << "*** no picture frame found in \"" << argv[1] << "\"" << endl;
      exit(1);
    }
  }   
  catch(const ID3_Error& err)
  {
    cout << endl;
    cout << err.GetErrorFile() << " (" << err.GetErrorLine() << "): "
         << err.GetErrorType() << ": " << err.GetErrorDesc() << endl;
  }

  return 0;
}
