// $Id: tag_render.cpp,v 1.7 1999/11/25 18:51:34 scott Exp $

//  The authors have released ID3Lib as Public Domain (PD) and claim no
//  copyright, patent or other intellectual property protection in this work.
//  This means that it may be modified, redistributed and used in commercial
//  and non-commercial software and hardware without restrictions.  ID3Lib is
//  distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
//  express or implied.
//
//  The ID3Lib authors encourage improvements and optimisations to be sent to
//  the ID3Lib coordinator, currently Dirk Mahoney (dirk@id3.org).  Approved
//  submissions may be altered, and will be included and released under these
//  terms.
//
//  Mon Nov 23 18:34:01 1998

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <memory.h>
#include <id3/tag.h>
#include <id3/misc_support.h>
#include <stdlib.h>

luint ID3_Tag::Render(uchar *buffer)
{
  luint bytesUsed = 0;
  
  if (NULL == buffer)
    ID3_THROW(ID3E_NoBuffer);

  ID3_Elem *cur = __pFrameList;
  ID3_TagHeader header;
    
  SetVersion(ID3_TAGVERSION, ID3_TAGREVISION);
    
  header.SetVersion(__ucVersion, __ucRevision);
  bytesUsed += header.Size();
    
  // set up the encryption and grouping IDs
    
  // ...
    
  while (cur)
  {
    if (cur->pFrame != NULL)
    {
      cur->pFrame->__bCompression = __bCompression;
      cur->pFrame->SetVersion(__ucVersion, __ucRevision);
      bytesUsed += cur->pFrame->Render(&buffer[bytesUsed]);
    }
      
    cur = cur->pNext;
  }
    
  if (__bSyncOn)
  {
    uchar *tempz;
    luint newTagSize;
    
    newTagSize = GetUnSyncSize(&buffer[header.Size()], 
                               bytesUsed - header.Size());
    if (newTagSize > 0 && (newTagSize + header.Size()) > bytesUsed)
    {
      tempz = new uchar[newTagSize];
      if (NULL == tempz)
        ID3_THROW(ID3E_NoMemory);

      UnSync(tempz, newTagSize, &buffer[header.Size()],
             bytesUsed - header.Size());
      header.SetFlags(ID3HF_UNSYNC);

      memcpy(&buffer[header.Size()], tempz, newTagSize);
      bytesUsed = newTagSize + header.Size();
      delete[] tempz;
    }
  }
    
  // zero the remainder of the buffer so that our padding bytes are zero
  luint nPadding = PaddingSize(bytesUsed);

  memset(&buffer[bytesUsed], '\0', nPadding);
  bytesUsed += nPadding;
    
  header.SetDataSize(bytesUsed - header.Size());
  header.Render(buffer);
    
  // set the flag which says that the tag hasn't changed
  __bHasChanged = false;
  
  return bytesUsed;
}

luint ID3_Tag::Size(void) const
{
  luint bytesUsed = 0;
  ID3_Elem *cur = __pFrameList;
  ID3_TagHeader header;
  
  header.SetVersion(__ucVersion, __ucRevision);
  bytesUsed += header.Size();
  
  while (cur)
  {
    if (cur->pFrame)
    {
      cur->pFrame->SetVersion(__ucVersion, __ucRevision);
      bytesUsed += cur->pFrame->Size();
    }
    
    cur = cur->pNext;
  }
  
  // add 30% for sync
  if (__bSyncOn)
    bytesUsed += bytesUsed / 3;
    
  bytesUsed += PaddingSize(bytesUsed);
  
  return bytesUsed;
}


void ID3_Tag::RenderExtHeader(uchar *buffer)
{
  if (__ucVersion == 3 && __ucRevision == 0)
  {}
  
  return ;
}


