// $Id: tag_render.cpp,v 1.31 2000/10/03 04:51:27 eldamitri Exp $

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

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#if defined HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "tag_impl.h"
#include "helpers.h"
#include "utils.h"

using namespace dami;

/** Renders an id3v1.1 version of the tag into the supplied buffer.
 ** 
 ** @return The actual number of the bytes of the buffer used to store the
 **         tag (should always be 128)
 ** @param  buffer The buffer that will contain the id3v1.1 tag.
 **/
size_t RenderV1(const ID3_TagImpl& tag, uchar *buffer)
{
  // Sanity check our buffer
  if (NULL == buffer)
  {
    return 0;
    //ID3_THROW(ID3E_NoBuffer);
  }

  // pCur is used to mark where to next write in the buffer
  // sTemp is used as a temporary string pointer for functions that return
  //  dynamically created strings
  char* pCur = (char *) buffer;
  String str;

  // The default char for a v1 tag is null
  ::memset(buffer, '\0', ID3_V1_LEN);

  // Write the TAG identifier
  strncpy(pCur, "TAG", ID3_V1_LEN_ID);
  pCur = &pCur[ID3_V1_LEN_ID];

  // Write the TITLE
  str = id3::v2::getTitle(tag);
  ::strncpy(pCur, str.c_str(), ID3_V1_LEN_TITLE);
  pCur = &pCur[ID3_V1_LEN_TITLE];

  // Write the ARTIST
  str = id3::v2::getArtist(tag);
  ::strncpy(pCur, str.c_str(), ID3_V1_LEN_ARTIST);
  pCur = &pCur[ID3_V1_LEN_ARTIST];

  // Write the ALBUM
  str = id3::v2::getAlbum(tag);
  ::strncpy(pCur, str.c_str(), ID3_V1_LEN_ALBUM);
  pCur = &pCur[ID3_V1_LEN_ALBUM];

  // Write the YEAR
  str = id3::v2::getYear(tag);
  ::strncpy(pCur, str.c_str(), ID3_V1_LEN_YEAR);
  pCur = &pCur[ID3_V1_LEN_YEAR];

  // Write the COMMENT
  // Find the comment with the description STR_V1_COMMENT_DESC
  ID3_Frame* frame;
  str = "";
  (frame = tag.Find(ID3FID_COMMENT, ID3FN_DESCRIPTION, STR_V1_COMMENT_DESC)) ||
  (frame = tag.Find(ID3FID_COMMENT, ID3FN_DESCRIPTION, ""))                  ||
  (frame = tag.Find(ID3FID_COMMENT));
  str = id3::v2::getString(frame, ID3FN_TEXT);
  ::strncpy(pCur, str.c_str(), ID3_V1_LEN_COMMENT);
  pCur = &pCur[ID3_V1_LEN_COMMENT];

  // Write the TRACK, if it isn't 0
  luint nTrack = id3::v2::getTrackNum(tag);
  if (0 != nTrack)
  {
    pCur -= 2;
    pCur[0] = '\0';
    pCur[1] = (uchar) nTrack;
    pCur += 2;
  }

  // Write the GENRE
  pCur[0] = (uchar) id3::v2::getGenreNum(tag);

  return ID3_V1_LEN;
}

size_t RenderFrames(uchar* buffer, const ID3_Elem* cur)
{
  size_t size = 0;
  if (cur)
  {
    size = RenderFrames(buffer, cur->pNext);
    size += cur->pFrame->Render(&buffer[size]);
  }
  return size;
}

/** Renders a binary image of the tag into the supplied buffer.
 ** 
 ** See <a href="#Size">Size</a> for an example.  This method returns the
 ** actual number of the bytes of the buffer used to store the tag.  This
 ** will be no more than the size of the buffer itself, because
 ** <a href="#Size">Size</a> over estimates the required buffer size when
 ** padding is enabled.
 ** 
 ** Before calling this method, it is advisable to call <a
 ** href="#HasChanged">HasChanged</a> first as this will let you know
 ** whether you should bother rendering the tag.
 ** 
 ** @see    ID3_IsTagHeader
 ** @see    ID3_Tag#HasChanged
 ** @return The actual number of the bytes of the buffer used to store the
 **         tag
 ** @param  buffer The buffer that will contain the rendered tag.
 **/
