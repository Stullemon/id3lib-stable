// $Id: codepageconvert.cpp,v 1.1 2004/04/11 17:10:01 t1mpy Exp $

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


#include <ctype.h>
#include "id3/utils.h" // has <config.h> "id3/id3lib_streams.h" "id3/globals.h" "id3/id3lib_strings.h"

#if (defined(__GNUC__) && __GNUC__ == 2)
#define NOCREATE ios::nocreate
#else
#define NOCREATE ((std::ios_base::openmode)0)
#endif

#ifdef HAVE_ICONV_H
// check if we have all unicodes
# if (defined(ID3_ICONV_FORMAT_UTF16BE) && defined(ID3_ICONV_FORMAT_UTF16) && defined(ID3_ICONV_FORMAT_UTF8) && defined(ID3_ICONV_FORMAT_ISO_8859_1))
#  include <errno.h>
#  include <iconv.h>
# else
#  undef HAVE_ICONV_H
# endif
#endif

#ifndef HAVE_ICONV_H
# if (defined(WIN32) && ((defined(_MSC_VER) && _MSC_VER > 1000) || (defined(__BORLANDC__) && __BORLANDC__  >= 0x0520)))
#  include <mlang.h>
#  define HAVE_MS_CONVERT 1
#  define ID3_MSCODEPAGE_UTF16BE   1201  //"Unicode (Big-Endian)", "unicodeFFFE", 1201
#  define ID3_MSCODEPAGE_UTF16     1200  //"Unicode", "unicode", 1200
#  define ID3_MSCODEPAGE_UTF8      65001 //"Unicode (UTF-8)", "utf-8", 65001
#  define ID3_MSCODEPAGE_ISO8859_1 28591 //"Western European (ISO)", "iso-8859-1", 28591
# else
#  error Id3lib needs iconv and the formats ISO_8859_1, UTF8, UTF16(BE, LE) to work
# endif //if (defined(WIN32) && defined (_MSC_VER) && _MSC_VER > 1000)
#endif

using namespace dami;

#if defined(HAVE_MS_CONVERT)
UINT GetMSCodePage(ID3_TextEnc enc)
{
  switch(enc)
  {
    case ID3TE_ISO8859_1:
      return ID3_MSCODEPAGE_ISO8859_1;
      break;
    case ID3TE_UTF16: //id3lib strips the byte order, hence the actual string becomes ID3TE_UTF16BE
      return ID3_MSCODEPAGE_UTF16BE; // this would be used if it had a unicode two byte byteorder: ID3_MSCODEPAGE_UTF16;
      break;
    case ID3TE_UTF16BE:
      return ID3_MSCODEPAGE_UTF16BE;
      break;
    case ID3TE_UTF8:
      return ID3_MSCODEPAGE_UTF8;
      break;
    default:
      return 0;
      break;
  }
}

