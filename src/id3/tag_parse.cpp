// $Id: tag_parse.cpp,v 1.6 1999/11/16 22:50:37 scott Exp $

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

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <id3/tag.h>
#include <zlib.h>

ID3_Elem *ID3_Tag::GetLastElem(ID3_Elem *list)
{
  ID3_Elem *last;
  
  last = list;
  
  while (last != NULL && last->pNext != NULL)
    last = last->pNext;
    
  return last;
}

void ID3_Tag::AddBinary(uchar *buffer, luint size)
{
  uchar *newBin;
  
  if (0 == size)
    ID3_THROW(ID3E_NoData);

  newBin = new uchar[size];
  if (NULL == newBin)
    ID3_THROW(ID3E_NoMemory);

  ID3_Elem *elem, *lastElem;
      
  memcpy(newBin, buffer, size);
      
  elem = new ID3_Elem;
  if (NULL == elem)
    ID3_THROW(ID3E_NoMemory);

  elem->pNext = NULL;
  elem->pFrame = NULL;
  elem->acBinary = newBin;
  elem->bTagOwns = true;
        
  lastElem = GetLastElem(__pBinaryList);
        
  if (NULL == lastElem)
    __pBinaryList = elem;
  else
    lastElem->pNext = elem;
    
  return ;
}

void ID3_Tag::ExpandBinaries(uchar *buffer, luint size)
{
  ID3_FrameAttr attr;
  ID3_FrameHeader frHeader;
  luint posn = 0;
  
  // TODO: 6 is a magic number!  Find a suitable constant
  while (posn < (size - 6) && buffer[posn] != 0)
  {
    luint newBinSize;
    
    frHeader.SetVersion(__ucVersion, __ucRevision);
    
    posn += frHeader.GetFrameInfo(attr, &buffer[posn]);
    newBinSize = frHeader.Size() + attr.ulSize;
    
    // firstly, let's check to see if we are parsing a CDM.
    if (strcmp(attr.sTextID, "CDM") != 0)
    {
      AddBinary(&buffer[posn - frHeader.Size()], 
                attr.ulSize + frHeader.Size());
      posn += attr.ulSize;
    }
    else
    {
      //If so, let's expand it out now.
      // if the method was zlib
      if ('z' == buffer[posn])
      {
        luint expandedSize = 0;
        uchar *expBin;
        
        expandedSize |= buffer[posn + 1] << 24;
        expandedSize |= buffer[posn + 2] << 16;
        expandedSize |= buffer[posn + 3] << 8;
        expandedSize |= buffer[posn + 4];
        
        expBin = new uchar[expandedSize];
        if (NULL == expBin)
          ID3_THROW(ID3E_NoMemory);

        uncompress(expBin, &expandedSize, 
                   &buffer[posn + 1 + sizeof(luint)], 
                   attr.ulSize - sizeof(luint) - 1);
          
        ExpandBinaries(expBin, expandedSize);
          
        posn += attr.ulSize;
          
        delete[] expBin;
      }
    }
  }
  
  return ;
}

void ID3_Tag::ProcessBinaries(ID3_FrameID whichFrame, bool attach)
{
  ID3_FrameAttr attr;
  ID3_FrameHeader frHeader;
  ID3_Elem *cur = __pBinaryList;

  frHeader.SetVersion(__ucVersion, __ucRevision);
  
  while (cur != NULL && cur->acBinary)
  {
    ID3_FrameID id;
    ID3_Frame *frame;
    luint posn;
    luint extras = 0;
    luint expandedSize = 0;
    uchar groupingID = 0;
    uchar encryptionID = 0;
    bool bShouldAttach = attach;
    
    posn = frHeader.GetFrameInfo(attr, cur->acBinary);
    
    if (attr.ulFlags & ID3FL_COMPRESSION)
    {
      expandedSize |= cur->acBinary[posn + 0] << 24;
      expandedSize |= cur->acBinary[posn + 1] << 16;
      expandedSize |= cur->acBinary[posn + 2] << 8;
      expandedSize |= cur->acBinary[posn + 3];
      
      extras += sizeof(luint);
    }
    
    if (attr.ulFlags & ID3FL_ENCRYPTION)
    {
      encryptionID = cur->acBinary[posn];
      posn++, extras++;
    }
    
    if (attr.ulFlags & ID3FL_GROUPING)
    {
      groupingID = cur->acBinary[posn];
      posn++, extras++;
    }
    id = ID3_FindFrameID(attr.sTextID);
    
    if ((id != whichFrame && ID3FID_NOFRAME != whichFrame) ||
        ID3FID_NOFRAME == id)
    {
      cur = cur->pNext;
    }
    else
    {
      uchar *expBin = NULL;
      
      if (attr.ulFlags & ID3FL_COMPRESSION)
      {
        expBin = new uchar[expandedSize];
        if (NULL == expBin)
          ID3_THROW(ID3E_NoMemory);
          
        uncompress(expBin, &expandedSize, &cur->acBinary[posn + sizeof(luint)],
                   attr.ulSize - extras);
      }
      
      frame = new ID3_Frame;
      if (NULL == frame)
        ID3_THROW(ID3E_NoMemory);
        
      ID3_Elem *elem, *lastElem;
      
      frame->SetID(id);
      
      try 
      {
        if (NULL == expBin)
        {
          frame->Parse(&cur->acBinary[posn], attr.ulSize - extras);
        }
        else
        {
          frame->Parse(expBin, expandedSize);
          delete[] expBin;
        }
        
        // here is where we call a special handler for this frame type if one
        // is specified in the frame definition
        {
          ID3_FrameDef *frameInfo;
        
          frameInfo = ID3_FindFrameDef(id);
        
          if (frameInfo != NULL && frameInfo->parseHandler != NULL)
            bShouldAttach = frameInfo->parseHandler(frame);
        }
      }
      catch (ID3_Error err)
      {
        // There's been an error in the parsing of the frame.  It shouldn't be
        // attached.
        // This should be logged somehow so that the user can determine how
        // many frames were parsed correctly and how many weren't
        // cerr << "xxx Error occurred: " << err.GetErrorDesc() << endl;
        bShouldAttach = false;
        if (NULL != expBin)
          delete [] expBin;
      }
      
      if (!bShouldAttach)
      {
        // if not, delete it
        delete frame;
      }
      else
      {
        // if, after all is said and done, we are still supposed to attach our
        // newly parsed frame to the tag, do so
        elem = new ID3_Elem;
        if (NULL == elem)
          ID3_THROW(ID3E_NoMemory);
          
        elem->pNext = NULL;
        elem->pFrame = frame;
        elem->acBinary = NULL;
        elem->bTagOwns = true;
        
        lastElem = GetLastElem(__pFrameList);
        
        if (lastElem)
          lastElem->pNext = elem;
        else
          __pFrameList = elem;
      }
      
      ID3_Elem *temp = cur;
      cur = cur->pNext;
      
      RemoveFromList(temp, &__pBinaryList);
    }
  }
  
  return ;
}


