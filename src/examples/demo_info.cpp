// Copyright 1999 Scott Thomas Haug <sth2@cs.wustl.edu>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  
// $Id: demo_info.cpp,v 1.5 1999/12/23 15:56:26 scott Exp $


#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <id3/tag.h>
#include <id3/misc_support.h>
#include <getopt.h>

void PrintUsage(char *sName)
{
  cout << "Usage: " << sName << " [OPTION]... [FILE]..." << endl;
  cout << "Display the id3 (both v1 and v2) tag information for a file." << endl;
  cout << endl;
  cout << "  -h, --help      Display this help and exit" << endl;
  cout << "  -v, --version   Display version information and exit" << endl;
  cout << endl;
  cout << "Will not differentiate between the two types of tags" << endl;
}

void PrintVersion(char *sName)
{
  cout << sName << " 1.0" << endl;
  cout << "Displays ID3 Tag Information - Written by Scott Thomas Haug" << endl;
  cout << "Uses " << ID3_FullName() << endl << endl;
}

char *GetDescription(const ID3_FrameID eFrameID)
{
  switch (eFrameID)
  {
    case ID3FID_ENCODEDBY:              return "Encoded by";
    case ID3FID_ORIGALBUM:              return "Original album";
    case ID3FID_PUBLISHER:              return "Publisher";
    case ID3FID_ENCODERSETTINGS:        return "Encoder settings";
    case ID3FID_ORIGFILENAME:           return "Original filename";
    case ID3FID_LANGUAGE:               return "Language";
    case ID3FID_PARTINSET:              return "";
    case ID3FID_DATE:                   return "Date";
    case ID3FID_TIME:                   return "Time";
    case ID3FID_RECORDINGDATES:         return "Recording dates";
    case ID3FID_MEDIATYPE:              return "Media type";
    case ID3FID_FILETYPE:               return "File type";
    case ID3FID_NETRADIOSTATION:        return "Net radio station";
    case ID3FID_NETRADIOOWNER:          return "Net radio owner";
    case ID3FID_LYRICIST:               return "Lyricist";
    case ID3FID_ORIGARTIST:             return "Original artist";
    case ID3FID_ORIGLYRICIST:           return "Original lyricist";
    case ID3FID_CONTENTGROUP:           return "Content group";
    case ID3FID_TITLE:                  return "Title";
    case ID3FID_SUBTITLE:               return "Subtitle";
    case ID3FID_LEADARTIST:             return "Lead artist";
    case ID3FID_BAND:                   return "Band";
    case ID3FID_CONDUCTOR:              return "Conductor";
    case ID3FID_MIXARTIST:              return "Mix artist";
    case ID3FID_ALBUM:                  return "Album";
    case ID3FID_YEAR:                   return "Year";
    case ID3FID_COMPOSER:               return "Composer";
    case ID3FID_COPYRIGHT:              return "Copyright";
    case ID3FID_CONTENTTYPE:            return "Content type";
    case ID3FID_TRACKNUM:               return "Track number";
    case ID3FID_USERTEXT:               return "User text";
    case ID3FID_COMMENT:                return "Comment";
    case ID3FID_UNSYNCEDLYRICS:         return "Lyrics (unsynced)";
    case ID3FID_WWWAUDIOFILE:           return "Audiofile URL";
    case ID3FID_WWWARTIST:              return "Artist's URL";
    case ID3FID_WWWAUDIOSOURCE:         return "Audio source URL";
    case ID3FID_WWWCOMMERCIALINFO:      return "Commercial info URL";
    case ID3FID_WWWCOPYRIGHT:           return "Copyright URL";
    case ID3FID_WWWPUBLISHER:           return "Publisher URL";
    case ID3FID_WWWPAYMENT:             return "Payment URL";
    case ID3FID_WWWRADIOPAGE:           return "Radio page URL";
    case ID3FID_WWWUSER:                return "User URL";
    case ID3FID_INVOLVEDPEOPLE:         return "Involved people";
    case ID3FID_PICTURE:                return "Picture";
    case ID3FID_GENERALOBJECT:          return "General object";
    case ID3FID_UNIQUEFILEID:           return "Unique file ID";
    case ID3FID_PLAYCOUNTER:            return "Play counter";
    case ID3FID_POPULARIMETER:          return "Popularimeter";
    case ID3FID_CRYPTOREG:              return "Crypto registration";
    case ID3FID_GROUPINGREG:            return "Grouping registration";
    default:                            return "Unknown";
  }
}

