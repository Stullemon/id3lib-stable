// $Id: reader.h,v 1.4 2000/10/03 00:53:04 eldamitri Exp $

// id3lib: a software library for creating and manipulating id3v1/v2 tags
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

#ifndef _ID3LIB_READER_H_
#define _ID3LIB_READER_H_

#include "debug.h"
#include "globals.h"

class ID3_Reader
{
 public:
  typedef size_t streamsize;
  typedef size_t streamoff;
  typedef unsigned char      char_type;
  typedef unsigned long long pos_type;
  typedef long long off_type;
  typedef long long int_type;
  static const int_type END_OF_READER = -1;
  
  /** Close the reader.  Any further actions on the reader should fail.
   **/
  virtual void close() = 0;

  /** Return the beginning position in the reader */
  virtual pos_type getBeg() { return static_cast<pos_type>(0); }

  /** Return the ending position in the reader */
  virtual pos_type getEnd() { return static_cast<pos_type>(-1); }

  /** Return the current position in the reader */
  virtual pos_type getCur() = 0;

  /** Set the value of the current position for reading.
   **/
  virtual pos_type setCur(pos_type pos) = 0;

  /**
   ** Read a single character and advance the internal position.  Note that the
   ** interal position may advance more than one byte for a single character 
   ** read.  Returns END_OF_READER if there isn't a character to read.
   **/
  virtual int_type readChar() 
  {
    if (this->atEnd())
    { 
      return END_OF_READER; 
    }
    char_type ch;
    this->readChars(&ch, 1);
    return ch;
  }

  /**
   ** Return the next character to be read without advancing the internal 
   ** position.  Returns END_OF_READER if there isn't a character to read.
   **/
  virtual int_type peekChar() = 0;
  
  /** Read up to \c len characters into buf and advance the internal position
   ** accordingly.  Returns the number of characters read into buf.  Note that
   ** the value returned may be less than the number of bytes that the internal
   ** position advances, due to multi-byte characters.
   **/
  virtual streamsize readChars(char* buf, streamsize len)
  {
    return this->readChars(reinterpret_cast<uchar *>(buf), len);
  }
  virtual streamsize readChars(uchar buf[], streamsize len) = 0;
  
  /** Skip up to \c len chars in the stream and advance the internal position
   ** accordingly.  Returns the number of characters actually skipped (may be 
   ** less than requested).
   **/
  virtual streamsize skipChars(streamsize len)
  {
    char_type bytes[len];
    return this->readChars(bytes, len);
  }

  virtual streamsize remainingChars()
  {
    pos_type end = this->getEnd(), cur = this->getCur();
    ID3D_NOTICE( "ID3_Reader::remainingChars(): [cur, end] = [" << cur << ", " << end << "]" );
    if (end == pos_type(-1))
    {
      return streamsize(-1);
    }

    if (end >= cur)
    {
      return end - cur;
    }

    return 0;
  }

  virtual bool atEnd()
  {
    return this->getCur() >= this->getEnd();
  }
};

#endif /* _ID3LIB_READER_H_ */
