// $Id: tag.cpp,v 1.26 2000/10/03 04:32:55 eldamitri Exp $

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

#include "tag.h"
#include "tag_impl.h"
#include "readers.h"
#include "strings.h"

using namespace dami;

/** \class ID3_Tag
 ** \brief The representative class of an id3 tag.
 ** 
 ** This is the 'container' class for everything else.  It is through an
 ** ID3_Tag that most of the productive stuff happens.  Let's look at what's
 ** required to start using ID3v2 tags.
 ** 
 ** \code
 **   #include <id3/tag.h>
 ** \endcode
 ** 
 ** This simple \c #include does it all.  In order to read an existing tag, do
 ** the following:
 **
 ** \code
 **   ID3_Tag myTag;
 **   myTag.Link("something.mp3");
 ** \endcode
 ** 
 ** That is all there is to it.  Now all you have to do is use the Find()
 ** method to locate the frames you are interested in is the following:
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
 ** \version $Id: tag.cpp,v 1.26 2000/10/03 04:32:55 eldamitri Exp $
 ** \sa ID3_Frame
 ** \sa ID3_Field
 ** \sa ID3_Err
 **/

/** Default constructor; it can accept an optional filename as a parameter.
 **
 ** If this file exists, it will be opened and all id3lib-supported tags will
 ** be parsed and converted to id3v2 if necessary.  After the conversion, the
 ** file will remain unchanged, and will continue to do so until you use the
 ** Update() method on the tag (if you choose to Update() at all).
 **
 ** \param name The filename of the mp3 file to link to
 **/
ID3_Tag::ID3_Tag(const char *name)
  : _impl(new ID3_TagImpl(name))
{
}

/** Standard copy constructor.
 **
 ** \param tag What is copied into this tag
 **/
ID3_Tag::ID3_Tag(const ID3_Tag &tag)
  : _impl(new ID3_TagImpl(tag))
{
}

ID3_Tag::~ID3_Tag()
{
  delete _impl;
}

/** Clears the object and disassociates it from any files.
 **
 ** It frees any resources for which the object is responsible, and the
 ** object is now free to be used again for any new or existing tag.
 **/