luint ID3_Tag::RenderV1(char *buffer)
{
  // Sanity check our buffer
  if (NULL == buffer)
    ID3_THROW(ID3E_NoBuffer);

  // pCur is used to mark where to next write in the buffer
  // sTemp is used as a temporary string pointer for functions that return
  //  dynamically created strings
  char *pCur = buffer, *sTemp = NULL;

  // The default char for a v1 tag is null
  // Note: Using strncpy should make this unnecessary, as strncpy will pad
  //  the destination string with nulls.  But unless it becomes a performance
  //  issue (I doubt it, for 128 bit strings), its an extra layer of
  //  protection.
  memset(buffer, '\0', LEN_V1);

  // Write the TAG identifier
  strncpy(pCur, "TAG", LEN_V1_ID);
  pCur = &pCur[LEN_V1_ID];

  // Write the TITLE
  sTemp = ID3_GetTitle(this);
  if (sTemp != NULL)
  {
    strncpy(pCur, sTemp, LEN_V1_TITLE);
    delete [] sTemp;
  }
  pCur = &pCur[LEN_V1_TITLE];

  // Write the ARTIST
  sTemp = ID3_GetArtist(this);
  if (sTemp != NULL)
  {
    strncpy(pCur, sTemp, LEN_V1_ARTIST);
    delete [] sTemp;
  }
  pCur = &pCur[LEN_V1_ARTIST];

  // Write the ALBUM
  sTemp = ID3_GetAlbum(this);
  if (sTemp != NULL)
  {
    strncpy(pCur, sTemp, LEN_V1_ALBUM);
    delete [] sTemp;
  }
  pCur = &pCur[LEN_V1_ALBUM];

  // Write the YEAR
  sTemp = ID3_GetYear(this);
  if (sTemp != NULL)
  {
    strncpy(pCur, sTemp, LEN_V1_YEAR);
    delete [] sTemp;
  }
  pCur = &pCur[LEN_V1_YEAR];

  // Write the COMMENT
  sTemp = ID3_GetComment(this);
  if (sTemp != NULL)
  {
    strncpy(pCur, sTemp, LEN_V1_COMMENT);
    delete [] sTemp;
  }
  pCur = &pCur[LEN_V1_COMMENT];

  // Write the TRACK, if it isn't 0
  luint nTrack = ID3_GetTrackNum(this);
  if (0 != nTrack)
  {
    pCur -= 2;
    pCur[0] = '\0';
    pCur[1] = (uchar) nTrack;
    pCur += 2;
  }

  // Write the GENRE
  pCur[0] = (uchar) ID3_GetGenreNum(this);

  return LEN_V1;
}

void ID3_Tag::RenderV1ToHandle(void)
{
  ID3V1_Tag tagV1;
  char sTag[LEN_V1];
  char sID[LEN_V1_ID];

  RenderV1(sTag);

  // We want to check if there is already an id3v1 tag, so we can write over
  // it.  First, seek to the beginning of any possible id3v1 tag
  if (fseek(__fFileHandle, -LEN_V1, SEEK_END) != 0)
    // TODO:  This is a bad error message.  Make it more descriptive
    ID3_THROW(ID3E_NoData);

  // Read in the TAG characters
  if (fread(sID, 1, LEN_V1_ID, __fFileHandle) != LEN_V1_ID)
    // TODO:  This is a bad error message.  Make it more descriptive
    ID3_THROW(ID3E_NoData);

  // If those three characters are TAG, then there's a preexisting id3v1 tag,
  // so we should set the file cursor so we can overwrite it with a new tag.
  if (memcmp(sID, "TAG", LEN_V1_ID) == 0)
  {
    if (fseek(__fFileHandle, -LEN_V1, SEEK_END) != 0)
      // TODO:  This is a bad error message.  Make it more descriptive
      ID3_THROW(ID3E_NoData);
  }
  // Otherwise, set the cursor to the end of the file so we can append on 
  // the new tag.
  else
  {
    if (fseek(__fFileHandle, 0, SEEK_END) != 0)
      // TODO:  This is a bad error message.  Make it more descriptive
      ID3_THROW(ID3E_NoData);
  }

  fwrite(sTag, 1, LEN_V1, __fFileHandle);
  __bHasV1Tag = true;
}