void ID3_Tag::Parse(uchar header[ID3_TAGHEADERSIZE], uchar *buffer)
{
  luint tagSize = 0;
  int28 temp = &header[6];
  luint posn = 0;
  uchar prevVer = __ucVersion;
  uchar prevRev = __ucRevision;
  
  Clear();
  
  tagSize = temp.get();
  
  SetVersion(header[3], header[4]);
  
  if (header[5] & ID3HF_UNSYNC)
    tagSize = ReSync(buffer, tagSize);
    
  // okay, if we are 2.01, then let's skip over the extended header for now
  // because I am lazy
  if (2 == __ucVersion && 1 == __ucRevision)
    if (header[5] & ID3HF_EXTENDEDHEADER)
    {
      luint extSize = 0;
      
      extSize |= buffer[0] << 24;
      extSize |= buffer[1] << 16;
      extSize |= buffer[2] << 8;
      extSize |= buffer[3] << 0;
      
      posn = extSize + sizeof(luint);
    }
    
  // okay, if we are 3.00, then let's actually parse the extended header (for
  // now, we skip it because we are lazy)
  if (3 == __ucVersion && 0 == __ucRevision)
    if (header[5] & ID3HF_EXTENDEDHEADER)
    {
      luint extSize = 0;
      
      extSize |= buffer[0] << 24;
      extSize |= buffer[1] << 16;
      extSize |= buffer[2] << 8;
      extSize |= buffer[3] << 0;
      
      posn = extSize + sizeof(luint);
    }
    
  // this call will convert the binary data block (tag) into a linked list of
  // binary frames
  ExpandBinaries(&buffer[posn], tagSize);
  
  // let's parse the CRYPTO frames. the 'false' parameter means "don't attach
  // the frame to the tag when processed".  This is because we have installed a
  // parsing handler for the crypto reg frame.  This is a default parameter -
  // if the frame type has a custom parsing handler, that handler will tell
  // ID3Lib whether to attach or not.
  ProcessBinaries(ID3FID_CRYPTOREG, false);
  
  // let's parse the GROUPING frames. the 'false' parameter means "don't attach
  // the frame to the tag when processed".  This is because we have installed a
  // parsing handler for the crypto reg frame.  This is a default parameter -
  // if the frame type has a custom parsing handler, that handler will tell
  // ID3Lib whether to attach or not.
  ProcessBinaries(ID3FID_GROUPINGREG, false);
  
  // let's parse the rest of the binaries
  ProcessBinaries();
  
  // reset the version parameters which were in effect before the parse
  SetVersion(prevVer, prevRev);
  
  // set the flag which says that the tag hasn't changed
  __bHasChanged = false;
  
  return ;
}


luint ID3_Tag::ParseFromHandle(void)
{
  luint size = 0;

  if (NULL == __fFileHandle)
    ID3_THROW(ID3E_NoData);
  else
  {
    uchar header[ID3_TAGHEADERSIZE];
    lsint tagSize;
    
    if (fseek(__fFileHandle, 0, SEEK_SET) != 0)
      ID3_THROW_DESC(ID3E_NoFile, 
                     "ID3_Tag::ParseFromHandle: Ack! Couldn't seek");
      
    if (fread(header, 1, sizeof(header), __fFileHandle) == 0)
      ID3_THROW_DESC(ID3E_NoFile, 
                     "ID3_Tag::ParseFromHandle: Ack! Couldn't read");

    tagSize = ID3_IsTagHeader(header);
    if (tagSize > 0)
    {
      uchar * bin;
        
      bin = new uchar[tagSize];
      if (NULL == bin)
        ID3_THROW(ID3E_NoMemory);

      if (fread(bin, 1, tagSize, __fFileHandle) == 0)
        ID3_THROW_DESC(ID3E_NoFile, 
                       "ID3_Tag::ParseFromHandle: Ack! Couldn't read");

      Parse(header, bin);
      size = tagSize;
          
      delete[] bin;
    }
    
    ParseLyrics3();
    ParseID3v1();
  }
    
  return size;

}

// $Log: tag_parse.cpp,v $
// Revision 1.6  1999/11/16 22:50:37  scott
// * tag_parse.cpp
// (ProcessBinaries): Added try/catch block to catch any exceptions so
// that the parser can carry on parsing the rest of the frames if any
// particular frame is poorly encoded.  Need to add some sort of
// mechanism to determine how many frames exist, how many have been
// parsed correctly and how many have been parsed incorrectly.
//
// Revision 1.5  1999/11/15 20:20:55  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
