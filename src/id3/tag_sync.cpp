// $Id: tag_sync.cpp,v 1.8 1999/12/17 16:12:03 scott Exp $
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



// $Log: tag_sync.cpp,v $
// Revision 1.8  1999/12/17 16:12:03  scott
// Updated opening comment block.
// (ShouldUnsync): Added for use with GetUnSyncSize and UnSync.
// (ReSync, GetUnSyncSize, UnSync): Renamed variables.  Restructured code.
//
// Revision 1.7  1999/12/01 18:00:59  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places (and to make compiling under
// windows easier).
//
// Revision 1.6  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.5  1999/11/15 20:21:36  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
