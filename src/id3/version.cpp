// $Id: version.cpp,v 1.2 1999/12/17 16:13:04 scott Exp $
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
// Revision 1.2  1999/12/17 16:13:04  scott
// Updated opening comment block.
//
// Revision 1.1  1999/11/30 20:19:26  scott
// Added.  Implements version functions declared in version.h.
//