size_t ID3_TagImpl::RenderV2(uchar *buffer) const
{
  // There has to be at least one frame for there to be a tag...
  if (this->NumFrames() == 0)
  {
    return 0;
  }
  
  if (NULL == buffer)
  {
    // log this
    return 0;
    //ID3_THROW(ID3E_NoBuffer);
  }

  ID3_TagHeader hdr;
  hdr.SetSpec(ID3V2_LATEST);
  size_t hdr_size = hdr.Size();
  size_t bytesUsed = hdr_size;
    
  // set up the encryption and grouping IDs
    
  // ...
  size_t frame_bytes = RenderFrames(&buffer[bytesUsed], _frames);
  if (frame_bytes == 0)
  {
    return 0;
  }
  
  bytesUsed += frame_bytes;
  
  if (this->GetUnsync())
  {
    size_t newTagSize = ::getUnSyncSize(&buffer[hdr_size], 
                                        bytesUsed - hdr_size);
    if (newTagSize > 0 && (newTagSize + hdr_size) > bytesUsed)
    {
      uchar* tempz = new uchar[newTagSize];
      if (NULL == tempz)
      {
        return 0;
        //ID3_THROW(ID3E_NoMemory);
      }

      ::unsync(tempz, newTagSize, &buffer[hdr_size],
               bytesUsed - hdr_size);
      hdr.SetUnsync(true);

      memcpy(&buffer[hdr_size], tempz, newTagSize);
      bytesUsed = newTagSize + hdr_size;
      delete[] tempz;
    }
  }
    
  // zero the remainder of the buffer so that our padding bytes are zero
  luint nPadding = PaddingSize(bytesUsed);

  memset(&buffer[bytesUsed], '\0', nPadding);
  bytesUsed += nPadding;
    
  hdr.SetDataSize(bytesUsed - hdr_size);
  hdr.Render(buffer);
  
  return bytesUsed;
}

size_t ID3_TagImpl::Render(uchar* buffer, ID3_TagType tt) const
{
  size_t tag_bytes = 0;
  if (tt & ID3TT_ID3V2)
  {
    tag_bytes = this->RenderV2(buffer);
  }
  else if (tt & ID3TT_ID3V1)
  {
    tag_bytes = RenderV1(*this, buffer);
  }
  return tag_bytes;
}

  /** Returns an over estimate of the number of bytes required to store a
   ** binary version of a tag. 
   ** 
   ** When using <a href="#Render">Render</a> to render a binary tag to a
   ** memory buffer, first use the result of this call to allocate a buffer of
   ** unsigned chars.
   ** 
   ** \code
   **   luint tagSize;
   **   uchar *buffer;
   **   if (myTag.HasChanged())
   **   {
   **     if ((tagSize= myTag.Size()) > 0)
   **     {
   **       if (buffer = new uchar[tagSize])
   **       {
   **         luint actualSize = myTag.Render(buffer);
   **         // do something useful with the first
   **         // 'actualSize' bytes of the buffer,
   **         // like push it down a socket
   **         delete [] buffer;
   **       }
   **     }
   **   }
   ** \endcode
   **
   ** @see #Render
   ** @return The (overestimated) number of bytes required to store a binary
   **         version of a tag
   **/
size_t ID3_TagImpl::Size() const
{
  if (!this->NumFrames())
  {
    return 0;
  }
  ID3_Elem *cur = _frames;
  ID3_TagHeader hdr;

  hdr.SetSpec(this->GetSpec());
  size_t bytesUsed = hdr.Size();
  
  size_t frame_bytes = 0;
  while (cur)
  {
    if (cur->pFrame)
    {
      cur->pFrame->SetSpec(this->GetSpec());
      frame_bytes += cur->pFrame->Size();
    }
    
    cur = cur->pNext;
  }
  
  if (!frame_bytes)
  {
    return 0;
  }
  
  bytesUsed += frame_bytes;
  // add 30% for sync
  if (this->GetUnsync())
  {
    bytesUsed += bytesUsed / 3;
  }
    
  bytesUsed += PaddingSize(bytesUsed);
  return bytesUsed;
}


void ID3_TagImpl::RenderExtHeader(uchar *buffer)
{
  if (this->GetSpec() == ID3V2_3_0)
  {
  }
  
  return ;
}


#define ID3_PADMULTIPLE (2048)
#define ID3_PADMAX  (4096)


size_t ID3_TagImpl::PaddingSize(size_t curSize) const
{
  luint newSize = 0;
  
  // if padding is switched off or there is no attached file
  if (! _is_padded || !this->GetPrependedBytes())
  {
    return 0;
  }
    
  // if the old tag was large enough to hold the new tag, then we will simply
  // pad out the difference - that way the new tag can be written without
  // shuffling the rest of the song file around
  if (this->GetPrependedBytes() && (this->GetPrependedBytes() >= curSize) && 
      (this->GetPrependedBytes() - curSize) < ID3_PADMAX)
  {
    newSize = this->GetPrependedBytes();
  }
  else
  {
    luint tempSize = curSize + ID3_GetDataSize(*this);
    
    // this method of automatic padding rounds the COMPLETE FILE up to the
    // nearest 2K.  If the file will already be an even multiple of 2K (with
    // the tag included) then we just add another 2K of padding
    tempSize = ((tempSize / ID3_PADMULTIPLE) + 1) * ID3_PADMULTIPLE;
    
    // the size of the new tag is the new filesize minus the audio data
    newSize = tempSize - ID3_GetDataSize(*this);
  }
  
  return newSize - curSize;
}