void PrintInformation(ID3_Tag &myTag)
{
  for (size_t nFrames = 0; nFrames < myTag.NumFrames(); nFrames++)
  {
    ID3_Frame *myFrame = myTag[nFrames];
    if (NULL != myFrame)
    { 
      ID3_FrameID eFrameID = myFrame->GetID();
      cout << "=== " << GetDescription(eFrameID) << ": ";
      switch (eFrameID)
      {
        case ID3FID_ENCODEDBY:
        case ID3FID_ORIGALBUM:
        case ID3FID_PUBLISHER:
        case ID3FID_ENCODERSETTINGS:
        case ID3FID_ORIGFILENAME:
        case ID3FID_LANGUAGE:
        case ID3FID_PARTINSET:
        case ID3FID_DATE:
        case ID3FID_TIME:
        case ID3FID_RECORDINGDATES:
        case ID3FID_MEDIATYPE:
        case ID3FID_FILETYPE:
        case ID3FID_NETRADIOSTATION:
        case ID3FID_NETRADIOOWNER:
        case ID3FID_LYRICIST:
        case ID3FID_ORIGARTIST:
        case ID3FID_ORIGLYRICIST:
        case ID3FID_CONTENTGROUP:
        case ID3FID_TITLE:
        case ID3FID_SUBTITLE:
        case ID3FID_LEADARTIST:
        case ID3FID_BAND:
        case ID3FID_CONDUCTOR:
        case ID3FID_MIXARTIST:
        case ID3FID_ALBUM:
        case ID3FID_YEAR:
        case ID3FID_COMPOSER:
        case ID3FID_COPYRIGHT:
        case ID3FID_CONTENTTYPE:
        case ID3FID_TRACKNUM:
        {
          char *sText = ID3_GetString(myFrame, ID3FN_TEXT);
          cout << sText << endl;
          delete [] sText;
          break;
        }
        case ID3FID_USERTEXT:
        {
          char 
            *sText = ID3_GetString(myFrame, ID3FN_TEXT), 
            *sDesc = ID3_GetString(myFrame, ID3FN_DESCRIPTION);
          cout << "(" << sDesc << "): " << sText << endl;
          delete [] sText;
          delete [] sDesc;
          break;
        }
        case ID3FID_COMMENT:
        case ID3FID_UNSYNCEDLYRICS:
        {
          char 
            *sText = ID3_GetString(myFrame, ID3FN_TEXT), 
            *sDesc = ID3_GetString(myFrame, ID3FN_DESCRIPTION), 
            *sLang = ID3_GetString(myFrame, ID3FN_LANGUAGE);
          cout << "(" << sDesc << ")[" << sLang << "]: "
               << sText << endl;
          delete [] sText;
          delete [] sDesc;
          delete [] sLang;
          break;
        }
        case ID3FID_WWWAUDIOFILE:
        case ID3FID_WWWARTIST:
        case ID3FID_WWWAUDIOSOURCE:
        case ID3FID_WWWCOMMERCIALINFO:
        case ID3FID_WWWCOPYRIGHT:
        case ID3FID_WWWPUBLISHER:
        case ID3FID_WWWPAYMENT:
        case ID3FID_WWWRADIOPAGE:
        {
          char *sURL = ID3_GetString(myFrame, ID3FN_URL);
          cout << sURL << endl;
          delete [] sURL;
          break;
        }
        case ID3FID_WWWUSER:
        {
          char 
            *sURL = ID3_GetString(myFrame, ID3FN_URL),
            *sDesc = ID3_GetString(myFrame, ID3FN_DESCRIPTION);
          cout << "(" << sDesc << "): " << sURL << endl;
          delete [] sURL;
          delete [] sDesc;
          break;
        }
        case ID3FID_INVOLVEDPEOPLE:
        {
          // This isn't the right way to do it---will only get first person
          size_t nItems = myFrame->Field(ID3FN_TEXT).GetNumTextItems();
          for (size_t nIndex = 1; nIndex <= nItems; nIndex++)
          {
            char *sPeople = ID3_GetString(myFrame, ID3FN_TEXT, nIndex);
            cout << sPeople;
            delete [] sPeople;
            if (nIndex < nItems)
            {
              cout << ", ";
            }
          }
          cout << endl;
          break;
        }
        case ID3FID_PICTURE:
        {
          char
            *sMimeType = ID3_GetString(myFrame, ID3FN_MIMETYPE),
            *sDesc     = ID3_GetString(myFrame, ID3FN_DESCRIPTION),
            *sFormat   = ID3_GetString(myFrame, ID3FN_IMAGEFORMAT);
          size_t
            nPicType   = myFrame->Field(ID3FN_PICTURETYPE).Get(),
            nDataSize  = myFrame->Field(ID3FN_DATA).Size();
          cout << "(" << sDesc << ")[" << sFormat << ", "
               << nPicType << "]: " << sMimeType << ", " << nDataSize
               << " bytes" << endl;
          delete [] sMimeType;
          delete [] sDesc;
          delete [] sFormat;
          break;
        }
        case ID3FID_GENERALOBJECT:
        {
          char 
            *sMimeType = ID3_GetString(myFrame, ID3FN_TEXT), 
            *sDesc = ID3_GetString(myFrame, ID3FN_DESCRIPTION), 
            *sFileName = ID3_GetString(myFrame, ID3FN_FILENAME);
          size_t 
            nDataSize = myFrame->Field(ID3FN_DATA).Size();
          cout << "(" << sDesc << ")[" 
               << sFileName << "]: " << sMimeType << ", " << nDataSize
               << " bytes" << endl;
          delete [] sMimeType;
          delete [] sDesc;
          delete [] sFileName;
          break;
        }
        case ID3FID_UNIQUEFILEID:
        {
          char *sOwner = ID3_GetString(myFrame, ID3FN_TEXT);
          size_t nDataSize = myFrame->Field(ID3FN_DATA).Size();
          cout << sOwner << ", " << nDataSize
               << " bytes" << endl;
          delete [] sOwner;
          break;
        }
        case ID3FID_PLAYCOUNTER:
        {
          size_t nCounter = myFrame->Field(ID3FN_COUNTER).Get();
          cout << nCounter << endl;
          break;
        }
        case ID3FID_POPULARIMETER:
        {
          char *sEmail = ID3_GetString(myFrame, ID3FN_EMAIL);
          size_t
            nCounter = myFrame->Field(ID3FN_COUNTER).Get(),
            nRating = myFrame->Field(ID3FN_RATING).Get();
          cout << sEmail << ", counter=" 
               << nCounter << " rating=" << nRating;
          delete [] sEmail;
          break;
        }
        case ID3FID_CRYPTOREG:
        case ID3FID_GROUPINGREG:
        {
          char *sOwner = ID3_GetString(myFrame, ID3FN_OWNER);
          size_t 
            nSymbol = myFrame->Field(ID3FN_SYMBOL).Get(),
            nDataSize = myFrame->Field(ID3FN_DATA).Size();
          cout << "(" << nSymbol << "): " << sOwner
               << ", " << nDataSize << " bytes";

          break;
        }
        default:
        {
          cout << " frame" << endl;
          break;
        }
      }
    }
  }
}

