// $Id: tag_file.cpp,v 1.14 2000/04/05 05:21:15 eldamitri Exp $

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

#include <cstring>
#include <iostream>
#include <cstdio>
#include "tag.h"

#if defined WIN32
#include <windows.h>
static int truncate(const char *path, off_t length)
{
  int result = -1;
  HANDLE fh;
  
  fh = ::CreateFile(path,
                    GENERIC_WRITE | GENERIC_READ,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
  
  if(INVALID_HANDLE_VALUE != fh)
  {
    SetFilePointer(fh, length, NULL, FILE_BEGIN);
    SetEndOfFile(fh);
    CloseHandle(fh);
    result = 0;
  }
  
  return result;
}
#else
#include <unistd.h>
#endif

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

bool exists(const char *name)
{
  bool doesExist = false;
  FILE *in = NULL;
  
  if (NULL == name)
  {
    ID3_THROW(ID3E_NoData);
  }

  in = fopen(name, "rb");
  doesExist = (NULL != in);
  if (doesExist)
  {
    fclose(in);
  }
    
  return doesExist;
}

void ID3_Tag::CreateFile(void)
{
  CloseFile();

  // Create a new file
  __fFileHandle = fopen(__sFileName, "wb+");

  // Check to see if file could not be created
  if (NULL == __fFileHandle)
  {
    ID3_THROW(ID3E_ReadOnly);
  }

  // Determine the size of the file
  fseek(__fFileHandle, 0, SEEK_END);
  __ulFileSize = ftell(__fFileHandle);
  fseek(__fFileHandle, 0, SEEK_SET);
  
  return ;
}

void ID3_Tag::OpenFileForWriting(void)
{
  CloseFile();
  
  if (exists(__sFileName))
  {
    // Try to open the file for reading and writing.
    __fFileHandle = fopen(__sFileName, "rb+");
  }
  else
  {
    // No such file to open
    ID3_THROW(ID3E_NoFile);
  }

  // Check to see if file could not be opened for writing
  if (NULL == __fFileHandle)
  {
    ID3_THROW(ID3E_ReadOnly);
  }

  // Determine the size of the file
  fseek(__fFileHandle, 0, SEEK_END);
  __ulFileSize = ftell(__fFileHandle);
  fseek(__fFileHandle, 0, SEEK_SET);
  
  return ;
}

void ID3_Tag::OpenFileForReading(void)
{
  CloseFile();

  __fFileHandle = fopen(__sFileName, "rb");
  
  if (NULL == __fFileHandle)
  {
    ID3_THROW(ID3E_NoFile);
  }

  // Determine the size of the file
  fseek(__fFileHandle, 0, SEEK_END);
  __ulFileSize = ftell(__fFileHandle);
  fseek(__fFileHandle, 0, SEEK_SET);

  return ;
}

bool ID3_Tag::CloseFile(void)
{
  bool bReturn = ((NULL != __fFileHandle) && (0 == fclose(__fFileHandle)));
  if (bReturn)
  {
    __fFileHandle = NULL;
  }
  return bReturn;
}

luint ID3_Tag::Link(char *fileInfo)
{
  luint posn = 0;
  
  if (NULL == fileInfo)
  {
    ID3_THROW(ID3E_NoData);
  }

  strcpy(__sFileName, fileInfo);
    
  // if we were attached to some other file then abort
  if (__fFileHandle != NULL)
  {
    ID3_THROW(ID3E_TagAlreadyAttached);
  }

  OpenFileForReading();

  __ulOldTagSize = ParseFromHandle();

  CloseFile();
    
  if (__ulOldTagSize > 0)
  {
    __ulOldTagSize += ID3_TAGHEADERSIZE;
  }
      
  __ulFileSize -= __ulOldTagSize;
  posn = __ulOldTagSize;
    
  return posn;
}

luint ID3_Tag::Update(luint ulTagFlag)
{
  OpenFileForWriting();
  luint ulTags = NO_TAG;

  if ((ulTagFlag & V1_TAG) && (!__bHasV1Tag || HasChanged()))
  {
    RenderV1ToHandle();
    ulTags |= V1_TAG;
  }
  if ((ulTagFlag & V2_TAG) && HasChanged())
  {
    RenderV2ToHandle();
    ulTags |= V2_TAG;
  }

  //CloseFile();
    
  return ulTags;
}

luint ID3_Tag::Strip(const luint ulTagFlag)
{
  luint ulTags = NO_TAG;

  if (!(ulTagFlag & V1_TAG) && !(ulTagFlag & V2_TAG))
  {
    return ulTags;
  }

  // First remove the v2 tag, if requested
  if (ulTagFlag & V2_TAG)
  {
    OpenFileForWriting();

    // We will remove the id3v2 tag in place: since it comes at the beginning
    // of the file, we'll effectively move all the data that comes after the
    // tag back n bytes, where n is the size of the id3v2 tag.  Once we've
    // copied the data, we'll truncate the file.
    //
    // To copy the data, we'll need to keep two "pointers" in the file: one
    // will mark where to read from next, the other will indicate where to 
    // write to. 
    long nNextRead, nNextWrite;
    nNextWrite = ftell(__fFileHandle);
    // Set the read pointer past the tag
    fseek(__fFileHandle, __ulOldTagSize, SEEK_CUR);
    nNextRead = ftell(__fFileHandle);
    
    uchar aucBuffer[BUFSIZ];
    
    // The nBytesRemaining variable indicates how many bytes are to be copied
    size_t nBytesToCopy = __ulFileSize;

    // Here we reduce the nBytesToCopy by the size of any tags that appear
    // at the end of the file (e.g the id3v1 and lyrics tag).  This isn't
    // strictly necessary, since the truncation stage will remove these,
    // but this check prevents us from copying them unnecessarily.
    if ((__ulExtraBytes > 0) && (ulTagFlag & V1_TAG))
    {
      nBytesToCopy -= __ulExtraBytes;
    }
    
    // The nBytesRemaining variable indicates how many bytes are left to be 
    // moved in the actual file.
    // The nBytesCopied variable keeps track of how many actual bytes were
    // copied (or moved) so far.
    size_t 
      nBytesRemaining = nBytesToCopy,
      nBytesCopied = 0;
    while (! feof(__fFileHandle))
    {
      // Move to the next read position
      fseek(__fFileHandle, nNextRead, SEEK_SET);
      size_t
        nBytesToRead = MIN(nBytesRemaining - nBytesCopied, BUFSIZ),
        nBytesRead   = fread(aucBuffer, 1, nBytesToRead, __fFileHandle);
      // Now that we've read, mark the current spot as the next spot for
      // reading
      nNextRead = ftell(__fFileHandle);

      if (nBytesRead > 0)
      {
        // Move to the next write position
        fseek(__fFileHandle, nNextWrite, SEEK_SET);
        size_t nBytesWritten = fwrite(aucBuffer, 1, nBytesRead, __fFileHandle);
        if (nBytesRead > nBytesWritten)
        {
          cerr << "--- attempted to write " << nBytesRead << " bytes, "
               << "only wrote " << nBytesWritten << endl;
        }
        // Marke the current spot as the next write position
        nNextWrite = ftell(__fFileHandle);
        nBytesCopied += nBytesWritten;
      }


      if (nBytesCopied == nBytesToCopy)
      {
        break;
      }
      if (nBytesToRead < BUFSIZ)
      {
        break;
      }
    }

    CloseFile();
  }
  
  size_t nNewFileSize = __ulFileSize;
  if ((__ulExtraBytes > 0) && (ulTagFlag & V1_TAG))
  {
    nNewFileSize -= __ulExtraBytes;
    ulTags |= V1_TAG;
  }
  if (ulTagFlag & V2_TAG && (__ulOldTagSize > 0))
  {
    nNewFileSize -= __ulOldTagSize;
    ulTags |= V2_TAG;
  }
  if (ulTags && (truncate(__sFileName, nNewFileSize) == -1))
  {
    ID3_THROW(ID3E_NoFile);
  }

  __ulOldTagSize = (ulTags & V2_TAG) ? 0 : __ulOldTagSize;
  __ulExtraBytes = (ulTags & V1_TAG) ? 0 : __ulExtraBytes;
      
  __bHasV1Tag    = __bHasV1Tag && !(ulTagFlag & V1_TAG);
        
  __bHasChanged  = ((ulTags & V1_TAG) || (ulTags & V2_TAG));
  
  return ulTags;
}



// $Log: tag_file.cpp,v $
// Revision 1.14  2000/04/05 05:21:15  eldamitri
// Updated initial comment information to reflect license, copyright
// change.
//
// Revision 1.13  2000/01/04 15:42:49  eldamitri
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
// Revision 1.12  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.11  1999/12/13 04:44:23  scott
// (exists): Add const qualifier to parameter.
// (Link): Change to reflect new __sFileName type.
//
// Revision 1.10  1999/12/09 03:32:33  scott
// Minor code cleanup.
//
// Revision 1.9  1999/12/01 22:16:36  scott
// (truncate): Added.  Defined only for windows, which doesn't have
// unistd.h available (thanks elrod).
//
// Revision 1.8  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.7  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.6  1999/11/25 18:39:29  scott
// (OpenLinkedFile): Removed method.  Replaced with CreateFile,
// OpenFileForReading, and OpenFileForWriting.
// (CreateFile): New method.  Creates a new file with file name sFileName.
// Throws if unable to.
// (OpenFileForWriting): New method.  Opens a file with write permissions.
// Throws if unable to.
// (OpenFileForReading): New method.  Opens a file with read permissions.
// Throws if unable to.
// (CloseFile): New method.  Closes a file.  Returns true if successful,
// false otherwise.
// (Link): Now dynamically allocate sFileName.  Make use of new method
// OpenFileForReading rather than OpenLinkedFile.  Closes the file
// when done.
// (Update): Now returns a flag indicating which tags were actually
// written.  Calls OpenFileForWriting to open the file appropriately.
// Closes the file when done.
// (Strip): Now returns a flag indicating which tags were actually
// written.  Now strips a file "in place," by copying the non-stripped
// information to the beginning of the file and using ftruncate to chop
// off the rest.  Prevents having to create a temporary file.  Calls
// OpenFileForWriting to open the file appropriately.  Closes the file
// when done.
//
// Revision 1.5  1999/11/19 19:09:12  scott
// (Update): Changed parameter to be a flag which indicates which type of
// tag to update, either V1_TAG, V2_TAG, or BOTH_TAGS.  Updated method to
// act appropriately based on the parameter passed in.
// (Strip): Changed parameter to be a flag which indicates which type of
// tag to update, either V1_TAG, V2_TAG, or BOTH_TAGS.  Updated method to
// act appropriately based on the parameter passed in.
//
// Revision 1.4  1999/11/15 20:20:37  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.  OpenLinkedFile now checks to see if the file is
// writable or not, and still opens the file, even if it can't be
// written to.  This needs to be more robust (i.e., we shouldn't be
// creating temp files if the file isn't writable).  Made use of
// bFileWrtiable variable, which was introduced.
//
// Revision 1.3  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
