// $Id: tag.cpp,v 1.12 1999/12/27 05:30:03 scott Exp $
// 
// This program is free software; you can distribute it and/or modify it under
// the terms discussed in the COPYING file, which should have been included
// with this distribution.
//  
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the COPYING file for more details.
//  
// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where
// to send such submissions.

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include "tag.h"

luint ID3_Tag::s_ulInstances = 0;


ID3_Tag& operator<<(ID3_Tag& tag, ID3_Frame& frame)
{
  tag.AddFrame(&frame);
  
  return tag;
}


ID3_Tag& operator<<(ID3_Tag& tag, ID3_Frame *frame)
{
  tag.AddFrame(frame);
  
  return tag;
}


ID3_Tag::ID3_Tag(char *name)
{
  SetupTag(name);
  
  s_ulInstances++;
}

ID3_Tag::ID3_Tag(const ID3_Tag &tag)
{
  SetupTag(NULL);
  *this = tag;

  s_ulInstances++;
}

void ID3_Tag::SetupTag(char *fileInfo)
{
  __ucVersion       = ID3v2_VERSION;
  __ucRevision      = ID3v2_REVISION;
  __pFrameList      = NULL;
  __pBinaryList     = NULL;
  __pFindCursor     = NULL;
  __fFileHandle     = NULL;
  __bSyncOn         = false;
  __bCompression    = true;
  __bPadding        = true;
  __bExtendedHeader = true;
  __bFileWritable   = false;
  __ulFileSize      = 0;
  __ulOldTagSize    = 0;
  __ulExtraBytes    = 0;
  __bHasV1Tag       = false;

  __sFileName[0]     = '\0';
  
  Clear();
  
  if (NULL != fileInfo)
  {
    Link(fileInfo);
  }
    
  return ;
}


ID3_Tag::~ID3_Tag(void)
{
  CloseFile();
    
  Clear();
  
  s_ulInstances--;
  
  if (s_ulInstances == 0)
  {
    // Do something here!
  }
  
}


void ID3_Tag::Clear(void)
{
  if (NULL != __pFrameList)
  {
    ClearList(__pFrameList);
    __pFrameList = NULL;
  }
  
  if (NULL != __pBinaryList)
  {
    ClearList(__pBinaryList);
    __pBinaryList = NULL;
  }

  __pFindCursor = NULL;
  __bHasChanged = true;
  
  return ;
}


void ID3_Tag::DeleteElem(ID3_Elem *cur)
{
  if (NULL != cur && cur->bTagOwns)
  {
    if (cur->pFrame)
    {
      delete cur->pFrame;
      cur->pFrame = NULL;
    }
      
    if (cur->acBinary)
    {
      delete [] cur->acBinary;
      cur->acBinary = NULL;
    }
    
    __pFindCursor = NULL;
    __bHasChanged = true;
    
    delete cur;
  }
  
  return ;
}


void ID3_Tag::ClearList(ID3_Elem *list)
{
  ID3_Elem *cur = list;
  
  while (cur)
  {
    ID3_Elem *pNext;
    
    pNext = cur->pNext;
    DeleteElem(cur);
    cur = pNext;
  }
  
  return ;
}

void ID3_Tag::AddFrame(ID3_Frame *newFrame)
{
  AddFrame(newFrame, false);
}

void ID3_Tag::AddNewFrame(ID3_Frame *newFrame)
{
  AddFrame(newFrame, true);
}

void ID3_Tag::AddFrame(ID3_Frame *newFrame, bool freeWhenDone)
{
  ID3_Elem *elem;
  
  if (NULL == newFrame)
  {
    ID3_THROW(ID3E_NoData);
  }

  elem = new ID3_Elem;
  if (NULL == elem)
  {
    ID3_THROW(ID3E_NoMemory);
  }

  elem->pNext = __pFrameList;
  elem->pFrame = newFrame;
  elem->acBinary = NULL;
  elem->bTagOwns = freeWhenDone;
      
  __pFrameList = elem;
  __pFindCursor = NULL;
      
  __bHasChanged = true;
    
  return ;
}


void ID3_Tag::AddFrames(ID3_Frame *frames, luint numFrames)
{
  AddFrames(frames, numFrames, false);
}

