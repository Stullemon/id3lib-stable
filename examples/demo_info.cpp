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
// $Id: demo_info.cpp,v 1.3 2000/04/20 03:27:47 eldamitri Exp $


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
  cout << "Uses " << ID3LIB_FULL_NAME << endl << endl;
}

char *GetDescription(const ID3_FrameID eFrameID)
{
  switch (eFrameID)
  {
    /* AENC */ case ID3FID_AUDIOCRYPTO:       return "Audio encryption";
    /* APIC */ case ID3FID_PICTURE:           return "Attached picture";
    /* COMM */ case ID3FID_COMMENT:           return "Comments";
    /* COMR */
    /* ENCR */ case ID3FID_CRYPTOREG:         return "Encryption method registration";
    /* EQUA */ case ID3FID_EQUALIZATION:      return "Equalization";
    /* ETCO */ case ID3FID_EVENTTIMING:       return "Event timing codes";
    /* GEOB */ case ID3FID_GENERALOBJECT:     return "General encapsulated object";
    /* GRID */ case ID3FID_GROUPINGREG:       return "Group identification registration";
    /* IPLS */ case ID3FID_INVOLVEDPEOPLE:    return "Involved people list";
    /* LINK */ case ID3FID_LINKEDINFO:        return "Linked information";
    /* MCDI */ case ID3FID_CDID:              return "Music CD identifier";
    /* MLLT */ case ID3FID_MPEGLOOKUP:        return "MPEG location lookup table";
    /* OWNE */ case ID3FID_OWNERSHIP:         return "Ownership frame";
    /* PRIV */ case ID3FID_PRIVATE:           return "Private frame";
    /* PCNT */ case ID3FID_PLAYCOUNTER:       return "Play counter";
    /* POPM */ case ID3FID_POPULARIMETER:     return "Popularimeter";
    /* POSS */ case ID3FID_POSITIONSYNC:      return "Position synchronisation frame";
    /* RBUF */ case ID3FID_BUFFERSIZE:        return "Recommended buffer size";
    /* RVAD */ case ID3FID_VOLUMEADJ:         return "Relative volume adjustment";
    /* RVRB */ case ID3FID_REVERB:            return "Reverb";
    /* SYLT */ case ID3FID_SYNCEDLYRICS:      return "Synchronized lyric/text";
    /* SYTC */ case ID3FID_SYNCEDTEMPO:       return "Synchronized tempo codes";
    /* TALB */ case ID3FID_ALBUM:             return "Album/Movie/Show title";
    /* TBPM */ case ID3FID_BPM:               return "BPM (beats per minute)";
    /* TCOM */ case ID3FID_COMPOSER:          return "Composer";
    /* TCON */ case ID3FID_CONTENTTYPE:       return "Content type";
    /* TCOP */ case ID3FID_COPYRIGHT:         return "Copyright message";
    /* TDAT */ case ID3FID_DATE:              return "Date";
    /* TDLY */ case ID3FID_PLAYLISTDELAY:     return "Playlist delay";
    /* TENC */ case ID3FID_ENCODEDBY:         return "Encoded by";
    /* TEXT */ case ID3FID_LYRICIST:          return "Lyricist/Text writer";
    /* TFLT */ case ID3FID_FILETYPE:          return "File type";
    /* TIME */ case ID3FID_TIME:              return "Time";
    /* TIT1 */ case ID3FID_CONTENTGROUP:      return "Content group description";
    /* TIT2 */ case ID3FID_TITLE:             return "Title/songname/content description";
    /* TIT3 */ case ID3FID_SUBTITLE:          return "Subtitle/Description refinement";
    /* TKEY */ case ID3FID_INITIALKEY:        return "Initial key";
    /* TLAN */ case ID3FID_LANGUAGE:          return "Language(s)";
    /* TLEN */ case ID3FID_SONGLEN:           return "Length";
    /* TMED */ case ID3FID_MEDIATYPE:         return "Media type";
    /* TOAL */ case ID3FID_ORIGALBUM:         return "Original album/movie/show title";
    /* TOFN */ case ID3FID_ORIGFILENAME:      return "Original filename";
    /* TOLY */ case ID3FID_ORIGLYRICIST:      return "Original lyricist(s)/text writer(s)";
    /* TOPE */ case ID3FID_ORIGARTIST:        return "Original artist(s)/performer(s)";
    /* TORY */ case ID3FID_ORIGYEAR:          return "Original release year";
    /* TOWN */ case ID3FID_FILEOWNER:         return "File owner/licensee";
    /* TPE1 */ case ID3FID_LEADARTIST:        return "Lead performer(s)/Soloist(s)";
    /* TPE2 */ case ID3FID_BAND:              return "Band/orchestra/accompaniment";
    /* TPE3 */ case ID3FID_CONDUCTOR:         return "Conductor/performer refinement";
    /* TPE4 */ case ID3FID_MIXARTIST:         return "Interpreted, remixed, or otherwise modified by";
    /* TPOS */ case ID3FID_PARTINSET:         return "Part of a set";
    /* TPUB */ case ID3FID_PUBLISHER:         return "Publisher";
    /* TRCK */ case ID3FID_TRACKNUM:          return "Track number/Position in set";
    /* TRDA */ case ID3FID_RECORDINGDATES:    return "Recording dates";
    /* TRSN */ case ID3FID_NETRADIOSTATION:   return "Internet radio station name";
    /* TRSO */ case ID3FID_NETRADIOOWNER:     return "Internet radio station owner";
    /* TSIZ */ case ID3FID_SIZE:              return "Size";
    /* TSRC */ case ID3FID_ISRC:              return "ISRC (international standard recording code)";
    /* TSSE */ case ID3FID_ENCODERSETTINGS:   return "Software/Hardware and settings used for encoding";
    /* TXXX */ case ID3FID_USERTEXT:          return "User defined text information";
    /* TYER */ case ID3FID_YEAR:              return "Year";
    /* UFID */ case ID3FID_UNIQUEFILEID:      return "Unique file identifier";
    /* USER */ case ID3FID_TERMSOFUSE:        return "Terms of use";
    /* USLT */ case ID3FID_UNSYNCEDLYRICS:    return "Unsynchronized lyric/text transcription";
    /* WCOM */ case ID3FID_WWWCOMMERCIALINFO: return "Commercial information";
    /* WCOP */ case ID3FID_WWWCOPYRIGHT:      return "Copyright/Legal infromation";
    /* WOAF */ case ID3FID_WWWAUDIOFILE:      return "Official audio file webpage";
    /* WOAR */ case ID3FID_WWWARTIST:         return "Official artist/performer webpage";
    /* WOAS */ case ID3FID_WWWAUDIOSOURCE:    return "Official audio source webpage";
    /* WORS */ case ID3FID_WWWRADIOPAGE:      return "Official internet radio station homepage";
    /* WPAY */ case ID3FID_WWWPAYMENT:        return "Payment";
    /* WPUB */ case ID3FID_WWWPUBLISHER:      return "Official publisher webpage";
    /* WXXX */ case ID3FID_WWWUSER:           return "User defined URL link";
    /*      */ case ID3FID_METACRYPTO:        return "Encrypted meta frame";
    /* ???? */ default:                       return "Unknown";
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
        case ID3FID_ALBUM:
        case ID3FID_BPM:
        case ID3FID_COMPOSER:
        case ID3FID_CONTENTTYPE:
        case ID3FID_COPYRIGHT:
        case ID3FID_DATE:
        case ID3FID_PLAYLISTDELAY:
        case ID3FID_ENCODEDBY:
        case ID3FID_LYRICIST:
        case ID3FID_FILETYPE:
        case ID3FID_TIME:
        case ID3FID_CONTENTGROUP:
        case ID3FID_TITLE:
        case ID3FID_SUBTITLE:
        case ID3FID_INITIALKEY:
        case ID3FID_LANGUAGE:
        case ID3FID_SONGLEN:
        case ID3FID_MEDIATYPE:
        case ID3FID_ORIGALBUM:
        case ID3FID_ORIGFILENAME:
        case ID3FID_ORIGLYRICIST:
        case ID3FID_ORIGARTIST:
        case ID3FID_ORIGYEAR:
        case ID3FID_FILEOWNER:
        case ID3FID_LEADARTIST:
        case ID3FID_BAND:
        case ID3FID_CONDUCTOR:
        case ID3FID_MIXARTIST:
        case ID3FID_PARTINSET:
        case ID3FID_PUBLISHER:
        case ID3FID_TRACKNUM:
        case ID3FID_RECORDINGDATES:
        case ID3FID_NETRADIOSTATION:
        case ID3FID_NETRADIOOWNER:
        case ID3FID_SIZE:
        case ID3FID_ISRC:
        case ID3FID_ENCODERSETTINGS:
        case ID3FID_YEAR:
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
            nSymbol = myFrame->Field(ID3FN_ID).Get(),
            nDataSize = myFrame->Field(ID3FN_DATA).Size();
          cout << "(" << nSymbol << "): " << sOwner
               << ", " << nDataSize << " bytes";
          break;
        }
        case ID3FID_AUDIOCRYPTO:
        case ID3FID_EQUALIZATION:
        case ID3FID_EVENTTIMING:
        case ID3FID_CDID:
        case ID3FID_MPEGLOOKUP:
        case ID3FID_OWNERSHIP:
        case ID3FID_PRIVATE:
        case ID3FID_POSITIONSYNC:
        case ID3FID_BUFFERSIZE:
        case ID3FID_VOLUMEADJ:
        case ID3FID_REVERB:
        case ID3FID_SYNCEDLYRICS:
        case ID3FID_SYNCEDTEMPO:
        case ID3FID_METACRYPTO:
        {
          cout << " (unimplemented)" << endl;
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
    {
      break;
    }

    if (iOpt == 0)
    {
      iOpt = iLongOpt;
    }

    switch (iOpt)
    {
      case 'v':
      {
        PrintVersion(argv[0]);
        exit (0);
        break;
      }
      case 'h':
      {
        PrintUsage(argv[0]);
        exit (0);
        break;
      }
      case 'a':
      {
        bTestAssignment = true;
        break;
      }
      case '?':
      {
        bError = true;
        break;
      }
      default:
      {
        cout << "?? getopt returned character code " << iOpt << " ??" << endl;
      }
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
// Revision 1.3  2000/04/20 03:27:47  eldamitri
// Replaced call to ID3_FullName with ID3LIB_FULL_NAME constant
//
// Revision 1.2  2000/04/18 21:45:39  eldamitri
// Moved demo_info.cpp from src/examples to examples
//
// Revision 1.7  2000/04/15 18:44:54  eldamitri
// (PrintInformation): Renamed ID3FN_SYMBOL to ID3FN_ID
//
// Revision 1.6  1999/12/26 00:29:55  scott
// (GetDescription): Reformatted.  Descriptions now match those from
// www.id3.org.  Added descriptions for new frame id's.
// (PrintInformation): Updated for added frames.  Now handles
// unimplemented frames.
//
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
