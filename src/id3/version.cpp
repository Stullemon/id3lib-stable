// $Id: version.cpp,v 1.1 1999/11/30 20:19:26 scott Exp $
// 
// The authors have released ID3Lib as Public Domain (PD) and claim no
// copyright, patent or other intellectual property protection in this work.
// This means that it may be modified, redistributed and used in commercial
// and non-commercial software and hardware without restrictions.  ID3Lib is
// distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
// express or implied.
// 
// The ID3Lib authors encourage improvements and optimisations to be sent to
// the ID3Lib coordinator, currently Scott Haug (sth2@cs.wustl.edu).  Approved
// submissions may be altered, and will be included and released under these
// terms.

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

const char *ID3_Name()
{
  return PACKAGE;
}

const char *ID3_Release()
{
  return VERSION;
}

const char *ID3_FullName()
{
  return ID3LIB_FULLNAME;
}

unsigned short ID3_Version()
{
  return ID3LIB_VERSION;
}

unsigned short ID3_Revision()
{
  return ID3LIB_REVISION;
}

unsigned short ID3_Patch()
{
  return ID3LIB_PATCH;
}

// $Log: version.cpp,v $
// Revision 1.1  1999/11/30 20:19:26  scott
// Added.  Implements version functions declared in version.h.
//