void ID3_Tag::AddFrames(ID3_Frame *frames, luint numFrames, bool freeWhenDone)
{
  lsint i;
  
  for (i = numFrames - 1; i >= 0; i--)
  {
    AddFrame(&frames[i], freeWhenDone);
  }
    
  return ;
}


void ID3_Tag::RemoveFrame(ID3_Frame *frame)
{
  ID3_Elem *elem = Find(frame);
  if (NULL != elem)
  {
    RemoveFromList(elem, &__pFrameList);
  }
    
  return ;
}


void ID3_Tag::RemoveFromList(ID3_Elem *which, ID3_Elem **list)
{
  ID3_Elem *cur = *list;
  
  if (cur == which)
  {
    *list = which->pNext;
    DeleteElem(which);
  }
  else
  {
    while (cur)
    {
      if (cur->pNext == which)
      {
        cur->pNext = which->pNext;
        DeleteElem(which);
        break;
      }
      else
      {
        cur = cur->pNext;
      }
    }
  }
  
  return ;
}


bool ID3_Tag::HasChanged(void) const
{
  bool changed = __bHasChanged;
  
  if (! changed)
  {
    ID3_Elem *cur = __pFrameList;
    
    while (cur)
    {
      if (cur->pFrame)
      {
        changed = cur->pFrame->HasChanged();
      }
        
      if (changed)
      {
        break;
      }
      else
      {
        cur = cur->pNext;
      }
    }
  }
  
  return changed;
}


void ID3_Tag::SetVersion(uchar ver, uchar rev)
{
  if (__ucVersion != ver || __ucRevision != rev)
  {
    __bHasChanged = true;
  }
    
  __ucVersion = ver;
  __ucRevision = rev;
  
  return ;
}


void ID3_Tag::SetUnsync(bool newSync)
{
  if (__bSyncOn != newSync)
  {
    __bHasChanged = true;
  }
    
  __bSyncOn = newSync;
  
  return ;
}


void ID3_Tag::SetExtendedHeader(bool ext)
{
  if (__bExtendedHeader != ext)
  {
    __bHasChanged = true;
  }
    
  __bExtendedHeader = ext;
  
  return ;
}


void ID3_Tag::SetCompression(bool comp)
{
  if (__bCompression != comp)
  {
    __bHasChanged = true;
  }
    
  __bCompression = comp;
  
  return ;
}


void ID3_Tag::SetPadding(bool pad)
{
  __bHasChanged = __bHasChanged && (__bPadding != pad);
    
  __bPadding = pad;
  
  return ;
}


luint ID3_Tag::NumFrames(void) const
{
  luint numFrames = 0;
  for (ID3_Elem *cur = __pFrameList; NULL != cur; cur = cur->pNext)
  {
    numFrames++;
  }
  
  return numFrames;
}

ID3_Tag &
ID3_Tag::operator=( const ID3_Tag &rTag )
{
  if (this != &rTag)
  {
    Clear();
    size_t nFrames = rTag.NumFrames();
    for (size_t nIndex = 0; nIndex < nFrames; nIndex++)
    {
      ID3_Frame *newFrame = new ID3_Frame;
      // Copy the frames in reverse order so that they appear in the same order
      // as the original tag when rendered.
      *newFrame = *(rTag[nFrames - nIndex - 1]);
      AddNewFrame(newFrame);
    }
  }
  return *this;
}

// $Log: tag.cpp,v $
// Revision 1.12  1999/12/27 05:30:03  scott
// (SetupTag): Updated for new version constants.
//
// Revision 1.11  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.10  1999/12/13 04:44:17  scott
// (SetupTag, ~ID3_Tag): Slight changes to reflect new __sFileName type.
//
// Revision 1.9  1999/12/09 03:32:28  scott
// (ID3_Tag): Added copy constructor implementation.
// (operator=): Added implementation.
//
// Revision 1.8  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.7  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.6  1999/11/25 18:24:27  scott
// (SetupTag): Initialized sFileName to NULL rather than the empty string
// to indicate no filename is present.
// (ID3_Tag): Deleted sFileName if allocated.  Added call to CloseFile.
// (AddFrame): Modified to reflect change in interface.  AddFrame now adds
// a frame without taking responsibility for deallocating the memory when
// the tag goes out of scope.
// (AddNewFrame): New method.  AddNewFrame adds a frame and takes
// responsibility for deallocating the frame when the tag goes out of
// scope.
//
// Revision 1.5  1999/11/15 20:20:17  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
