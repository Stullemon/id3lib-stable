// $Id: tag_file.cpp,v 1.5 1999/11/19 19:09:12 scott Exp $

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
#include <id3/tag.h>

bool exists(char *name)
{
  bool doesExist = false;
  FILE *in = NULL;
  
  if (NULL == name)
    ID3_THROW(ID3E_NoData);

  in = fopen(name, "rb");
  doesExist = (NULL != in);
  if (doesExist)
    fclose(in);
    
  return doesExist;
}

void ID3_Tag::OpenLinkedFile(void)
{
  if (!exists(__sFileName))
    // Create a new file if the filename doesn't exist
    __fFileHandle = fopen(__sFileName, "wb+");
  else
  {
    // Try to open the file for reading and writing. If that doesn't work, open
    // it for just reading.
    __fFileHandle = fopen(__sFileName, "rb+");
    __bFileWritable = (__fFileHandle != NULL);
    if (!__bFileWritable)
    {
      __fFileHandle = fopen(__sFileName, "rb");
    }
  }
  
  if (NULL == __fFileHandle)
    // File could not be opened, either for reading or writing
    ID3_THROW(ID3E_NoFile);
  else
  {
    fseek(__fFileHandle, 0, SEEK_END);
    __ulFileSize = ftell(__fFileHandle);
    fseek(__fFileHandle, 0, SEEK_SET);
  }
  
  return ;
}


luint ID3_Tag::Link(char *fileInfo)
{
  luint posn = 0;
  
  if (NULL == fileInfo)
    ID3_THROW(ID3E_NoData);

  strncpy(__sFileName, fileInfo, 256);
    
  // if we were attached to some other file then abort
  if (__fFileHandle != NULL)
    ID3_THROW(ID3E_TagAlreadyAttached);
      
  OpenLinkedFile();
  if (__bFileWritable)
    GenerateTempName();
  __ulOldTagSize = ParseFromHandle();
    
  if (__ulOldTagSize > 0)
    __ulOldTagSize += ID3_TAGHEADERSIZE;
      
  __ulFileSize -= __ulOldTagSize;
  posn = __ulOldTagSize;
    
  return posn;
}


void ID3_Tag::Update(luint ulTagFlag)
{
  if (!__bFileWritable)
    ID3_THROW_DESC(ID3E_NoFile, "ID3_Tag::Update: File not writable.");

  if ((ulTagFlag & V1_TAG) && (!__bHasV1Tag || HasChanged()))
    RenderV1ToHandle();
  if ((ulTagFlag & V2_TAG) && HasChanged())
    RenderV2ToHandle();
    
  return ;
}


void ID3_Tag::Strip(const luint ulTagFlag)
{
  FILE *tempOut;
  
  if (!__bFileWritable)
    ID3_THROW_DESC(ID3E_NoFile, "ID3_Tag::Strip: File not writable.");

  if (!(ulTagFlag & V1_TAG) && !(ulTagFlag & V2_TAG))
  {
    cerr << "--- No tag to strip. exiting." << endl;
    return;
  }

  if (strlen(__sTempName) == 0)
  {
    cerr << "--- No temp name.  Exiting." << endl;
  }
  else
  {
    tempOut = fopen(__sTempName, "wb");
    if (NULL == tempOut)
    {
      ID3_THROW_DESC(ID3E_NoData, "Ack! Couldn't open temp file.");
    }
    else
    {
      uchar * buffer2;
      luint ulOffset = (ulTagFlag & V2_TAG) ? __ulOldTagSize : 0;

      fseek(__fFileHandle, ulOffset, SEEK_SET);
      
      buffer2 = new uchar[BUFF_SIZE];
      if (NULL == buffer2)
        ID3_THROW(ID3E_NoMemory);

      bool done = false;
      luint bytesCopied = 0;
      luint bytesToCopy = __ulFileSize;
      size_t i;
        
      if (!(ulTagFlag & V2_TAG))
      {
        cerr << "+++ keeping old v2 tag" << endl;
        bytesToCopy += __ulOldTagSize;
      }
      if (__bHasV1Tag && (ulTagFlag & V1_TAG))
      {
        cerr << "+++ removing old v1 tag" << endl;
        bytesToCopy -= __ulExtraBytes;
      }
          
      while (! feof(__fFileHandle) && ! done)
      {
        luint size = BUFF_SIZE;
          
        if ((bytesToCopy - bytesCopied) < BUFF_SIZE)
        {
          size = bytesToCopy - bytesCopied;
          done = true;
        }
          
        i = fread(buffer2, 1, size, __fFileHandle);
        if (i > 0)
          fwrite(buffer2, 1, i, tempOut);
            
        bytesCopied += i;
      }
        
      delete[] buffer2;
      
      fclose(tempOut);
      fclose(__fFileHandle);
      remove(__sFileName);
      rename(__sTempName, __sFileName);
      OpenLinkedFile();
      
      __ulOldTagSize = 0;
      __ulExtraBytes = 0;
      
      __bHasV1Tag = __bHasV1Tag && !(ulTagFlag & V1_TAG);
        
      __bHasChanged = true;
    }
  }
  
  return ;
}



// $Log: tag_file.cpp,v $
// Revision 1.5  1999/11/19 19:09:12  scott
// (Update): Changed parameter to be a flag which indicates which type of
// tag to update, either V1_TAG, V2_TAG, or BOTH_TAGS.  Updated method to
// act appropriately based on the parameter passed in.
// (Strip): Changed parameter to be a flag which indicates which type of
// tag to update, either V1_TAG, V2_TAG, or BOTH_TAGS.  Updated method to
// act appropriately based on the parameter passed in.
//
// Revision 1.4  1999/11/15 20:20:37  scott
// Added include for config.h.  Minor code cleanup.  Removed
// assignments from if checks; first makes assignment, then checks
// for appropriate value.  Made private member variable names more
// descriptive.  OpenLinkedFile now checks to see if the file is
// writable or not, and still opens the file, even if it can't be
// written to.  This needs to be more robust (i.e., we shouldn't be
// creating temp files if the file isn't writable).  Made use of
// bFileWrtiable variable, which was introduced.
//
// Revision 1.3  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
