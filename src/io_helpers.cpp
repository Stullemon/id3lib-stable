// $Id: io_helpers.cpp,v 1.1 2000/10/14 18:45:45 eldamitri Exp $

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

#include "debug.h"

#include "io_decorators.h"
#include "io_helpers.h"
#include "utils.h"

using namespace dami;

String io::readString(ID3_Reader& reader)
{
  String str;
  while (!reader.atEnd())
  {
    ID3_Reader::char_type ch = reader.readChar();
    if (ch == '\0')
    {
      break;
    }
    str += static_cast<char>(ch);
  }
  return str;
}

String io::readText(ID3_Reader& reader, size_t len)
{
  String str;
  str.reserve(len);
  const size_t SIZE = 1024;
  ID3_Reader::char_type buf[SIZE];
  size_t remaining = len;
  while (remaining > 0 && !reader.atEnd())
  {
    size_t numRead = reader.readChars(buf, min(remaining, SIZE));
    remaining -= numRead;
    str.append(reinterpret_cast<String::value_type *>(buf), numRead);
  }
  return str;
}

String io::readUnicodeString(ID3_Reader& reader)
{
  String str;
  while (!reader.atEnd())
  {
    ID3_Reader::char_type ch1 = reader.readChar();
    if (reader.atEnd())
    {
      break;
    }
    ID3_Reader::char_type ch2 = reader.readChar();
    if (ch1 == '\0' && ch2 == '\0')
    {
      break;
    }
    str += static_cast<char>(ch1);
    str += static_cast<char>(ch2);
  }
  return str;
}

String io::readUnicodeText(ID3_Reader& reader, size_t len)
{
  return readText(reader, len * 2);
}

BString io::readAllBinary(ID3_Reader& reader)
{
  return readBinary(reader, reader.remainingBytes());
}

BString io::readBinary(ID3_Reader& reader, size_t len)
{
  BString binary;
  binary.reserve(len);
  
  size_t remaining = len;
  const size_t SIZE = 1024;
  ID3_Reader::char_type buf[SIZE];
  while (!reader.atEnd() && remaining > 0)
  {
    size_t numRead = reader.readChars(buf, min(remaining, SIZE));
    remaining -= numRead;
    binary.append(reinterpret_cast<BString::value_type *>(buf), numRead);
  }
  
  return binary;
}

uint32 io::readLENumber(ID3_Reader& reader, size_t len)
{
  uint32 val = 0;
  for (size_t i = 0; i < len; i++)
  {
    if (reader.atEnd())
    {
      break;
    }
    val += (static_cast<uint32>(0xFF & reader.readChar()) << (i * 8));
  }
  return val;
}

uint32 io::readBENumber(ID3_Reader& reader, size_t len)
{
  uint32 val = 0;
  
  for (ID3_Reader::size_type i = 0; i < len && !reader.atEnd(); ++i)
  {
    val *= 256; // 2^8
    val += static_cast<uint32>(0xFF & reader.readChar());
  }
  return val;
}

String io::readTrailingSpaces(ID3_Reader& reader, size_t len)
{
  io::WindowedReader wr(reader, len);
  String str;
  String spaces;
  str.reserve(len);
  spaces.reserve(len);
  while (!wr.atEnd())
  {
    ID3_Reader::char_type ch = wr.readChar();
    if (ch == '\0' || ch == ' ')
    {
      spaces += ch;
    }
    else
    {
      str += spaces + (char) ch;
      spaces.erase();
    }
  }
  return str;
}

size_t io::writeBENumber(ID3_Writer& writer, uint32 val, size_t len)
{
  ID3_Writer::char_type bytes[sizeof(uint32)];
  ID3_Writer::size_type size = min<ID3_Reader::size_type>(len, sizeof(uint32));
  renderNumber(bytes, val, size);
  return writer.writeChars(bytes, size);
}

ID3_Writer::size_type io::writeTrailingSpaces(ID3_Writer& writer, String buf, size_t len)
{
  ID3_Writer::size_type strLen = buf.size();
  ID3_Writer::size_type size = min(len, strLen);
  writer.writeChars(buf.data(), size);
  for (; size < len; ++size)
  {
    writer.writeChar(' ');
  }
}
