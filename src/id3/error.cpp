// $Id: error.cpp,v 1.6 1999/11/16 22:50:10 scott Exp $

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
#include <id3/error.h>

static char *ID3_ErrorDescs[] =
{
  "out of memory",
  "no source/dest data specified",
  "no buffer specified",
  "invalid frame id",
  "field not found",
  "unknown field type",
  "tag is already attached to a file",
  "invalid tag version",
  "file not found",
  "error in zlib compression library",
  "improperly formatted id3 tag"
};
  
  
ID3_Error::ID3_Error(const ID3_Err eCode, const char *sFileName, 
                     const luint nLine, const char *sDesc)
{
  __eError = eCode;
  __nErrLine = nLine;
  luint nFileNameLen = strlen(sFileName);
  luint nDescLen = strlen(sDesc);
  
  __sErrFileName = new char[nFileNameLen+1];
  __sErrDesc     = new char[nDescLen+1];
  strncpy(__sErrFileName, sFileName, nFileNameLen);
  strncpy(__sErrDesc, sDesc, nDescLen);
}


ID3_Err ID3_Error::GetErrorID(void) const
{
  return __eError;
}


char *ID3_Error::GetErrorType(void) const
{
  return ID3_ErrorDescs[__eError];
}

char *ID3_Error::GetErrorDesc(void) const
{
  return __sErrDesc;
}

char *ID3_Error::GetErrorFile(void) const
{
  return __sErrFileName;
}


luint ID3_Error::GetErrorLine(void) const
{
  return __nErrLine;
}



// $Log: error.cpp,v $
// Revision 1.6  1999/11/16 22:50:10  scott
// * error.cpp (ID3_ErrorDescs): Added text entry for ID3E_BadData.
//
// Revision 1.5  1999/11/15 20:15:03  scott
// Added include for config.h.  Added new interface to error
// reporting to allow for more descriptive error messages (this
// should still be worked on).  Made private member variable names
// more descriptive.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