void ID3_Tag::RenderV2ToHandle(void)
{
  uchar *buffer;
  luint size = Size();
  
  if (NULL == __fFileHandle)
    ID3_THROW(ID3E_NoData);

  if (size == 0)
    return;

  buffer = new uchar[size];
  if (NULL == buffer)
    ID3_THROW(ID3E_NoMemory);

  size = Render(buffer);      
  if (0 == size)
  {
    delete [] buffer;
    return;
  }

  // if the new tag fits perfectly within the old and the old one
  // actually existed (ie this isn't the first tag this file has had)
  if ((0 == __ulOldTagSize && 0 == __ulFileSize) || 
      (size == __ulOldTagSize && size != 0))
  {
    fseek(__fFileHandle, 0, SEEK_SET);
    fwrite(buffer, 1, size, __fFileHandle);
    __ulOldTagSize = size;
  }
  else
  {
    // else we gotta make a temp file, copy the tag into it, copy the
    // rest of the old file after the tag, delete the old file, rename
    // this new file to the old file's name and update the __fFileHandle
            
    uchar buffer2[BUFF_SIZE];
    FILE *tempOut = tmpfile();
    if (NULL == tempOut)
      ID3_THROW(ID3E_ReadOnly);

    fwrite(buffer, 1, size, tempOut);
                
    fseek(__fFileHandle, __ulOldTagSize, SEEK_SET);
      
    while (! feof(__fFileHandle))
    {
      size_t nBytes = fread(buffer2, 1, BUFF_SIZE, __fFileHandle);
      fwrite(buffer2, 1, nBytes, tempOut);
    }
      
    rewind(tempOut);
    freopen(__sFileName, "w+", __fFileHandle);

    while (!feof(tempOut))
    {
      size_t nBytes = fread(buffer2, 1, BUFF_SIZE, tempOut);
      fwrite(buffer2, 1, nBytes, __fFileHandle);
    }

    fclose(tempOut);
    
    __ulOldTagSize = size;
  }
        
  delete[] buffer;
    
  return ;
}


#define ID3_PADMULTIPLE (2048)
#define ID3_PADMAX  (4096)


luint ID3_Tag::PaddingSize(luint curSize) const
{
  luint newSize = 0;
  
  // if padding is switched off or there is no attached file
  if (! __bPadding || __ulFileSize == 0)
    return 0;
    
  // if the old tag was large enough to hold the new tag, then we will simply
  // pad out the difference - that way the new tag can be written without
  // shuffling the rest of the song file around
  if (__ulOldTagSize && (__ulOldTagSize > curSize) && 
      (curSize - __ulOldTagSize) < ID3_PADMAX)
  {
    newSize = __ulOldTagSize;
  }
  else
  {
    luint tempSize = curSize + __ulFileSize;
    
    // this method of automatic padding rounds the COMPLETE FILE up to the
    // nearest 2K.  If the file will already be an even multiple of 2K (with
    // the tag included) then we just add another 2K of padding
    tempSize = ((tempSize / ID3_PADMULTIPLE) + 1) * ID3_PADMULTIPLE;
    
    // the size of the new tag is the new filesize minus the song size
    newSize = tempSize - __ulFileSize;
  }
  
  return newSize - curSize;
}



// $Log: tag_render.cpp,v $
// Revision 1.7  1999/11/25 18:51:34  scott
// (Render): Replaced manual array initialization with call to memset.
// (GenerateTempName): Removed method.  No longer used.
// (RenderV1): New method.  Renders an id3v1 tag to an array.
// (RenderV1ToHandle): Moved work of writing tag to array to RenderV1.
// (RenderV2ToHandle): Rather than generating a temporary file name with
// mktemp (or similar) and opening that file name to a temporary file,
// we now use tmpfile to create a temporary file directly.  Unfortunately,
// this means we can't just rename the tempfile to the original file
// when all the copying is done, so we have to perform a second copy from
// the temp file back to the original.  This makes the code less efficient
// but more portable, as we rely on tmpfile for knowing where to create
// the temporary file.  To improve, we'll need to figure out a portable
// mechanism for creating temporary files (i.e., resolve the differences
// between /tmp/ and C:\temp\).
//
// Revision 1.6  1999/11/19 19:27:27  scott
// (RenderV1ToHandle): Added.  Adds or overwrites an id3v1 tag to a file
// handle.  Needs testing.
// (RenderV2ToHandle): Renamed from RenderToHandle.
//
// Revision 1.5  1999/11/15 20:21:29  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.  Now uses mktemp rather than mkstemp in
// GenerateTempName so that a file descriptor isn't created along
// with the temporary name.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
