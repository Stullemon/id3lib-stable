// $Id: tag_find.cpp,v 1.7 1999/11/29 18:47:37 scott Exp $
// 
// The authors have released ID3Lib as Public Domain (PD) and claim no
// copyright, patent or other intellectual property protection in this work.
// This means that it may be modified, redistributed and used in commercial
// and non-commercial software and hardware without restrictions.  ID3Lib is
// distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
// express or implied.
// 
// The ID3Lib authors encourage improvements and optimisations to be sent to
// the ID3Lib coordinator, currently Dirk Mahoney (dirk@id3.org).  Approved
// submissions may be altered, and will be included and released under these
// terms.

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <id3/tag.h>
#include <id3/misc_support.h>

ID3_Elem *ID3_Tag::Find(ID3_Frame *frame) const
{
  ID3_Elem *elem = NULL;
  
  for (ID3_Elem *cur = __pFrameList; NULL != cur; cur = cur->pNext)
    if (cur->pFrame == frame)
    {
      elem = cur;
      break;
    }
  
  return elem;
}

ID3_Frame *ID3_Tag::Find(ID3_FrameID id)
{
  ID3_Frame *frame = NULL;
  
  // reset the cursor if it isn't set
  if (NULL == __pFindCursor)
    __pFindCursor = __pFrameList;

  for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
  {
    // We want to cycle through the list to find the matching frame.  We
    // should start from the cursor, search each successive frame, wrapping
    // if necessary.  The enclosing loop and the assignment statments below
    // ensure that we first start at the cursor and search to the end of the
    // list and, if unsuccessful, start from the beginning of the list and
    // search to the cursor.
    ID3_Elem
      *pStart  = (0 == iCount ? __pFindCursor : __pFrameList), 
      *pFinish = (0 == iCount ? NULL          : __pFindCursor);
    // search from the cursor to the end
    for (ID3_Elem *cur = pStart; cur != pFinish; cur = cur->pNext)
    {
      if ((cur->pFrame != NULL) && (cur->pFrame->GetID() == id))
      {
        // We've found a valid frame.  Set the cursor to be the next element
        frame = cur->pFrame;
        __pFindCursor = cur->pNext;
        break;
      }
    }
  }
  
  return frame;
}

ID3_Frame *ID3_Tag::Find(ID3_FrameID id, ID3_FieldID fld, char *data)
{
  ID3_Frame *frame = NULL;
  unicode_t *temp;
  
  temp = new unicode_t[strlen(data) + 1];
  if (NULL == temp)
    ID3_THROW(ID3E_NoMemory);

  mbstoucs(temp, data, strlen(data) + 1);
    
  frame = Find(id, fld, temp);
    
  delete[] temp;
  
  return frame;
}

ID3_Frame *ID3_Tag::Find(ID3_FrameID id, ID3_FieldID fld, unicode_t *data)
{
  ID3_Frame *frame = NULL;
  
  // reset the cursor if it isn't set
  if (NULL == __pFindCursor)
    __pFindCursor = __pFrameList;

  for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
  {
    // We want to cycle through the list to find the matching frame.  We
    // should start from the cursor, search each successive frame, wrapping
    // if necessary.  The enclosing loop and the assignment statments below
    // ensure that we first start at the cursor and search to the end of the
    // list and, if unsuccessful, start from the beginning of the list and
    // search to the cursor.
    ID3_Elem
      *pStart  = (0 == iCount ? __pFindCursor : __pFrameList), 
      *pFinish = (0 == iCount ? NULL          : __pFindCursor);
    // search from the cursor to the end
    for (ID3_Elem *cur = pStart; cur != pFinish; cur = cur->pNext)
    {
      if ((cur->pFrame != NULL) && (cur->pFrame->GetID() == id) &&
          (data != NULL) && ucslen(data) > 0 && 
          BS_ISSET(cur->pFrame->__auiFieldBits, fld))
      {
        luint ulSize = cur->pFrame->Field(fld).BinSize();
        unicode_t *wsBuffer = new unicode_t[ulSize];
          
        if (NULL == wsBuffer)
          ID3_THROW(ID3E_NoMemory);
          
        cur->pFrame->Field(fld).Get(wsBuffer, ulSize);
          
        bool bInFrame = (ucscmp(wsBuffer, data) == 0);
          
        delete [] wsBuffer;

        if (bInFrame)
        {
          // We've found a valid frame.  Set cursor to be the next element
          frame = cur->pFrame;
          __pFindCursor = cur->pNext;
          break;
        }
      }
    }
  }
  
  return frame;
}

ID3_Frame *ID3_Tag::Find(ID3_FrameID id, ID3_FieldID fld, luint data)
{
  ID3_Frame *frame = NULL;
  
  // reset the cursor if it isn't set
  if (NULL == __pFindCursor)
    __pFindCursor = __pFrameList;

  for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
  {
    // We want to cycle through the list to find the matching frame.  We
    // should start from the cursor, search each successive frame, wrapping
    // if necessary.  The enclosing loop and the assignment statments below
    // ensure that we first start at the cursor and search to the end of the
    // list and, if unsuccessful, start from the beginning of the list and
    // search to the cursor.
    ID3_Elem
      *pStart  = (0 == iCount ? __pFindCursor : __pFrameList), 
      *pFinish = (0 == iCount ? NULL          : __pFindCursor);
    // search from the cursor to the end
    for (ID3_Elem *cur = pStart; cur != pFinish; cur = cur->pNext)
    {
      if ((cur->pFrame != NULL) && (cur->pFrame->GetID() == id) &&
          (cur->pFrame->Field(fld).Get() == data))
      {
        // We've found a valid frame.  Set the cursor to be the next element
        frame = cur->pFrame;
        __pFindCursor = cur->pNext;
        break;
      }
    }
  }
  
  return frame;
}

ID3_Frame *ID3_Tag::GetFrameNum(luint num) const
{
  ID3_Frame *frame = NULL;

  luint curNum = 0;
  for (ID3_Elem *cur = __pFrameList; cur != NULL; cur = cur->pNext)
    // compare and advance counter
    if (num == curNum++)
    {
      frame = cur->pFrame;
      break;
    }
  
  return frame;
}

ID3_Frame *ID3_Tag::operator[](luint num) const
{
  return GetFrameNum(num);
}

// $Log: tag_find.cpp,v $
// Revision 1.7  1999/11/29 18:47:37  scott
// (): Updated methods to use unicode_t instead of wchar_t.  unicode_t is
// defined in globals.h.  The type unicode_t is more likely to be two
// bytes on every platform.  Updated calls to unicode methods to more
// general methods defined in misc_support.cpp.
//
// Revision 1.6  1999/11/19 19:10:14  scott
// * tag_find.cpp (Find): Add const qualifier.
//
// Revision 1.5  1999/11/15 20:20:47  scott
// Added include for config.h.  Removed assignments from if checks;
// first makes assignment, then checks for appropriate value.  Made
// private member variable names more descriptive.  Major code
// cleanup: use of for loops instead of while's to ease code
// readability, more compact looping structures to reduce possibility
// in coding or human parsing errors,
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
