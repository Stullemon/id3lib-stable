// $Id: tag_sync.cpp,v 1.3 2000/05/01 13:05:07 eldamitri Exp $

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

#include "tag.h"

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

// To be used when reading an ID3v2-tag
// Transforms all FF 00 sequences into FF

luint ID3_Tag::ReSync(uchar *pData, luint nDataSize)
{
  uchar *pSrc, *pDest;
  for (pSrc = pDest = pData; pSrc < pData + nDataSize - 1; pSrc++, pDest++)
  {
    *pDest = *pSrc;
    if (0xFF == pSrc[0] && 0x00 == pSrc[1])
    {
      pSrc++;
    }
  }
  
  return nDataSize - (pSrc - pDest);
}

// Determine if pCur is at a point in the pStart buffer where unsyncing is 
// necessary
bool ShouldUnsync(const uchar *pCur, const uchar *pStart, const size_t nSize)
{
  // The current byte is a sync if it's equal to 0xFF and 
  // 1) It's the last byte in the file, or
  // 2) It comes before 111xxxxx (second part of an mp3 sync), or
  // 3) It comes before 00000000 (consequence of unsyncing)
  return
    ( pCur           >= pStart )           &&
    ( pCur            < pStart + nSize)    &&
    ( pCur[0]        == 0xFF)              && // first sync
    ((pCur    + 1    == (pStart + nSize))  || // last byte?
     (pCur[1] & 0xE0 == 0xE0)              || // second sync
     (pCur[1]        == 0x00));               // second null
}

// How big will the tag be after we unsync?
luint ID3_Tag::GetUnSyncSize(uchar *pBuffer, luint nSize)
{
  size_t nNewSize = nSize;
  
  // Determine the size needed for the destination data
  for (uchar *pCur = pBuffer; pCur < pBuffer + nSize; pCur++)
  {
    if (ShouldUnsync(pCur, pBuffer, nSize))
    {
      nNewSize++;
    }
  }
  
  return nNewSize;
}


// To be used when writing an ID3v2-tag
// Transforms:
// 11111111 111xxxxx -> 11111111 00000000 111xxxxx
// 11111111 00000000 -> 11111111 00000000 00000000
// 11111111 <EOF> -> 11111111 00000000 <EOF>

void ID3_Tag::UnSync(uchar *pDestData, luint nDestSize, uchar *pSrcData, luint nSrcSize)
{
  // Now do the real transformation
  for (uchar *pSrc = pSrcData, *pDest = pDestData; 
       (pSrc < pSrcData + nSrcSize) && (pDest < pDestData + nDestSize);
       pSrc++, pDest++)
  {
    // Copy the current character from source to destination
    *pDest = *pSrc;

    // If we're at a sync point in the source, insert an extra null character
    // in the destination buffer
    if (ShouldUnsync(pSrc, pSrcData, nSrcSize))
    {
      pDest++;
      *pDest = 0x00;
    }
  }
}
