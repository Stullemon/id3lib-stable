// $Id: tag_file.cpp,v 1.25 2000/10/03 04:51:27 eldamitri Exp $

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
#include <fstream.h>
#include "utils.h"

#if !defined HAVE_MKSTEMP
#  include <stdio.h>
#endif

#if defined WIN32
#  include <windows.h>
static int truncate(const char *path, size_t length)
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

// prevents a weird error I was getting compiling this under windows
#  if defined CreateFile
#    undef CreateFile
#  endif

#else
#  include <unistd.h>
#endif

#if defined HAVE_CONFIG_H
#  include <config.h>
#endif

#include "tag_impl.h"

using namespace dami;

size_t ID3_TagImpl::Link(const char *fileInfo, bool parseID3v1, bool parseLyrics3)
{
  flags_t tt = ID3TT_NONE;
  if (parseID3v1)
  {
    tt |= ID3TT_ID3V1;
  }
  if (parseLyrics3)
  {
    tt |= ID3TT_LYRICS;
  }
  return this->Link(fileInfo, tt);
}

size_t ID3_TagImpl::Link(const char *fileInfo, flags_t tag_types)
{
  _tags_to_parse.set(tag_types);
  
  if (NULL == fileInfo)
  {
    return 0;
  }

  strcpy(_file_name, fileInfo);
  _changed = true;

  this->ParseFile();
  
  return this->GetPrependedBytes();
}

size_t RenderV1ToFile(ID3_TagImpl& tag, fstream& file)
{
  if (!file)
  {
    return 0;
  }

  uchar sTag[ID3_V1_LEN];
  size_t tag_size = tag.Render(sTag, ID3TT_ID3V1);

  if (tag_size > tag.GetAppendedBytes())
  {
    file.seekp(0, ios::end);
  }
  else
  {
    // We want to check if there is already an id3v1 tag, so we can write over
    // it.  First, seek to the beginning of any possible id3v1 tag
    file.seekg(0-tag_size, ios::end);
    char sID[ID3_V1_LEN_ID];

    // Read in the TAG characters
    file.read(sID, ID3_V1_LEN_ID);

    // If those three characters are TAG, then there's a preexisting id3v1 tag,
    // so we should set the file cursor so we can overwrite it with a new tag.
    if (memcmp(sID, "TAG", ID3_V1_LEN_ID) == 0)
    {
      file.seekp(0-ID3_V1_LEN, ios::end);
    }
    // Otherwise, set the cursor to the end of the file so we can append on 
    // the new tag.
    else
    {
      file.seekp(0, ios::end);
    }
  }
  
  file.write(sTag, tag_size);

  return tag_size;
}

