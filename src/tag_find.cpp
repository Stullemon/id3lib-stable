// $Id: tag_find.cpp,v 1.20 2000/10/09 04:29:40 eldamitri Exp $

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

#include <string.h>

#include "tag_impl.h"
#include "field.h"
#include "utils.h"

using namespace dami;

ID3_Elem *ID3_TagImpl::Find(const ID3_Frame *frame) const
{
  ID3_Elem *elem = NULL;
  
  for (ID3_Elem *cur = _frames; NULL != cur; cur = cur->pNext)
  {
    if (cur->pFrame == frame)
    {
      elem = cur;
      break;
    }
  }
  
  return elem;
}

ID3_Frame *ID3_TagImpl::Find(ID3_FrameID id) const
{
  ID3_Frame *frame = NULL;
  
  // reset the cursor if it isn't set
  if (NULL == _cursor)
  {
    _cursor = _frames;
  }

  for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
  {
    // We want to cycle through the list to find the matching frame.  We
    // should start from the cursor, search each successive frame, wrapping
    // if necessary.  The enclosing loop and the assignment statments below
    // ensure that we first start at the cursor and search to the end of the
    // list and, if unsuccessful, start from the beginning of the list and
    // search to the cursor.
    ID3_Elem
      *pStart  = (0 == iCount ? _cursor : _frames), 
      *pFinish = (0 == iCount ? NULL          : _cursor);
    // search from the cursor to the end
    for (ID3_Elem *cur = pStart; cur != pFinish; cur = cur->pNext)
    {
      if ((cur->pFrame != NULL) && (cur->pFrame->GetID() == id))
      {
        // We've found a valid frame.  Set the cursor to be the next element
        frame = cur->pFrame;
        _cursor = cur->pNext;
        break;
      }
    }
  }
  
  return frame;
}

ID3_Frame *ID3_TagImpl::Find(ID3_FrameID id, ID3_FieldID fldID, const char *data) const
{
  ID3_Frame *frame = NULL;
  
  // reset the cursor if it isn't set
  if (NULL == _cursor)
  {
    _cursor = _frames;
  }

  if (NULL == data)
  {
    return frame;
  }

  for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
  {
    // We want to cycle through the list to find the matching frame.  We
    // should start from the cursor, search each successive frame, wrapping
    // if necessary.  The enclosing loop and the assignment statments below
    // ensure that we first start at the cursor and search to the end of the
    // list and, if unsuccessful, start from the beginning of the list and
    // search to the cursor.
    ID3_Elem
      *pStart  = (0 == iCount ? _cursor : _frames), 
      *pFinish = (0 == iCount ? NULL     : _cursor);
    // search from the cursor to the end
    for (ID3_Elem *cur = pStart; cur != pFinish; cur = cur->pNext)
    {
      if ((cur->pFrame != NULL) && (cur->pFrame->GetID() == id) &&
          cur->pFrame->Contains(fldID))
      {
        ID3_Field* fld = cur->pFrame->GetField(fldID);
        if (NULL == fld)
        {
          continue;
        }

        const char* text = fld->GetText();
        size_t fldSize = fld->Size();

        if ((text == NULL && ::strlen(data) == 0) ||
            (text != NULL && ::strcmp(fld->GetText(), data) == 0))
        {
          // We've found a valid frame.  Set cursor to be the next element
          frame = cur->pFrame;
          _cursor = cur->pNext;
          break;
        }
      }
    }
  }
  
  return frame;
}

