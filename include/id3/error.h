// $Id: error.h,v 1.7 2000/04/20 03:43:09 eldamitri Exp $

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

#ifndef __ID3LIB_ERROR_H__
#define __ID3LIB_ERROR_H__

#include "globals.h"

/**
 ** Predefined id3lib error types.
 **/
enum ID3_Err
{
  /// No available memory
  ID3E_NoMemory = 0,
  /// No data to parse
  ID3E_NoData,
  /// Improperly formatted data
  ID3E_BadData,
  /// No buffer to write to
  ID3E_NoBuffer,
  /// Buffer is too small
  ID3E_SmallBuffer,
  /// Invalid frame id
  ID3E_InvalidFrameID,
  /// Requested field not found
  ID3E_FieldNotFound,
  /// Unknown field type
  ID3E_UnknownFieldType,
  /// Tag is already attached to a file
  ID3E_TagAlreadyAttached,
  /// Invalid tag version
  ID3E_InvalidTagVersion,
  /// No file to parse
  ID3E_NoFile,
  /// Attempting to write to a read-only file
  ID3E_ReadOnly,
  /// Error in compression/uncompression
  ID3E_zlibError
};

/** When id3lib encounters a nasty error, it thros an exception of type
 ** ID3_Error.  A function that calls an id3lib routine can place the call in a
 ** try block and provide an appropriate catch block.
 ** 
 ** <pre>
 ** try
 ** {
 **   // call some id3lib routine
 **   ID3_Tag myTag("mySong.mp3");
 **   ID3_Frame *myFrame = NULL;
 **   
 **   // this will generate an exception
 **   myTag << myFrame;
 ** }
 ** catch (ID3_Error err)
 ** {
 **   // handle the error
 **   ...
 ** }</pre>
 **/
class ID3_Error
{
public:
  /** Returns the ID3_Err value, which represents the ID of the error
   ** that caused the exception.
   **/
  ID3_Err GetErrorID(void) const;
  /** Returns the English string that defines the error type.
   ** 
   ** Each error ID has a set string error type.
   **/
  char   *GetErrorType(void) const;
  /** Returns a string that gives more explanation as to what caused the
   ** exception, if enabled by the code that caused the exception.
   **/
  char   *GetErrorDesc(void) const;
  /** Returns a pointer to a string of characters that is the name
   ** of the id3lib source file that generated the exception.
   ** 
   ** When submitting bug reports, it is useful to include the following.
   ** 
   ** <pre>
   ** cout << "Exception in file '" << err.GetErrorFile() << "'" << endl;</pre>
   **/
  char   *GetErrorFile(void) const;
  /** Returns the line number in the id3lib source file that threw the
   ** exception.
   ** 
   ** <pre>cout << "Line #" << err.GetErrorLine() << endl;</pre>
   **/
  luint   GetErrorLine(void) const;
  
  /** Constructor
   ** 
   ** @param eID          Erroy id
   ** @param sFileName    Filename where error occurred
   ** @param nLineNum     Linenumber where error occurred
   ** @param sDescription Description of error
   **/
  ID3_Error(const ID3_Err eID, const char *sFileName, const luint nLineNum, 
            const char *sDescription);
private:
  ID3_Err __eError;
  luint   __nErrLine;
  char   *__sErrFileName;
  char   *__sErrDesc;
};

/** Shortcut macro for throwing an error without a description
 ** 
 ** @param x The error id
 **/
#define ID3_THROW(x) throw ID3_Error(x, __FILE__, __LINE__, "")
/** Shortcut macro for throwing an error with a description
 ** 
 ** @param x The error id
 ** @param y The error description
 **/
#define ID3_THROW_DESC(x, y) throw ID3_Error(x, __FILE__, __LINE__, y)

#endif /* __ID3LIB_ERROR_H__ */