size_t RenderV2ToFile(const ID3_TagImpl& tag, fstream& file)
{
  uchar *buffer = NULL;
  
  if (!file)
  {
    return 0;
  }

  // Size() returns an over-estimate of the size needed for the tag
  size_t tag_size = 0;
  size_t size_est = tag.Size();
  if (size_est)
  {
    buffer = new uchar[size_est];
    tag_size = tag.Render(buffer, ID3TT_ID3V2);
    if (!tag_size)
    {
      delete [] buffer;
      buffer = NULL;
    }
  }
  

  // if the new tag fits perfectly within the old and the old one
  // actually existed (ie this isn't the first tag this file has had)
  if ((!tag.GetPrependedBytes() && !ID3_GetDataSize(tag)) ||
      (tag_size == tag.GetPrependedBytes()))
  {
    file.seekp(0, ios::beg);
    if (buffer)
    {
      file.write(buffer, tag_size);
    }
  }
  else
  {
#if !defined HAVE_MKSTEMP
    // This section is for Windows folk

    FILE *tempOut = tmpfile();
    if (NULL == tempOut)
    {
      // log this
      return 0;
      //ID3_THROW(ID3E_ReadOnly);
    }
    
    if (buffer)
    {
      fwrite(buffer, 1, tag_size, tempOut);
    }
    
    file.seekg(tag.GetPrependedBytes(), ios::beg);
    
    uchar buffer2[BUFSIZ];
    while (!file)
    {
      file.read(buffer2, BUFSIZ);
      size_t nBytes = file.gcount();
      fwrite(buffer2, 1, nBytes, tempOut);
    }
    
    rewind(tempOut);
    ::openWritableFile(tag.GetFileName(), file);
    
    while (!feof(tempOut))
    {
      size_t nBytes = fread(buffer2, 1, BUFSIZ, tempOut);
      file.write(buffer2, nBytes);
    }
    
    fclose(tempOut);
    
#else

    // else we gotta make a temp file, copy the tag into it, copy the
    // rest of the old file after the tag, delete the old file, rename
    // this new file to the old file's name and update the handle

    const char sTmpSuffix[] = ".XXXXXX";
    if (strlen(tag.GetFileName()) + strlen(sTmpSuffix) > ID3_PATH_LENGTH)
    {
      // log this
      return 0;
      //ID3_THROW_DESC(ID3E_NoFile, "filename too long");
    }
    char sTempFile[ID3_PATH_LENGTH];
    strcpy(sTempFile, tag.GetFileName());
    strcat(sTempFile, sTmpSuffix);
    
    int fd = mkstemp(sTempFile);
    if (fd < 0)
    {
      remove(sTempFile);
      ID3_THROW_DESC(ID3E_NoFile, "couldn't open temp file");
    }

    ofstream tmpOut(fd);
    if (!tmpOut)
    {
      tmpOut.close();
      remove(sTempFile);
      return 0;
      // log this
      //ID3_THROW(ID3E_ReadOnly);
    }
    if (buffer)
    {
      tmpOut.write(buffer, tag_size);
    }
    file.seekg(tag.GetPrependedBytes(), ios::beg);
    uchar buffer2[BUFSIZ];
    while (file)
    {
      file.read(buffer2, BUFSIZ);
      size_t nBytes = file.gcount();
      tmpOut.write(buffer2, nBytes);
    }
      
    tmpOut.close();

    file.close();

    remove(tag.GetFileName());
    rename(sTempFile, tag.GetFileName());

    ::openWritableFile(tag.GetFileName(), file);
#endif
  }

  if (buffer)
  {
    delete [] buffer;
  }

  return tag_size;
}


flags_t ID3_TagImpl::Update(flags_t ulTagFlag)
{
  flags_t tags = ID3TT_NONE;
  
  fstream file;
  ID3_Err err = ::openWritableFile(this->GetFileName(), file);
  _file_size = ::getFileSize(file);
  
  if (err == ID3E_NoFile)
  {
    err = ::createFile(this->GetFileName(), file);
  }
  if (err == ID3E_ReadOnly)
  {
    return tags;
  }

  if ((ulTagFlag & ID3TT_ID3V2) && this->HasChanged())
  {
    _prepended_bytes = RenderV2ToFile(*this, file);
    if (_prepended_bytes)
    {
      tags |= ID3TT_ID3V2;
    }
  }
  
  if ((ulTagFlag & ID3TT_ID3V1) && 
      (!this->HasTagType(ID3TT_ID3V1) || this->HasChanged()))
  {
    size_t tag_bytes = RenderV1ToFile(*this, file);
    if (tag_bytes)
    {
      // only add the tag_bytes if there wasn't an id3v1 tag before
      if (! _file_tags.test(ID3TT_ID3V1))
      {
        _appended_bytes += tag_bytes;
      }
      tags |= ID3TT_ID3V1;
    }
  }
  _changed = false;
  _file_tags.add(tags);
  _file_size = ::getFileSize(file);
  file.close();
  return tags;
}

