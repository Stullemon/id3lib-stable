// $Id: version.h,v 1.2 1999/12/17 16:05:02 scott Exp $
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

#ifndef ID3LIB_VERSION_H
#define ID3LIB_VERSION_H

/** id3lib defines some functions for making known the libraries version info.
    
    <p>
    
    They are used to identify the version and date of the id3lib that is
    currently being used.
*/
/// Returns the name of the library (i.e., "id3lib")
const char *ID3_Name(void);
/// Returns the library release (i.e., "3.6.0")
const char *ID3_Release(void);
/// Returns the full name of the library (i.e., "id3lib-3.6.0")
const char *ID3_FullName(void);
/// Returns the library version (i.e., 3)
unsigned short ID3_Version(void);
/// Returns the library revision (i.e., 6)
unsigned short ID3_Revision(void);
/// Returns the library patch (i.e., 0)
unsigned short ID3_Patch(void);

#endif

// $Log: version.h,v $
// Revision 1.2  1999/12/17 16:05:02  scott
// Updated opening comment block.
//
// Revision 1.1  1999/12/01 17:16:12  scott
// moved from src/id3 to include/id3
//
// Revision 1.8  1999/11/30 20:16:35  scott
// Changed from preprocessor macros to functions.  Now changes to top
// level configure.in with regards to version info will be reflected in
// these functions without having to change this file.
//
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
