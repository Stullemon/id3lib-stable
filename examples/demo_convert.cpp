// $Id: demo_convert.cpp,v 1.9 2000/10/02 17:01:45 eldamitri Exp $
//
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

#include <iostream.h>
#include <id3/tag.h>
#include <id3/error.h>
#include <popt.h>

static const char* VERSION_NUMBER = "$Revision: 1.9 $";

void PrintUsage(const char *sName)
{
  cout << "Usage: " << sName << " [OPTION]... [FILE]..." << endl;
  cout << "Converts between id3v1 and id3v2 tags of an mp3 file." << endl;
  cout << endl;
  cout << "  -1, --v1tag     Render only the id3v1 tag" << endl;
  cout << "  -2, --v2tag     Render only the id3v2 tag" << endl;
  cout << "  -s, --strip     Strip, rather than render, the tags" << endl;
  cout << "  -p, --padding   Use padding in the tag" << endl;
  cout << "  -h, --help      Display this help and exit" << endl;
  cout << "  -v, --version   Display version information and exit" << endl;
  cout << endl;
  cout << "Will render both types of tag by default.  Only the last" << endl
       << "tag type indicated in the option list will be used.  Non-" << endl
       << "rendered will remain unchanged in the original file.  Will" << endl
       << "also parse and convert Lyrics3 v2.0 frames, but will not" << endl
       << "render them." << endl;
}

void PrintVersion(const char *sName)
{
  size_t nIndex;
  cout << sName << " ";
  for (nIndex = 0; nIndex < strlen(VERSION_NUMBER); nIndex++)
  {
    if (VERSION_NUMBER[nIndex] == ' ') 
    {
      break;
    }
  }
  nIndex++;
  for (; nIndex < strlen (VERSION_NUMBER); nIndex++)
  {
    if (VERSION_NUMBER[nIndex] == ' ') 
    {
      break;
    }
    cout << VERSION_NUMBER[nIndex];
  }
  cout << endl;
  cout << "Uses " << ID3LIB_FULL_NAME << endl << endl;

  cout << "This program converts and strips ID3v1/1.1 and Lyrics3 v2.0" << endl;
  cout << "tags to ID3v2 tags." << endl << endl;
}

void DisplayTags(ostream &os, luint nTags)
{
  if (!((nTags & ID3TT_ID3V1) || (nTags & ID3TT_ID3V2)))
  {
    os << "no tag";
  }
  if (nTags & ID3TT_ID3V1)
  {
    os << "v1";
  }
  if ((nTags & ID3TT_ID3V1) && (nTags & ID3TT_ID3V2))
  {
    os << " and ";
  }
  if (nTags & ID3TT_ID3V2)
  {
    os << "v2";
  }
}

int main( unsigned int argc, const char *argv[])
{
  const char* appname = argv[0];
  int ret = 0;
  flags_t ulFlag = ID3TT_ALL;
  bool bStrip = false;
  bool bPadding = false;
  bool bVersion = false;
  bool bHelp = false;
  static struct poptOption options[] =
  {
    { "v1tag",   '1', POPT_ARG_NONE, NULL,      ID3TT_ID3V1 },
    { "v2tag",   '2', POPT_ARG_NONE, NULL,      ID3TT_ID3V2 },
    { "strip",   's', POPT_ARG_NONE, &bStrip,   0 },
    { "padding", 'p', POPT_ARG_NONE, &bPadding, 0 },
    { "version", 'v', POPT_ARG_NONE, &bVersion, 0 },
    { "help",    'h', POPT_ARG_NONE, &bHelp,    0 },
    { NULL,      0,   0,             NULL,      0 }
  };
  poptContext con = poptGetContext(NULL, argc, argv, options, 0);
  int rc = 0;
  while ((rc = poptGetNextOpt(con)) > 0)
  {
    ulFlag = rc;
  }
  if (rc < -1)
  {
    cerr << poptBadOption(con, POPT_BADOPTION_NOALIAS) << ": "
         << poptStrerror(rc) << endl;
    
    cout << "Try `" << appname << " --help' for more information." << endl;
    ret = -rc;
  }
  else if (bHelp)
  {
    PrintVersion(appname);
    PrintUsage(appname);
  }
  else if (bVersion)
  {
    PrintVersion(appname);
  }
  else
  {
    const char* filename = NULL;
    while((filename = poptGetArg(con)) != NULL)
    {
      try
      {
        ID3_Tag myTag;
        
        if (bStrip)
        {
          cout << "Stripping ";
        }
        else
        {
          cout << "Converting ";
        }
        cout << filename << ": ";
        
        myTag.Clear();
        myTag.Link(filename, ID3TT_ALL);
        myTag.SetPadding(bPadding);
        
        cout << "attempting ";
        DisplayTags(cout, ulFlag);
        luint nTags;
        
        if (bStrip)
        {
          nTags = myTag.Strip(ulFlag);
          cout << ", stripped ";
        }
        else
        {
          nTags = myTag.Update(ulFlag);
          cout << ", converted ";
        }
        
        DisplayTags(cout, nTags);
        cout << endl;
      }
      
      catch(ID3_Error err)
      {
        cout << endl;
        cout << err.GetErrorFile() << " (" << err.GetErrorLine() << "): "
             << err.GetErrorType() << ": " << err.GetErrorDesc() << endl;
        ret = 1;
        break;
      }
    }
  }
  poptFreeContext(con);
  return ret;
}