flags_t ID3_TagImpl::Strip(flags_t ulTagFlag)
{
  flags_t ulTags = ID3TT_NONE;
  const size_t data_size = ID3_GetDataSize(*this);
  
  // First remove the v2 tag, if requested
  if (ulTagFlag & ID3TT_PREPENDED & _file_tags.get())
  {
    fstream file;
    if (ID3E_NoError != ::openWritableFile(this->GetFileName(), file))
    {
      return ulTags;
    }
    _file_size = ::getFileSize(file);

    // We will remove the id3v2 tag in place: since it comes at the beginning
    // of the file, we'll effectively move all the data that comes after the
    // tag back n bytes, where n is the size of the id3v2 tag.  Once we've
    // copied the data, we'll truncate the file.
    file.seekg(this->GetPrependedBytes(), ios::beg);
    
    uchar aucBuffer[BUFSIZ];
    
    // The nBytesRemaining variable indicates how many bytes are to be copied
    size_t nBytesToCopy = data_size;

    // Here we increase the nBytesToCopy by the size of any tags that appear
    // at the end of the file if we don't want to strip them
    if (!(ulTagFlag & ID3TT_APPENDED))
    {
      nBytesToCopy += this->GetAppendedBytes();
    }
    
    // The nBytesRemaining variable indicates how many bytes are left to be 
    // moved in the actual file.
    // The nBytesCopied variable keeps track of how many actual bytes were
    // copied (or moved) so far.
    size_t 
      nBytesRemaining = nBytesToCopy,
      nBytesCopied = 0;
    while (!file.eof())
    {
      size_t nBytesToRead = MIN(nBytesRemaining - nBytesCopied, BUFSIZ);
      file.read(aucBuffer, nBytesToRead);
      size_t nBytesRead = file.gcount();

      if (nBytesRead != nBytesToRead)
      {
        // TODO: log this
        //cerr << "--- attempted to write " << nBytesRead << " bytes, "
        //     << "only wrote " << nBytesWritten << endl;
      }
      if (nBytesRead > 0)
      {
        long offset = nBytesRead + this->GetPrependedBytes();
        file.seekp(-offset, ios::cur);
        file.write(aucBuffer, nBytesRead);
        file.seekg(this->GetPrependedBytes(), ios::cur);
        nBytesCopied += nBytesRead;
      }
      
      if (nBytesCopied == nBytesToCopy || nBytesToRead < BUFSIZ)
      {
        break;
      }
    }
    file.close();
  }
  
  size_t nNewFileSize = data_size;

  if ((_file_tags.get() & ID3TT_APPENDED) && (ulTagFlag & ID3TT_APPENDED))
  {
    ulTags |= _file_tags.get() & ID3TT_APPENDED;
  }
  else
  {
    // if we're not stripping the appended tags, be sure to increase the file
    // size by those bytes
    nNewFileSize += this->GetAppendedBytes();
  }
  
  if ((ulTagFlag & ID3TT_PREPENDED) && (_file_tags.get() & ID3TT_PREPENDED))
  {
    // If we're stripping the ID3v2 tag, there's no need to adjust the new
    // file size, since it doesn't account for the ID3v2 tag size
    ulTags |= _file_tags.get() & ID3TT_PREPENDED;
  }
  else
  {
    // add the original prepended tag size since we don't want to delete it,
    // and the new file size represents the file size _not_ counting the ID3v2
    // tag
    nNewFileSize += this->GetPrependedBytes();
  }

  if (ulTags && (truncate(_file_name, nNewFileSize) == -1))
  {
    // log this
    return 0;
    //ID3_THROW(ID3E_NoFile);
  }

  _prepended_bytes = (ulTags & ID3TT_PREPENDED) ? 0 : _prepended_bytes;
  _appended_bytes  = (ulTags & ID3TT_APPENDED)  ? 0 : _appended_bytes;
  _file_size = data_size + _prepended_bytes + _appended_bytes;
  
  _changed = _file_tags.remove(ulTags) || _changed;
  
  return ulTags;
}
