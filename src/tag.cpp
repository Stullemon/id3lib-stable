// $Id: tag.cpp,v 1.9 2000/05/01 16:36:57 eldamitri Exp $

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

#include "tag.h"

#ifdef  MAXPATHLEN
#  define ID3_PATH_LENGTH   (MAXPATHLEN + 1)
#elif   defined (PATH_MAX)
#  define ID3_PATH_LENGTH   (PATH_MAX + 1)
#else   /* !MAXPATHLEN */
#  define ID3_PATH_LENGTH   (2048 + 1)
#endif  /* !MAXPATHLEN && !PATH_MAX */

/** \class ID3_Tag
 ** \brief The representative class of an id3 tag.
 ** 
 ** This is the 'container' class for everything else.  It is through an
 ** ID3_Tag that most of the productive stuff happens.  
 ** Let's look at what's
 ** required to start using ID3v2 tags.
 ** 
 ** \code
 **   #include <id3/tag.h>
 ** \endcode
 ** 
 ** This simple \c #include does it all.  In order to read an
 ** existing tag, do the following:
 **
 ** \code
 **   ID3_Tag myTag;
 **   myTag.Link("something.mp3");
 ** \endcode
 ** 
 ** That is all there is to it.  Now all you have to do is use the 
 ** Find() method to locate the frames you are interested in
 ** is the following:
 ** 
 ** \code
 **   ID3_Frame *myFrame;
 **   if (myTag.Find(ID3FID_TITLE) == myFrame)
 **   {
 **     char title[1024];
 **     myFrame->Field(ID3FN_TEXT).Get(title, 1024);
 **     cout << "Title: " << title << endl;
 **   }
 ** \endcode
 ** 
 ** This code snippet locates the ID3FID_TITLE frame and copies the contents of
 ** the text field into a buffer and displays the buffer.  Not difficult, eh?
 **
 ** When using the ID3_Tag::Link() method, you automatically gain access to any
 ** ID3v1/1.1, ID3v2, and Lyrics3 v2.0 tags present in the file.  The class
 ** will automaticaly parse and convert any of these foreign tag formats into
 ** ID3v2 tags.  Also, id3lib will correctly parse any correctly formatted
 ** 'CDM' frames from the unreleased ID3v2 2.01 draft specification.
 **
 ** \author Dirk Mahoney
 ** \version $Id: tag.cpp,v 1.9 2000/05/01 16:36:57 eldamitri Exp $
 ** \sa ID3_Frame
 ** \sa ID3_Field
 ** \sa ID3_Err
 **/

luint ID3_Tag::s_ulInstances = 0;

/** Copies a frame to the tag.  The frame parameter can thus safely be deleted
 ** or allowed to go out of scope.
 ** 
 ** Operator<< supports the addition of a pointer to a frame object, or
 ** the frame object itself.
 **
 ** \code
 **   ID3_Frame *pFrame, frame;
 **   p_frame = &frame;
 **   myTag << pFrame;
 **   myTag << frame;
 ** \endcode
 **
 ** Both these methods copy the given frame to the tag---the tag creates its
 ** own copy of the frame.
 ** 
 ** \name operator<<
 ** \param frame The frame to be added to the tag.
 **/
ID3_Tag& operator<<(ID3_Tag& tag, const ID3_Frame& frame)
{
  tag.AddFrame(frame);
  
  return tag;
}


ID3_Tag& operator<<(ID3_Tag& tag, const ID3_Frame *frame)
{
  tag.AddFrame(frame);
  
  return tag;
}


/** Default constructor; it can accept an optional filename as a parameter.
 **
 ** If this file exists, it will be opened and all id3lib-supported tags will
 ** be parsed and converted to id3v2 if necessary.  After the conversion, the
 ** file will remain unchanged, and will continue to do so until you use the
 ** Update() method on the tag (if you choose to Update() at all).
 **
 ** \param name The filename of the mp3 file to link to
 **/
ID3_Tag::ID3_Tag(char *name)
{
  SetupTag(name);
  
  s_ulInstances++;
}

  /** Standard copy constructor.
   **
   ** \param tag What is copied into this tag
   **/
ID3_Tag::ID3_Tag(const ID3_Tag &tag)
{
  SetupTag(NULL);
  *this = tag;

  s_ulInstances++;
}

