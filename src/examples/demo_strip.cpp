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
#include <getopt.h>

void PrintUsage(char *sName)
{
  cout << "Usage: " << sName << " [OPTION]... [FILE]..." << endl;
  cout << "Strips id3 (both v1 and v2) tags off of a file." << endl;
  cout << endl;
  cout << "  -1, --v1tag     Remove only the id3v1 tag" << endl;
  cout << "  -2, --v2tag     Remove only the id3v2 tag" << endl;
  cout << "  -h, --help      Display this help and exit" << endl;
  cout << "  -v, --version   Display version information and exit" << endl;
  cout << endl;
  cout << "Will strip both types of tag by default.  Only the last tag type indicated" << endl 
       << "in the option list will be used" << endl;
}

void PrintVersion(char *sName)
{
  cout << sName << " 1.0" << endl;
  cout << "An ID3 Tag Stripper - Written by Scott Thomas Haug" << endl;
  cout << "Uses " << ID3LIB_NAME << " " << ID3LIB_VERSION << endl << endl;
}

int main( int argc, char *argv[])
{
  int ulFlag = BOTH_TAGS;
  int iOpt;
  bool bError = false;
  while (true)
  {
    int option_index = 0;
    int iLongOpt = 0;
    static struct option long_options[] = 
    { 
      { "v1tag",   no_argument, &iLongOpt, '1' },
      { "v2tag",   no_argument, &iLongOpt, '2' },
      { "version", no_argument, &iLongOpt, 'v' },
      { "help",    no_argument, &iLongOpt, 'h' },
      { 0, 0, 0, 0 }
    };
    iOpt = getopt_long (argc, argv, "12vh", long_options, &option_index);

    if (iOpt == -1)
      break;

    if (iOpt == 0) iOpt = iLongOpt;

    switch (iOpt)
    {
      case '1':
        ulFlag = V1_TAG;
        break;
        
      case '2':
        ulFlag = V2_TAG;
        break;

      case 'v':
        PrintVersion(argv[0]);
        exit (0);
        break;
        

      case 'h':
        PrintUsage(argv[0]);
        exit (0);
        break;

      case '?':
        bError = true;
        break;

      default:
        cout << "?? getopt returned character code " << iOpt << " ??" << endl;
    }
  }
  if (bError)
  {
    cout << "Try `" << argv[0] << " --help' for more information." << endl;
  }
  else 
    for (size_t nIndex = optind; nIndex < argc; nIndex++)
    {
      try
      {
        ID3_Tag myTag;
        myTag.Link(argv[nIndex]);
        myTag.Strip(ulFlag);
      }

      catch(ID3_Error err)
      {
        cout << err.GetErrorFile() << " (" << err.GetErrorLine() << "): "
             << err.GetErrorType() << ": " << err.GetErrorDesc() << endl;
      }
    }

  return 0;
}
