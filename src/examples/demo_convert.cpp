//  The authors have released ID3Lib as Public Domain (PD) and claim no
//  copyright, patent or other intellectual property protection in this work.
//  This means that it may be modified, redistributed and used in commercial
//  and non-commercial software and hardware without restrictions.  ID3Lib is
//  distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
//  express or implied.
//  
//  The ID3Lib authors encourage improvements and optimisations to be sent to
//  the ID3Lib coordinator, currently Dirk Mahoney (dirk@id3.org).  Approved
//  submissions may be altered, and will be included and released under these
//  terms.
//  
//  Mon Nov 23 18:34:01 1998


#include <iostream.h>
#include <id3/tag.h>

int main( int argc, char *argv[])
{
  cout << "ID3v2 Tag Converter - No copyright 1998 Dirk Mahoney" << endl;
  cout << "Uses " << ID3LIB_NAME << " " << ID3LIB_VERSION << endl << endl;

  cout << "This program converts ID3v1/1.1 and Lyrics3 v2.0" << endl;
  cout << "tags to ID3v2 tags.  This program will also remove" << endl;
  cout << "the old tags." << endl << endl;

  if (argc > 1)
  {
    try
    {
      ID3_Tag myTag;
      luint argNum = 1;

      myTag.Link(argv[argNum]);
      myTag.Strip();
      myTag.Update();
    }

    catch(ID3_Error err)
    {
      cout << err.GetErrorFile() << " (" << err.GetErrorLine() << "): "
           << err.GetErrorType() << ": " << err.GetErrorDesc() << endl;
    }
  }
  else
  {
    cout << "Usage: " << argv[0] << " <file>" << endl << endl;

    cout << "Where: 'file' is the file you wish to convert." << endl << endl;
  }

  return 0;
}