void ID3_Tag::SetupTag(char *fileInfo)
{
  __sFileName       = new char[ID3_PATH_LENGTH];
  __spec            = ID3V2_LATEST;
  __pFrameList      = NULL;
  __pBinaryList     = NULL;
  __pFindCursor     = NULL;
  __fFileHandle     = NULL;
  __bSyncOn         = false;
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

  delete [] __sFileName;
  
}

  /** Clears the object and disassociates it from any files.
   **
   ** It frees any resources for which the object is responsible, and the
   ** object is now free to be used again for any new or existing tag.
   **/
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

void ID3_Tag::AddFrame(const ID3_Frame& frame)
{
  this->AddFrame(&frame);
}

/** Attaches a frame to the tag; the tag doesn't take responsibility for
 ** releasing the frame's memory when tag goes out of scope.
 ** 
 ** Optionally, operator<< can also be used to attach a frame to a tag.  To
 ** use, simply supply its parameter a pointer to the ID3_Frame object you wish
 ** to attach.
 ** 
 ** \code
 **   ID3_Frame myFrame;
 **   myTag.AddFrame(&myFrame);
 ** \endcode 
 ** 
 ** As stated, this method attaches the frames to the tag---the tag does
 ** not create its own copy of the frame.  Frames created by an application
 ** must exist until the frame is removed or the tag is finished with it.
 ** 
 ** \param pFrame A pointer to the frame that is being added to the tag.
 ** \sa ID3_Frame
 **/
void ID3_Tag::AddFrame(const ID3_Frame *frame)
{
  if (frame)
  {
    ID3_Frame* new_frame = new ID3_Frame(*frame);
    this->AttachFrame(new_frame);
  }
}

/** Attaches a frame to the tag; the tag takes responsibility for
 ** releasing the frame's memory when tag goes out of scope.
 ** 
 ** This method accepts responsibility for the attached frame's memory, and
 ** will delete the frame and its contents when the tag goes out of scope or is
 ** deleted.  Therefore, be sure the frame isn't "Attached" to other tags.
 ** 
 ** \code
 **   ID3_Frame *frame = new ID3_Frame;
 **   myTag.AttachFrame(frame);
 ** \endcode
 ** 
 ** \param frame A pointer to the frame that is being added to the tag.
 **/
void ID3_Tag::AttachFrame(ID3_Frame *frame)
{
  ID3_Elem *elem;
  
  if (NULL == frame)
  {
    ID3_THROW(ID3E_NoData);
  }

  elem = new ID3_Elem;
  if (NULL == elem)
  {
    ID3_THROW(ID3E_NoMemory);
  }

  elem->pNext = __pFrameList;
  elem->pFrame = frame;
  elem->acBinary = NULL;
  elem->bTagOwns = true;
  
  __pFrameList = elem;
  __pFindCursor = NULL;
  
  __bHasChanged = true;
  
  return ;
}


/** Copies an array of frames to the tag.
 ** 
 ** This method copies each frame in an array to the tag.  As in 
 ** AddFrame, the tag adds a copy of the frame, and it assumes responsiblity
 ** for freeing the frames' memory when the tag goes out of scope.
 ** 
 ** \code
 **   ID3_Frame myFrames[10];
 **   myTag.AddFrames(myFrames, 10);
 ** \endcode
 ** 
 ** \sa ID3_Frame
 ** \sa ID3_Frame#AddFrame
 ** \param pNewFrames A pointer to an array of frames to be added to the tag.
 ** \param nFrames The number of frames in the array pNewFrames.
 **/
void ID3_Tag::AddFrames(const ID3_Frame *frames, luint numFrames)
{
  lsint i;
  
  for (i = numFrames - 1; i >= 0; i--)
  {
    AddFrame(frames[i]);
  }
    
  return ;
}


/** Removes a frame from the tag.
 ** 
 ** If you already own the frame object in question, then you should already
 ** have a pointer to the frame you want to delete.  If not, or if you wish to
 ** delete a pre-existing frame (from a tag you have parsed, for example), the
 ** use one of the Find methods to obtain a frame pointer to pass to this
 ** method.
 ** 
 ** \code
 **   ID3_Frame *someFrame;
 **   if (someFrame = myTag.Find(ID3FID_TITLE))
 **   {
 **     myTag.RemoveFrame(someFrame);
 **   }
 ** \endcode
 **   
 ** \sa ID3_Tag#Find
 ** \param pOldFrame A pointer to the frame that is to be removed from the
 **                  tag
 **/
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


  /** Indicates whether the tag has been altered since the last parse, render,
   ** or update.
   **
   ** If you have a tag linked to a file, you do not need this method since the
   ** Update() method will check for changes before writing the tag.
   ** 
   ** This method is primarily intended as a status indicator for applications
   ** and for applications that use the Parse() and Render() methods.
   **
   ** Setting a field, changed the ID of an attached frame, setting or grouping
   ** or encryption IDs, and clearing a frame or field all constitute a change
   ** to the tag, as do calls to the SetUnsync(), SetExtendedHeader(), and
   ** SetPadding() methods.
   
   ** \code
   **   if (myTag.HasChanged())
   **   {
   **     // render and output the tag
   **   }
   ** \endcode
   
   ** \return Whether or not the tag has been altered.
   **/
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

