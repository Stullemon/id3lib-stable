// $Id: tag_file.cpp,v 1.21 2000/07/11 04:21:26 eldamitri Exp $

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

#include "tag.h"

size_t ID3_Tag::Link(const char *fileInfo, bool parseID3v1, bool parseLyrics3)
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

/** Attaches a file to the tag, parses the file, and adds any tag information
 ** found in the file to the tag.
 ** 
 ** Use this method if you created your ID3_Tag object without supplying a
 ** parameter to the constructor (maybe you created an array of ID3_Tag
 ** pointers).  This is the preferred method of interacting with files, since
 ** id3lib can automatically do things like parse foreign tag formats and
 ** handle padding when linked to a file.  When a tag is linked to a file,
 ** you do not need to use the <a href="#Size">Size</a>, <a
 ** href="#Render">Render</a>, or <a href="#Parse">Parse</a> methods or the
 ** <code>ID3_IsTagHeader</code> function---id3lib will take care of those
 ** details for you.  The single parameter is a pointer to a file name.
 ** 
 ** Link returns a 'luint' which is the byte position within the file that
 ** the audio starts (i.e., where the id3v2 tag ends).
 ** 
 ** \code
 **   ID3_Tag *myTag;
 **   if (myTag = new ID3_Tag)
 **   {
 **     myTag->Link("mysong.mp3");
 **     
 **     // do whatever we want with the tag
 **     // ...
 **   
 **     // setup all our rendering parameters
 **     myTag->SetUnsync(false);
 **     myTag->SetExtendedHeader(true);
 **     myTag->SetCompression(true);
 **     myTag->SetPadding(true);
 **     
 **     // write any changes to the file
 **     myTag->Update()
 **     
 **     // free the tag
 **     delete myTag;
 **   }
 ** \endcode
 ** 
 ** @see ID3_IsTagHeader
 ** @param fileInfo The filename of the file to link to.
 **/
size_t ID3_Tag::Link(const char *fileInfo, flags_t tag_types)
{
  __tags_to_parse.set(tag_types);
  
  if (NULL == fileInfo)
  {
    return 0;
  }

  strcpy(__file_name, fileInfo);
  __changed = true;

  this->ParseFile();
  
  return this->GetPrependedBytes();
}

size_t RenderV1ToFile(ID3_Tag& tag, fstream& file)
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

size_t RenderV2ToFile(const ID3_Tag& tag, fstream& file)
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
      ID3_THROW(ID3E_ReadOnly);
    }
    
    if (buffer)
    {
      fwrite(buffer, 1, tag_size, tempOut);
    }
    
    file.seekg(tag.GetPrependedBytes(), ios::beg);
    
    uchar buffer2[BUFSIZ];
    while (file)
    {
      file.read(buffer2, BUFSIZ);
      size_t nBytes = file.gcount();
      fwrite(buffer2, 1, nBytes, tempOut);
    }
    
    rewind(tempOut);
    ID3_OpenWritableFile(tag.GetFileName(), file);
    __file_size = ID3_GetFileSize(file);
    
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
      ID3_THROW_DESC(ID3E_NoFile, "filename too long");
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
      ID3_THROW(ID3E_ReadOnly);
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

    ID3_OpenWritableFile(tag.GetFileName(), file);
#endif
  }

  if (buffer)
  {
    delete [] buffer;
  }

  return tag_size;
}


/** Renders the tag and writes it to the attached file; the type of tag
 ** rendered can be specified as a parameter.  The default is to update only
 ** the ID3v2 tag.  See the ID3_TagType enumeration for the constants that
 ** can be used.
 ** 
 ** Make sure the rendering parameters are set before calling the method.
 ** See the Link dcoumentation for an example of this method in use.
 ** 
 ** \sa ID3_TagType
 ** \sa Link
 ** \param tt The type of tag to update.
 **/
flags_t ID3_Tag::Update(flags_t ulTagFlag)
{
  flags_t tags = ID3TT_NONE;
  
  fstream file;
  ID3_Err err = ID3_OpenWritableFile(this->GetFileName(), file);
  __file_size = ID3_GetFileSize(file);
  
  if (err == ID3E_NoFile)
  {
    err = ID3_CreateFile(this->GetFileName(), file);
  }
  if (err == ID3E_ReadOnly)
  {
    return tags;
  }

  if ((ulTagFlag & ID3TT_ID3V2) && this->HasChanged())
  {
    __prepended_bytes = RenderV2ToFile(*this, file);
    if (__prepended_bytes)
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
      if (! __file_tags.test(ID3TT_ID3V1))
      {
        __appended_bytes += tag_bytes;
      }
      tags |= ID3TT_ID3V1;
    }
  }
  __changed = false;
  __file_tags.add(tags);
  __file_size = ID3_GetFileSize(file);
  file.close();
  return tags;
}

/** Strips the tag(s) from the attached file. The type of tag stripped
 ** can be specified as a parameter.  The default is to strip all tag types.
 ** 
 ** \param tt The type of tag to strip
 ** \sa ID3_TagType@see
 **/
flags_t ID3_Tag::Strip(flags_t ulTagFlag)
{
  flags_t ulTags = ID3TT_NONE;
  const size_t data_size = ID3_GetDataSize(*this);
  
  // First remove the v2 tag, if requested
  if (ulTagFlag & ID3TT_PREPENDED & __file_tags.get())
  {
    fstream file;
    if (ID3E_NoError != ID3_OpenWritableFile(this->GetFileName(), file))
    {
      return ulTags;
    }
    __file_size = ID3_GetFileSize(file);

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
    while (file)
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

  if ((__file_tags.get() & ID3TT_APPENDED) && (ulTagFlag & ID3TT_APPENDED))
  {
    ulTags |= __file_tags.get() & ID3TT_APPENDED;
  }
  else
  {
    // if we're not stripping the appended tags, be sure to increase the file
    // size by those bytes
    nNewFileSize += this->GetAppendedBytes();
  }
  
  if ((ulTagFlag & ID3TT_PREPENDED) && (__file_tags.get() & ID3TT_PREPENDED))
  {
    // If we're stripping the ID3v2 tag, there's no need to adjust the new
    // file size, since it doesn't account for the ID3v2 tag size
    ulTags |= __file_tags.get() & ID3TT_PREPENDED;
  }
  else
  {
    // add the original prepended tag size since we don't want to delete it,
    // and the new file size represents the file size _not_ counting the ID3v2
    // tag
    nNewFileSize += this->GetPrependedBytes();
  }

  if (ulTags && (truncate(__file_name, nNewFileSize) == -1))
  {
    ID3_THROW(ID3E_NoFile);
  }

  __prepended_bytes = (ulTags & ID3TT_PREPENDED) ? 0 : __prepended_bytes;
  __appended_bytes  = (ulTags & ID3TT_APPENDED)  ? 0 : __appended_bytes;
  __file_size = data_size + __prepended_bytes + __appended_bytes;
  
  __changed = __file_tags.remove(ulTags) || __changed;
  
  return ulTags;
}
