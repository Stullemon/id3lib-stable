// Copyright (C) 1999 Scott Thomas Haug <sth2@cs.wustl.edu>
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
// $Id: demo_info.cpp,v 1.1 1999/11/16 05:25:52 scott Exp $


#include <iostream.h>
#include <id3/tag.h>

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

char *GetText(const ID3_Frame *frame, const ID3_FieldID fldName)
{
  char *sText = NULL;
  if (NULL != frame)
  {
    size_t nText = frame->Field(fldName).Size();
    sText = new char[nText + 1];
    frame->Field(fldName).Get(sText, nText);
    sText[nText] = '\0';
  }
  return sText;
}

int main( int argc, char *argv[])
{
  if (argc > 1)
  {
    try
    {
      ID3_Tag myTag;
      luint argNum = 1;

      myTag.Link(argv[argNum]);

      for (size_t iFrames = 0; iFrames < myTag.NumFrames(); iFrames++)
      {
        ID3_Frame *myFrame = myTag[iFrames];
        if (NULL != myFrame)
        { 
          ID3_FrameID eFrameID = myFrame->GetID();
          cout << "*** " << GetDescription(eFrameID) << ": ";
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
              char *sText = GetText(myFrame, ID3FN_TEXT);
              cout << sText << endl;
              delete [] sText;
              break;
            }
            case ID3FID_USERTEXT:
            {
              char 
                *sText = GetText(myFrame, ID3FN_TEXT), 
                *sDesc = GetText(myFrame, ID3FN_DESCRIPTION);
              cout << "(" << sDesc << "): " << sText << endl;
              delete [] sText;
              delete [] sDesc;
              break;
            }
            case ID3FID_COMMENT:
            case ID3FID_UNSYNCEDLYRICS:
            {
              char 
                *sText = GetText(myFrame, ID3FN_TEXT), 
                *sDesc = GetText(myFrame, ID3FN_DESCRIPTION), 
                *sLang = GetText(myFrame, ID3FN_LANGUAGE);
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
              char *sURL = GetText(myFrame, ID3FN_URL);
              cout << sURL << endl;
              delete [] sURL;
              break;
            }
            case ID3FID_WWWUSER:
            {
              char 
                *sURL = GetText(myFrame, ID3FN_URL),
                *sDesc = GetText(myFrame, ID3FN_DESCRIPTION);
              cout << "(" << sDesc << "): " << sURL << endl;
              delete [] sURL;
              delete [] sDesc;
              break;
            }
            case ID3FID_INVOLVEDPEOPLE:
            {
              // This isn't the right way to do it---will only get first person
              char *sPeople = GetText(myFrame, ID3FN_TEXT);
              cout << sPeople << endl;
              delete [] sPeople;
              break;
            }
            case ID3FID_PICTURE:
            {
              char
                *sMimeType = GetText(myFrame, ID3FN_MIMETYPE),
                *sDesc     = GetText(myFrame, ID3FN_DESCRIPTION),
                *sFormat   = GetText(myFrame, ID3FN_IMAGEFORMAT);
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
                *sMimeType = GetText(myFrame, ID3FN_TEXT), 
                *sDesc = GetText(myFrame, ID3FN_DESCRIPTION), 
                *sFileName = GetText(myFrame, ID3FN_FILENAME);
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
              char *sOwner = GetText(myFrame, ID3FN_TEXT);
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
              char *sEmail = GetText(myFrame, ID3FN_EMAIL);
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
              char *sOwner = GetText(myFrame, ID3FN_OWNER);
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

    catch(ID3_Error err)
    {
      cout << err.GetErrorFile() << " (" << err.GetErrorLine() << "): "
           << err.GetErrorType() << ": " << err.GetErrorDesc() << endl;
    }
  }
  else
  {
    cout << "Usage: " << argv[0] << " <file>" << endl << endl;
  }

  return 0;
}

// $Log: demo_info.cpp,v $
// Revision 1.1  1999/11/16 05:25:52  scott
// Initial revision.
//
