// $Id: error.cpp,v 1.13 2000/04/08 04:34:37 eldamitri Exp $

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug

// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where to
// send such submissions.  See the AUTHORS file for a list of people who have
// contributed to id3lib.  See the ChangeLog file for a list of changes to
// id3lib.  These files are distributed with id3lib at
// http://download.sourceforge.net/id3lib/


#include <string.h>
#include "error.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

static char *ID3_ErrorDescs[] =
{
  "out of memory",
  "no source/dest data specified",
  "improperly formatted id3 tag",
  "no buffer specified",
  "buffer is insufficient in size",
  "invalid frame id",
  "field not found",
  "unknown field type",
  "tag is already attached to a file",
  "invalid tag version",
  "file not found",
  "trying to write to a read-only file",
  "error in zlib compression library"
};
  
ID3_Error::ID3_Error(const ID3_Err eCode, const char *sFileName, 
                     const luint nLine, const char *sDesc)
{
  __eError = eCode;
  __nErrLine = nLine;
  luint nFileNameLen = strlen(sFileName);
  luint nDescLen = strlen(sDesc);
  
  __sErrFileName = new char[nFileNameLen+1];
  __sErrDesc     = new char[nDescLen+1];
  strncpy(__sErrFileName, sFileName, nFileNameLen+1);
  strncpy(__sErrDesc, sDesc, nDescLen+1);
}


ID3_Err ID3_Error::GetErrorID(void) const
{
  return __eError;
}


char *ID3_Error::GetErrorType(void) const
{
  return ID3_ErrorDescs[__eError];
}

char *ID3_Error::GetErrorDesc(void) const
{
  return __sErrDesc;
}

char *ID3_Error::GetErrorFile(void) const
{
  return __sErrFileName;
}


luint ID3_Error::GetErrorLine(void) const
{
  return __nErrLine;
}



// $Log: error.cpp,v $
// Revision 1.13  2000/04/08 04:34:37  eldamitri
// Changed new ANSI-standard C++ include headers to old-style headers.
//
// Revision 1.12  2000/04/05 05:21:15  eldamitri
// Updated initial comment information to reflect license, copyright
// change.
//
// Revision 1.11  2000/01/04 15:42:49  eldamitri
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
// Revision 1.10  1999/12/17 16:13:03  scott
// Updated opening comment block.
//
// Revision 1.9  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.8  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.7  1999/11/25 18:17:22  scott
// (ID3_ErrorDescs): Added descriptions for new error types.
// (ID3_Error): Minor bugfix in string copying.
//
// Revision 1.6  1999/11/16 22:50:10  scott
// * error.cpp (ID3_ErrorDescs): Added text entry for ID3E_BadData.
//
// Revision 1.5  1999/11/15 20:15:03  scott
// Added include for config.h.  Added new interface to error
// reporting to allow for more descriptive error messages (this
// should still be worked on).  Made private member variable names
// more descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
