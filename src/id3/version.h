// $Id: version.h,v 1.5 1999/11/25 19:25:46 scott Exp $

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

#ifndef ID3LIB_VERSION_H
#define ID3LIB_VERSION_H
/**
 * id3lib defines some constant symbols that applications are free to use.
 * They are used to identify the version and date of the id3lib that is
 * currently being used.
 *
 * For example...
 *
 * <pre>cout << "Using " << ID3LIB_VERSTRING << endl;</pre>
 */
const char * const ID3LIB_NAME      = "ID3Lib";
const char * const ID3LIB_VERSION   = "v3.6.0";
const luint        ID3LIB_VER       = 3;
const luint        ID3LIB_REV       = 6;
const char * const ID3LIB_DATE      = "Sat Nov 20 11:27:25 CST 1999";
#define     ID3LIB_VERSTRING   ID3LIB_NAME " " ID3LIB_VERSION " " ID3LIB_DATE

#endif

// $Log: version.h,v $
// Revision 1.5  1999/11/25 19:25:46  scott
// Added doc++/kdoc/javadoc-like documentation.  Replaced #define's with
// const's.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