int main( int argc, char *argv[])
{

  int iOpt;
  bool bError = false, bTestAssignment = false;
  while (true)
  {
    int option_index = 0;
    int iLongOpt = 0;
    static struct option long_options[] = 
    { 
      { "version", no_argument, &iLongOpt, 'v' },
      { "help",    no_argument, &iLongOpt, 'h' },
      { 0, 0, 0, 0 }
    };
    iOpt = getopt_long (argc, argv, "vha", long_options, &option_index);

    if (iOpt == -1)
      break;

    if (iOpt == 0) iOpt = iLongOpt;

    switch (iOpt)
    {
      case 'v':
        PrintVersion(argv[0]);
        exit (0);
        break;
        

      case 'h':
        PrintUsage(argv[0]);
        exit (0);
        break;

      case 'a':
        bTestAssignment = true;
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
  {
    for (size_t nIndex = optind; nIndex < argc; nIndex++)
    {
      try
      {
        ID3_Tag myTag;

        myTag.Link(argv[nIndex]);
        cout << endl << "*** Tag information for " << argv[nIndex] << endl;
        if (!bTestAssignment)
        {
          PrintInformation(myTag);
        }
        else
        {
          cout << "*** Testing assignment operator" << endl;
          ID3_Tag tmpTag(myTag);
          PrintInformation(tmpTag);
        }
      }
      catch(ID3_Error &err)
      {
        cout << err.GetErrorFile() << " (" << err.GetErrorLine() << "): "
             << err.GetErrorType() << ": " << err.GetErrorDesc() << endl;
      }
    }
  }

  return 0;
}

// $Log: demo_info.cpp,v $
// Revision 1.5  1999/12/23 15:56:26  scott
// Added check and include for config.h.
//
// Revision 1.4  1999/12/09 02:48:54  scott
// (PrintInformation): Separated out information printing code into this
// separate function.
// (main): Added new runtime option -a that will test the ability of
// id3lib to make copies of tags.  Not very useful in general, but nice
// for testing the new operator= code.
//
// Revision 1.3  1999/11/30 20:05:30  scott
// Updated to reflect changes from version macros to version functions.
//
// Revision 1.2  1999/11/19 17:09:02  scott
// * demo_info.cpp: Added runtime options for version and help.  Minor
// code cleanup.
//
// Revision 1.1  1999/11/16 05:25:52  scott
// Initial revision.
//
