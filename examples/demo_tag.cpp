// $Id: demo_tag.cpp,v 1.8 2000/10/02 17:01:45 eldamitri Exp $

#include <iostream.h>
#include <popt.h>
#include <stdlib.h>

#include <id3/tag.h>
#include <id3/error.h>
#include <id3/misc_support.h>

static const char* VERSION_NUMBER = "$Revision: 1.8 $";

void PrintUsage(const char *sName)
{
  cout << "Usage: " << sName << " [OPTION]... [FILE]..." << endl;
  cout << "Tags an mp3 file with id3v1 and/or id3v2 tags." << endl;
  cout << endl;
  cout << "  -1, --v1tag              Render only the id3v1 tag" << endl;
  cout << "  -2, --v2tag              Render only the id3v2 tag" << endl;
  cout << "  -h, --help               Display this help and exit" << endl;
  cout << "  -v, --version            Display version information and exit" << endl;
  cout << "  -a, --artist  \"ARTIST\"   Set the artist information" << endl;
  cout << "  -s, --song    \"SONG\"     Set the song title information" << endl;
  cout << "  -A, --album   \"ALBUM\"    Set the album title information" << endl;
  cout << "  -c, --comment \"COMMENT\"  Set the comment infromation" << endl;
  cout << "  -C, --desc \"DESCRIPTION\" Set the comment description" << endl;
  cout << "  -g, --genre   num        Set the genre number" << endl;
  cout << "  -y, --year    num        Set the year" << endl;
  cout << "  -t, --track   num        Set the track number" << endl;
  cout << "  -T, --total   num        Set the total number of tracks on the album" << endl;
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
    if (VERSION_NUMBER[nIndex] == ' ') break;
  nIndex++;
  for (; nIndex < strlen (VERSION_NUMBER); nIndex++)
  {
    if (VERSION_NUMBER[nIndex] == ' ') break;
    cout << VERSION_NUMBER[nIndex];
  }
  cout << endl;
  cout << "Uses " << ID3LIB_FULL_NAME << endl << endl;

  cout << "This program tags mp3 files with ID3v1/1.1 and/or id3v2 tags" << endl;
}

void DisplayTags(ostream &os, luint nTags)
{
  if (!((nTags & ID3TT_ID3V1) || (nTags & ID3TT_ID3V2)))
    os << "no tag";
  if (nTags & ID3TT_ID3V1)
    os << "v1";
  if ((nTags & ID3TT_ID3V1) && (nTags & ID3TT_ID3V2))
    os << " and ";
  if (nTags & ID3TT_ID3V2)
    os << "v2";
}

int main( unsigned int argc, const char *argv[])
{
  const char* appname = argv[0];
  int ret = 0;
  int ulFlag = ID3TT_ID3;
  bool 
    bVersion = false,
    bHelp = false;
  const char
    *sArtist  = NULL,
    *sAlbum   = NULL,
    *sSong    = NULL,
    *sYear    = NULL,
    *sComment = NULL,
    *sDesc    = "";
  int
    nTrack    = 0,
    nTotal    = 0,
    nGenre    = 0xFF;
  static struct poptOption options[] =
  {
    { "v1tag",   '1', POPT_ARG_NONE,   NULL,      1 },
    { "v2tag",   '2', POPT_ARG_NONE,   NULL,      2 },
    { "artist",  'a', POPT_ARG_STRING, &sArtist,  0 },
    { "album",   'A', POPT_ARG_STRING, &sAlbum,   0 },
    { "song",    's', POPT_ARG_STRING, &sSong,    0 },
    { "genre",   'g', POPT_ARG_INT,    &nGenre,   0 },
    { "year",    'y', POPT_ARG_STRING, &sYear,    0 },
    { "comment", 'c', POPT_ARG_STRING, &sComment, 0 },
    { "desc",    'C', POPT_ARG_STRING, &sDesc,    0 },
    { "track",   't', POPT_ARG_INT,    &nTrack,   0 },
    { "total",   'T', POPT_ARG_STRING, &sComment, 0 },
    { "version", 'v', POPT_ARG_NONE,   &bVersion, 0 },
    { "help",    'h', POPT_ARG_NONE,   &bHelp,    0 },
    { NULL,      0,   0,               NULL,      0 }
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
    if (sArtist)
    {
      cout << "+++ Artist  = " << sArtist << endl;
    }
    if (sAlbum)
    {
      cout << "+++ Album   = " << sAlbum << endl;
    }
    if (sSong)
    {
      cout << "+++ Song    = " << sSong << endl;
    }
    if (sYear)
    {
      cout << "+++ Year    = " << sYear << endl;
    }
    if (sComment)
    {
      cout << "+++ Comment = " << sComment << endl;
      cout << "+++ Comment Description" << endl;
      cout << "            = " << sDesc << endl;
    }
    if (nGenre > 0 && nGenre < 0xFF)
    {
      cout << "+++ Genre   = " << nGenre << endl;
    }
    if (nTrack)
    {
      cout << "+++ Track   = " << nTrack;
    }
    if (nTotal)
    {
      cout << "+++ Total   = " << nTotal << endl;
    }
    const char* filename = NULL;
    while((filename = poptGetArg(con)) != NULL)
    {
      try
      {
        ID3_Tag myTag;
        
        cout << "Tagging " << filename << ": ";
        
        myTag.Link(filename);
        
        cout << "attempting ";
        DisplayTags(cout, ulFlag);
        
        if (sArtist)
        {
          ID3_AddArtist(&myTag, sArtist, true);
        }
        if (sAlbum)
        {
          ID3_AddAlbum(&myTag, sAlbum, true);
        }
        if (sSong)
        {
          ID3_AddTitle(&myTag, sSong, true);
        }
        if (sYear)
        {
          ID3_AddYear(&myTag, sYear, true);
        }
        if (sComment)
        {
          ID3_AddComment(&myTag, sComment, sDesc, true);
        }
        if (nGenre > 0)
        {
          ID3_AddGenre(&myTag, nGenre, true);
        }
        if (nTrack > 0)
        {
          ID3_AddTrack(&myTag, nTrack, nTotal, true);
        }
        luint nTags = myTag.Update(ulFlag);
        cout << ", tagged ";
        
        DisplayTags(cout, nTags);
        
        cout << endl;
      }
      
      catch(ID3_Error err)
      {
        cout << endl;
        cout << err.GetErrorFile() << " (" << err.GetErrorLine() << "): "
             << err.GetErrorType() << ": " << err.GetErrorDesc() << endl;
      }
    }
  }

  poptFreeContext(con);
  return ret;
}