void ID3_Tag::SetSpec(const ID3_V2Spec spec)
{
  __bHasChanged = __bHasChanged || (spec != __spec);
  __spec = spec;
}

ID3_V2Spec ID3_Tag::GetSpec() const
{
  return __spec;
}

  /** Turns unsynchronization on or off, dependant on the value of the boolean
   ** parameter.
   ** 
   ** If you call this method with 'false' as the parameter, the
   ** binary tag will not be unsync'ed, regardless of whether the tag should
   ** be.  This option is useful when the file is only going to be used by
   ** ID3v2-compliant software.  See the id3v2 standard document for futher
   ** details on unsync.
   **
   ** Be default, tags are created without unsync.
   ** 
   ** \code
   **   myTag.SetUnsync(false);
   ** \endcode
   ** 
   ** \param bSync Whether the tag should be unsynchronized
   **/
void ID3_Tag::SetUnsync(bool newSync)
{
  if (__bSyncOn != newSync)
  {
    __bHasChanged = true;
  }
    
  __bSyncOn = newSync;
  
  return ;
}


  /** Turns extended header rendering on or off, dependant on the value of the
   ** boolean parameter.
   ** 
   ** This option is currently ignored as id3lib doesn't yet create extended
   ** headers.  This option only applies when rendering tags for id3v2 versions
   ** that support extended headers.
   **
   ** By default, id3lib will generate extended headers for all tags in which
   ** extended headers are supported.
   ** 
   ** \code
   **   myTag.SetExtendedHeader(true);
   ** \endcode
   ** 
   ** \param bExt Whether to render an extended header
   **/
void ID3_Tag::SetExtendedHeader(bool ext)
{
  if (__bExtendedHeader != ext)
  {
    __bHasChanged = true;
  }
    
  __bExtendedHeader = ext;
  
  return ;
}


/** Turns padding on or off, dependant on the value of the boolean
 ** parameter.
 ** 
 ** When using id3v2 tags in association with files, id3lib can optionally
 ** add padding to the tags to ensure minmal file write times when updating
 ** the tag in the future.
 ** 
 ** When the padding option is switched on, id3lib automatically creates
 ** padding according to the 'ID3v2 Programming Guidelines'.  Specifically,
 ** enough padding will be added to round out the entire file (song plus
 ** tag) to an even multiple of 2K.  Padding will only be created when the
 ** tag is attached to a file and that file is not empty (aside from a
 ** pre-existing tag).
 ** 
 ** id3lib's addition to the guidelines for padding, is that if frames are
 ** removed from a pre-existing tag (or the tag simply shrinks because of
 ** other reasons), the new tag will continue to stay the same size as the
 ** old tag (with padding making the difference of course) until such time as
 ** the padding is greater than 4K.  When this happens, the padding will be
 ** reduced and the new tag will be smaller than the old.
 ** 
 ** By default, padding is switched on.
 ** 
 ** \code
 **   myTag.SetPadding(false);
 ** \endcode
 ** 
 ** \param bPad Whether or not render the tag with padding.
 **/
void ID3_Tag::SetPadding(bool pad)
{
  __bHasChanged = __bHasChanged && (__bPadding != pad);
    
  __bPadding = pad;
  
  return ;
}


  /** Returns the number of frames present in the tag object.
   ** 
   ** This includes only those frames that id3lib recognises.  This is used as
   ** the upper bound on calls to the GetFrame() and operator[]() methods.
   ** 
   ** \return The number of frames present in the tag object.
   **/
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
      ID3_Frame *frame = new ID3_Frame;
      // Copy the frames in reverse order so that they appear in the same order
      // as the original tag when rendered.
      *frame = *(rTag[nFrames - nIndex - 1]);
      AttachFrame(frame);
    }
  }
  return *this;
}
