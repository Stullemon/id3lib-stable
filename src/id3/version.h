// $Id: version.h,v 1.7 1999/11/29 19:26:18 scott Exp $
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

#ifndef ID3LIB_VERSION_H
#define ID3LIB_VERSION_H
/** id3lib defines some constant symbols that applications are free to use.
    
    <p>
    
    They are used to identify the version and date of the id3lib that is
    currently being used.
   
    <p>
    
    For example...
    
    <pre>cout << "Using " << ID3LIB_VERSTRING << endl;</pre>

    @name version_constants
*/
/// The name of the library
const char  ID3LIB_NAME[]      = "ID3Lib";
/// The full library version
const char  ID3LIB_VERSION[]   = "v3.6";
/// The library version
const luint ID3LIB_VER         = 3;
/// The library revision
const luint ID3LIB_REV         = 6;
/// The library date
const char  ID3LIB_DATE[]      = "$Date: 1999/11/29 19:26:18 $";

#endif

// $Log: version.h,v $
// Revision 1.7  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.6  1999/11/29 19:13:51  scott
// Updated documentation to work better with doc++ (probably doesn't work
// well with kdoc anymore).  Updated the constants.
//
// Revision 1.5  1999/11/25 19:25:46  scott
// Added doc++/kdoc/javadoc-like documentation.  Replaced #define's with
// const's.
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