ID3_Err msconvert( const String *source, String *target, ID3_TextEnc sourceEnc, ID3_TextEnc targetEnc)
{
  unsigned char *target_str = NULL;
  unsigned char *source_str = NULL;
  UINT target_size = 0;
  UINT source_size = 0;
  IMultiLanguage* mlang = NULL;
  IMLangConvertCharset* conv = NULL;
  HRESULT hResult = S_OK;
  UINT uiSrcCodePage = GetMSCodePage(sourceEnc);
  UINT uiDstCodePage = GetMSCodePage(targetEnc);

  if (uiSrcCodePage == uiDstCodePage) //this can happen when converting between ID3TE_UTF16BE and ID3TE_UTF16, since the byteorder is stripped they are treated equally
  {
    target->append(*source);
    return ID3E_NoError;
  }

  if (uiSrcCodePage == 0 || uiDstCodePage == 0)
    return ID3E_UnknownEncoding;

  //initialize com
  hResult = CoInitialize(NULL);
  if (hResult != S_OK)
  {
    CoUninitialize();
    return ID3E_ConvertInitError;
  }

  hResult = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage, (void**) &mlang);
  if (hResult != S_OK || NULL == mlang)
  {
    CoUninitialize();
    return ID3E_ConvertInitError;
  }

  hResult = mlang->CreateConvertCharset(uiSrcCodePage, uiDstCodePage, 0, (struct IMLangConvertCharset **) &conv);
  if ( hResult != S_OK || NULL == conv )
  {
    CoUninitialize();
    return ID3E_ConvertInitError;
  }

  source_str = (unsigned char*)source->data();
  source_size = source->size();
  target_str = new unsigned char[(4 * source_size) + 4];//plenty of space
  target_size = (4 * source_size) + 4; //plus two bytes for byteorder header, plus 2 bytes for double NULL to end the string

  hResult = conv->DoConversion(source_str, &source_size, target_str, &target_size);
  if ( hResult != S_OK )
  {
    CoUninitialize();
    delete target_str;
    return ID3E_ConvertError;
  }

  CoUninitialize();
  target->append((char *)target_str, target_size);
  delete target_str;
  return ID3E_NoError;
}
#else
const char* getIconvFormat(ID3_TextEnc enc)
{
  const char* format = NULL;
  switch (enc)
  {
    case ID3TE_ISO8859_1:
      format = ID3_ICONV_FORMAT_ISO_8859_1;
      break;
    case ID3TE_UTF16:
      format = ID3_ICONV_FORMAT_UTF16;
      break;
    case ID3TE_UTF16BE:
      format = ID3_ICONV_FORMAT_UTF16BE;
      break;
    case ID3TE_UTF8:
      format = ID3_ICONV_FORMAT_UTF8;
      break;
    default:
      break;
  }
  return format;
}
#define ID3LIB_BUFSIZ 1024
ID3_Err iconvconvert( const String *source, String *target, ID3_TextEnc sourceEnc, ID3_TextEnc targetEnc)
{
  ID3_Err e;
  char *target_str = NULL;
  char *source_str = NULL;
  size_t target_size = ID3LIB_BUFSIZ;
  size_t source_size = source->size();
  const char* targetFormat = getIconvFormat(targetEnc);
  const char* sourceFormat = getIconvFormat(sourceEnc);

  //get a conversion desciptor
  iconv_t cd = iconv_open (targetFormat, sourceFormat);
  if (cd == (iconv_t)-1)
  {
    return ID3E_ConvertInitError;
  }

  //create buffer for source
  source_str = new char[source_size];
  source->copy(source_str, String::npos);
  //create buffer for target
  target_str = new char[target_size];

  //we need to copy these pointers, since iconv will higher then in each loop
#ifdef ID3LIB_ICONV_CONSTSOURCE
  const char* source_buf = source_str;
#else
  char* source_buf = source_str;
#endif
  char* target_buf = target_str;

  do
  {
    errno = 0;
    size_t nconv = iconv(cd,
                         &source_buf,
                         &source_size,
                         &target_buf,
                         &target_size);

    if (nconv == (size_t) -1 && errno != EINVAL && errno != E2BIG)
    {
      // errno is probably EILSEQ here, which means either an invalid byte sequence or a valid but unconvertible byte sequence
        delete source_str;
        delete target_str;
        return ID3E_ConvertError;
    }
      // printf("Debug convert_i 3: %d %d %d:%s\n", nconv, source_size, target_size, target_str );
    //add it to target
    target->append(target_str, ID3LIB_BUFSIZ - target_size);
    //reuse buffer for next run
    target_buf  = target_str;
    target_size = ID3LIB_BUFSIZ;
  } while (source_size > 0);

  //the string target should now have been filled
  iconv_close (cd);
  delete source_str;
  delete target_str;

  return ID3E_NoError;
}

#endif //defined(HAVE_MS_CONVERT)

//codepage convert; converts between codepages
ID3_Err dami::codepageconvert( const String *source, String *target, ID3_TextEnc sourceEnc, ID3_TextEnc targetEnc)
{
  if (!source || source->size() == 0)
    return ID3E_NoData;
  if (!target)
    return ID3E_NoBuffer;
  if (target->size())
    return ID3E_BufferNotEmpty;
  if (sourceEnc == targetEnc)
  {
    target->append(*source);
    return ID3E_NoError;
  }

#if defined(HAVE_MS_CONVERT)
  return msconvert(source, target, sourceEnc, targetEnc);
#else
  return iconvconvert(source, target, sourceEnc, targetEnc);
#endif //defined(HAVE_MS_CONVERT)
}