ID3_Frame *ID3_TagImpl::Find(ID3_FrameID id, ID3_FieldID fldID, const unicode_t *data) const
{
  ID3_Frame *frame = NULL;
  
  // reset the cursor if it isn't set
  if (NULL == _cursor)
  {
    _cursor = _frames;
  }

  if (NULL == data)
  {
    return frame;
  }

  for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
  {
    // We want to cycle through the list to find the matching frame.  We
    // should start from the cursor, search each successive frame, wrapping
    // if necessary.  The enclosing loop and the assignment statments below
    // ensure that we first start at the cursor and search to the end of the
    // list and, if unsuccessful, start from the beginning of the list and
    // search to the cursor.
    ID3_Elem
      *pStart  = (0 == iCount ? _cursor : _frames), 
      *pFinish = (0 == iCount ? NULL     : _cursor);
    // search from the cursor to the end
    for (ID3_Elem *cur = pStart; cur != pFinish; cur = cur->pNext)
    {
      if ((cur->pFrame != NULL) && (cur->pFrame->GetID() == id) &&
          cur->pFrame->Contains(fldID))
      {
        ID3_Field* fld = cur->pFrame->GetField(fldID);
        if (NULL == fld)
        {
          continue;
        }

        if (::ucscmp(fld->GetUnicodeText(), data) == 0)
        {
          // We've found a valid frame.  Set cursor to be the next element
          frame = cur->pFrame;
          _cursor = cur->pNext;
          break;
        }
      }
    }
  }
  
  return frame;
}

ID3_Frame *ID3_TagImpl::Find(ID3_FrameID id, ID3_FieldID fldID, uint32 data) const
{
  ID3_Frame *frame = NULL;
  
  // reset the cursor if it isn't set
  if (NULL == _cursor)
    _cursor = _frames;

  for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
  {
    // We want to cycle through the list to find the matching frame.  We
    // should start from the cursor, search each successive frame, wrapping
    // if necessary.  The enclosing loop and the assignment statments below
    // ensure that we first start at the cursor and search to the end of the
    // list and, if unsuccessful, start from the beginning of the list and
    // search to the cursor.
    ID3_Elem
      *pStart  = (0 == iCount ? _cursor : _frames), 
      *pFinish = (0 == iCount ? NULL          : _cursor);
    // search from the cursor to the end
    for (ID3_Elem *cur = pStart; cur != pFinish; cur = cur->pNext)
    {
      if ((cur->pFrame != NULL) && (cur->pFrame->GetID() == id) &&
          (cur->pFrame->GetField(fldID)->Get() == data))
      {
        // We've found a valid frame.  Set the cursor to be the next element
        frame = cur->pFrame;
        _cursor = cur->pNext;
        break;
      }
    }
  }
  
  return frame;
}

  /** Returns a pointer to the frame with the given index; returns NULL if
   ** there is no such frame at that index.
   ** 
   ** Optionally, <a href="#operator[]">operator[]</a> can be used as an
   ** alternative to this method.  Indexing is 0-based (that is, the first
   ** frame is number 0, and the last frame in a tag that holds n frames is
   ** n-1).
   ** 
   ** If you wish to have a more comlex searching facility, then at least for
   ** now you will have to devise it yourself and implement it useing these
   ** methods.
   ** 
   ** @param nIndex The index of the frame that is to be retrieved
   ** @return A pointer to the requested frame, or NULL if no such frame.
   **/
ID3_Frame *ID3_TagImpl::GetFrameNum(index_t num) const
{
  const size_t num_frames = this->NumFrames();
  if (num >= num_frames)
  {
    return NULL;
  }

  ID3_Frame *frame = NULL;
  index_t curNum = num_frames;
  for (ID3_Elem *cur = _frames; cur != NULL; cur = cur->pNext)
  {
    // compare and advance counter
    if (num == --curNum)
    {
      frame = cur->pFrame;
      break;
    }
  }
  
  return frame;
}

/** Returns a pointer to the frame with the given index; returns NULL if
 ** there is no such frame at that index.
 ** 
 ** @name operator[]
 ** @param nIndex The index of the frame that is to be retrieved
 ** @return A pointer to the requested frame, or NULL if no such frame. 
 ** @see #GetFrameNum
 **/
ID3_Frame *ID3_TagImpl::operator[](index_t num) const
{
  return GetFrameNum(num);
}