void ID3_Tag::Clear()
{
  _impl->Clear();
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
 ** 
 ** \code
 **   if (myTag.HasChanged())
 **   {
 **     // render and output the tag
 **   }
 ** \endcode
 ** 
 ** \return Whether or not the tag has been altered.
 **/
bool ID3_Tag::HasChanged() const
{
  return _impl->HasChanged();
}

size_t ID3_Tag::Size() const
{
  return _impl->Size();
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
bool ID3_Tag::SetUnsync(bool b)
{
  return _impl->SetUnsync(b);
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
bool ID3_Tag::SetExtendedHeader(bool ext)
{
  return _impl->SetExtendedHeader(ext);
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
bool ID3_Tag::SetPadding(bool pad)
{
  return _impl->SetPadding(pad);
}

void ID3_Tag::AddFrame(const ID3_Frame& frame)
{
  _impl->AddFrame(frame);
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
void ID3_Tag::AddFrame(const ID3_Frame* frame)
{
  _impl->AddFrame(frame);
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
  _impl->AttachFrame(frame);
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
ID3_Frame* ID3_Tag::RemoveFrame(const ID3_Frame *frame)
{
  return _impl->RemoveFrame(frame);
}

bool ID3_Tag::Parse(ID3_Reader& reader)
{
  return _impl->Parse(reader);
}

size_t ID3_Tag::Parse(const uchar* buffer, size_t bytes)
{
  ID3_MemoryReader mr(reinterpret_cast<const char *>(buffer), bytes);
  ID3_Reader::pos_type beg = mr.getCur();
  this->Parse(mr);
  return mr.getEnd() - beg;
}

/** Turns a binary tag into a series of ID3_Frame objects attached to the
 ** tag.
 ** 
 ** \code
 **   ID3_Tag myTag;
 **   uchar header[ID3_TAGHEADERSIZE];
 **   uchar *buffer;
 **   luint tagSize;
 ** 
 **   // get ID3_TAGHEADERSIZE from a socket or somewhere
 **   // ...
 ** 
 **   if ((tagSize = ID3_IsTagHeader(ourSourceBuffer)) > -1)
 **   {
 **     // read a further 'tagSize' bytes in
 **     // from our data source
 **     // ...
 **   
 **     if (buffer = new uchar[tagSize])
 **     {
 **       // now we will call ID3_Tag::Parse()
 **       // with these values (explained later)
 **       myTag.Parse(header, buffer);
 **       
 **       // do something with the objects,
 **       // like look for titles, artists, etc.
 **       // ...
 **       
 **       // free the buffer
 **       delete [] buffer;
 **     }
 **   }
 ** \endcode
 ** 
 ** \sa ID3_Frame
 ** @param header The byte header read in from the data source.
 ** @param buffer The remainder of the tag (not including the data source) 
 **               read in from the data source.
 **/
size_t ID3_Tag::Parse(const uchar header[ID3_TagHeader::SIZE],
		      const uchar *buffer)
{
  size_t size = ID3_Tag::IsV2Tag(header);
  if (0 == size)
  {
    return 0;
  }
  ::BString buf;
  buf.reserve(ID3_TagHeader::SIZE + size);
  buf.append((char*)header, ID3_TagHeader::SIZE);
  buf.append((char*)buffer, size);
  return this->Parse((uchar*)buf.data(), buf.size());
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
size_t ID3_Tag::Render(uchar* buffer, ID3_TagType tt) const
{
  return _impl->Render(buffer, tt);
}

/** Attaches a file to the tag, parses the file, and adds any tag information
 ** found in the file to the tag.
 ** 
 ** Use this method if you created your ID3_Tag object without supplying a
 ** parameter to the constructor (maybe you created an array of ID3_Tag
 ** pointers).  This is the preferred method of interacting with files, since
 ** id3lib can automatically do things like parse foreign tag formats and
 ** handle padding when linked to a file.  When a tag is linked to a file, you
 ** do not need to use the Size(), Render(const uchar*, size_t), or
 ** Parse(ID3_Reader&) methods or the IsV2Tag(ID3_Reader&) static function--
 ** id3lib will take care of those details for you.  The single parameter is a
 ** pointer to a file name.
 ** 
 ** Link returns the size of the the id3v2 tag (if any) that begins the file.
 ** 
 ** \code
 **   ID3_Tag myTag;
 **   myTag.Link("mysong.mp3");
 **   
 **   // do whatever we want with the tag
 **   // ...
 **   
 **   // setup all our rendering parameters
 **   myTag->SetUnsync(false);
 **   myTag->SetExtendedHeader(true);
 **   myTag->SetCompression(true);
 **   myTag->SetPadding(true);
 **   
 **   // write any changes to the file
 **   myTag->Update()
 **   
 ** \endcode
 ** 
 ** @see IsV2Tag
 ** @param fileInfo The filename of the file to link to.
 **/
size_t ID3_Tag::Link(const char *fileInfo, flags_t flags)
{
  return _impl->Link(fileInfo, flags);
}

flags_t ID3_Tag::Update(flags_t flags)
{
  return _impl->Update(flags);
}

/** Strips the tag(s) from the attached file. The type of tag stripped
 ** can be specified as a parameter.  The default is to strip all tag types.
 ** 
 ** \param tt The type of tag to strip
 ** \sa ID3_TagType
 **/
flags_t ID3_Tag::Strip(flags_t flags)
{
  return _impl->Strip(flags);
}
  
size_t ID3_Tag::GetPrependedBytes() const 
{
  return _impl->GetPrependedBytes();
}

size_t ID3_Tag::GetAppendedBytes() const 
{ 
  return _impl->GetAppendedBytes();
}

size_t ID3_Tag::GetFileSize() const 
{ 
  return _impl->GetFileSize();
}

const char* ID3_Tag::GetFileName() const 
{ 
  return _impl->GetFileName();
}

/// Finds frame with given frame id
ID3_Frame* ID3_Tag::Find(ID3_FrameID id) const
{
  return _impl->Find(id);
}

/// Finds frame with given frame id, fld id, and integer data
ID3_Frame* ID3_Tag::Find(ID3_FrameID id, ID3_FieldID fld, uint32 data) const
{
  return _impl->Find(id, fld, data);
}

/// Finds frame with given frame id, fld id, and ascii data
ID3_Frame* ID3_Tag::Find(ID3_FrameID id, ID3_FieldID fld, const char* data) const
{
  return _impl->Find(id, fld, data);
}

/// Finds frame with given frame id, fld id, and unicode data
ID3_Frame* ID3_Tag::Find(ID3_FrameID id, ID3_FieldID fld, const unicode_t* data) const
{
  return _impl->Find(id, fld, data);
}

size_t ID3_Tag::NumFrames() const
{
  return _impl->NumFrames();
}

ID3_Frame* ID3_Tag::GetFrameNum(index_t index) const
{
  return _impl->GetFrameNum(index);
}

ID3_Frame* ID3_Tag::operator[](index_t index) const
{
  return this->GetFrameNum(index);
}

ID3_Tag& ID3_Tag::operator=( const ID3_Tag &rTag )
{
  if (this != &rTag)
  {
    *_impl = rTag;
  }
  return *this;
}

bool ID3_Tag::GetUnsync() const
{
  return _impl->GetUnsync();
}

bool ID3_Tag::HasTagType(uint16 tt) const
{
  return _impl->HasTagType(16);
}

ID3_V2Spec ID3_Tag::GetSpec() const
{
  return _impl->GetSpec();
}

bool ID3_Tag::SetSpec(ID3_V2Spec spec)
{
  return _impl->SetSpec(spec);
}

/** Analyses a buffer to determine if we have a valid ID3v2 tag header.
 ** If so, return the total number of bytes (including the header) to
 ** read so we get all of the tag
 **/
size_t ID3_Tag::IsV2Tag(const uchar* const data)
{
  ID3_MemoryReader mr(reinterpret_cast<const char*>(data), ID3_TagHeader::SIZE);
  return ID3_TagImpl::IsV2Tag(mr);
}

/// Deprecated
void ID3_Tag::AddNewFrame(ID3_Frame* f) 
{ 
  _impl->AttachFrame(f);
}

void ID3_Tag::AddFrames(const ID3_Frame *frames, size_t numFrames)
{
  _impl->AddFrames(frames, numFrames);
}

size_t ID3_Tag::Link(const char *fileInfo, bool parseID3v1, bool parseLyrics3)
{
  _impl->Link(fileInfo, parseID3v1, parseLyrics3);
}

void ID3_Tag::SetCompression(bool b) 
{ 
  ; 
}

bool ID3_Tag::HasLyrics() const 
{ 
  return this->HasTagType(ID3TT_LYRICS);
}
bool ID3_Tag::HasV2Tag()  const 
{ 
  return this->HasTagType(ID3TT_ID3V2); 
}
bool ID3_Tag::HasV1Tag()  const 
{ 
  return this->HasTagType(ID3TT_ID3V1); 
}

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
ID3_Tag& ID3_Tag::operator<<(const ID3_Frame& frame)
{
  this->AddFrame(frame);
  return *this;
}


ID3_Tag& ID3_Tag::operator<<(const ID3_Frame* frame)
{
  if (frame)
  {
    this->AddFrame(frame);
  }
  return *this;
}

int32 ID3_IsTagHeader(const uchar data[ID3_TAGHEADERSIZE])
{
  size_t size = ID3_Tag::IsV2Tag(data);
  
  if (!size)
  {
    return -1;
  }
  
  return size - ID3_TagHeader::SIZE;
}


