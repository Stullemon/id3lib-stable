// $Id: get_pic.cpp,v 1.4 2000/10/08 21:28:19 eldamitri Exp $

#include <iostream.h>
#include <stdlib.h>

#include <id3/tag.h>
#include <id3/error.h>
#include <id3/misc_support.h>

int main( int argc, char *argv[])
{
  try
  {
    if (argc != 3)
    {
      cout << "Usage: get_pic <tagfile> <picfilename>" << endl;
      return 1;
    }

    ID3_Tag tag(argv[1]);
    const ID3_Frame* frame = tag.Find(ID3FID_PICTURE);
    if (frame && frame->Contains(ID3FN_DATA))
    {
      cout << "*** extracting picture to file \"" << argv[2] << "\"...";
      frame->Field(ID3FN_DATA).ToFile(argv[2]);
      cout << " done!" << endl;
    }
    else
    {
      cout << "*** no picture frame found in \"" << argv[1] << "\"" << endl;
      return 1;
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
