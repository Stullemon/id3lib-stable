// $Id: reader.h,v 1.1 2000/09/22 04:40:04 eldamitri Exp $

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

class ID3_Reader
{
 public:
  typedef size_t streamsize;
  typedef size_t streamoff;
  typedef unsigned char      char_type;
  typedef unsigned long long pos_type;
  typedef long long off_type;
  typedef long long int_type;
  
  /** Close the reader.  Any further actions on the reader should fail.
   **/
  virtual void close() = 0;

  /** Read a single byte and advance the internal position.  Returns EOF if
   ** there isn't a byte to read.
   **/
  virtual int_type readByte()
  { 
    if (this->peek() == EOF) 
    { 
      return EOF; 
    }
    int_type ch = EOF;
    this->readBytes(&static_cast<char_type>(ch), 1);
    return ch;
  }

  /**
   ** Read a single character and advance the internal position.  Note that the
   ** interal position may advance more than one byte for a single character 
   ** read.  Returns EOF if there isn't a character to read.
   **/
  virtual int_type readChar() { return this->readByte(); }

  /**
   ** Return the next byte to be read without advancing the internal 
   ** position.  Returns EOF if there isn't a byte to read.
   **/
  virtual int_type peek() = 0;
  
  /** Read up to \c len bytes into buf and advance the internal position
   ** accordingly.  Returns the number of bytes read into buf.
   **/
  virtual streamsize readBytes(char_type buf[], streamsize len) = 0;

  /** Read up to \c len characters into buf and advance the internal position
   ** accordingly.  Returns the number of characters read into buf.  Note that
   ** the value returned may be less than the number of bytes that the internal
   ** position advances, due to multi-byte characters.
   **/
  virtual streamsize readChars(char_type buf[], streamsize len)
  { return this->readBytes(buf, len); }
  
  /** Skip up to \c len bytes in the stream and advance the internal position
   ** accordingly.  Returns the number of bytes actually skipped (may be less).
   **/
  virtual int_type skipBytes(streamsize len)
  {
    char_type bytes[len];
    return this->readBytes(bytes, len);
  }

  /** Skip up to \c len characters in the stream and advance the internal
   ** position accordingly.  Returns the number of bytes actually skipped (may
   ** be less).
   **/
  virtual int_type skipChars(streamsize len) { return this->skipBytes(len); }

  /** Returns the value of the internal position.
   **/
  virtual pos_type tell() = 0;

  /** Set the value of the internal position for reading.
   **/
  virtual pos_type seek(pos_type pos) = 0;
};

#endif /* _ID3LIB_READER_H_ */
