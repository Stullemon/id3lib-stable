// $Id: error.h,v 1.1 1999/12/01 17:15:59 scott Exp $
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


#ifndef ID3LIB_ERROR_H
#define ID3LIB_ERROR_H

#include <id3/types.h>

/**
   Predefined id3lib error types.
 */
enum ID3_Err
{
  /// No available memory
  ID3E_NoMemory = 0,
  /// No data to parse
  ID3E_NoData,
  /// Improperly formatted data
  ID3E_BadData,
  /// No buffer to write to
  ID3E_NoBuffer,
  /// Buffer is too small
  ID3E_SmallBuffer,
  /// Invalid frame id
  ID3E_InvalidFrameID,
  /// Requested field not found
  ID3E_FieldNotFound,
  /// Unknown field type
  ID3E_UnknownFieldType,
  /// Tag is already attached to a file
  ID3E_TagAlreadyAttached,
  /// Invalid tag version
  ID3E_InvalidTagVersion,
  /// No file to parse
  ID3E_NoFile,
  /// Attempting to write to a read-only file
  ID3E_ReadOnly,
  /// Error in compression/uncompression
  ID3E_zlibError
};

/** When id3lib encounters a nasty error, it thros an exception of type
    ID3_Error.  A function that calls an id3lib routine can place the call in a
    try block and provide an appropriate catch block.
    
    <pre>
    try
    {
      // call some id3lib routine
      ID3_Tag myTag("mySong.mp3");
      ID3_Frame *myFrame = NULL;
      
      // this will generate an exception
      myTag << myFrame;
    }
    catch (ID3_Error err)
    {
      // handle the error
      ...
    }</pre>
*/
class ID3_Error
{
public:
  /** Returns the ID3_Err value, which represents the ID of the error
      that caused the exception.
   */
  ID3_Err GetErrorID(void) const;
  /** Returns the English string that defines the error type.
      
      <p>
      
      Each error ID has a set string error type.
   */
  char   *GetErrorType(void) const;
  /** Returns a string that gives more explanation as to what caused the
      exception, if enabled by the code that caused the exception.
   */
  char   *GetErrorDesc(void) const;
  /** Returns a pointer to a string of characters that is the name
      of the id3lib source file that generated the exception.
      
      <p>
      
      When submitting bug reports, it is useful to include the following.
      
      <pre>
      cout << "Exception in file '" << err.GetErrorFile() << "'" << endl;</pre>
  */
  char   *GetErrorFile(void) const;
  /** Returns the line number in the id3lib source file that threw the
      exception.
      
      <pre>cout << "Line #" << err.GetErrorLine() << endl;</pre>
  */
  luint   GetErrorLine(void) const;
  
  /** Constructor
      
      @param eID          Erroy id
      @param sFileName    Filename where error occurred
      @param nLineNum     Linenumber where error occurred
      @param sDescription Description of error
  */
  ID3_Error(const ID3_Err eID, const char *sFileName, const luint nLineNum, 
            const char *sDescription);
private:
  ID3_Err __eError;
  luint   __nErrLine;
  char   *__sErrFileName;
  char   *__sErrDesc;
};

/** Shortcut macro for throwing an error without a description
    
    @param x The error id
*/
#define ID3_THROW(x) throw ID3_Error(x, __FILE__, __LINE__, "")
/** Shortcut macro for throwing an error with a description
    
    @param x The error id
    @param y The error description
*/
#define ID3_THROW_DESC(x, y) throw ID3_Error(x, __FILE__, __LINE__, y)

#endif

// $Log: error.h,v $
// Revision 1.1  1999/12/01 17:15:59  scott
// moved from src/id3 to include/id3
//
// Revision 1.9  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.8  1999/11/29 17:00:53  scott
// Updated documentation to work better with doc++ (probably doesn't work
// well with kdoc anymore).
//
// Revision 1.7  1999/11/25 19:05:06  scott
// Added doc++/kdoc/javadoc-like documentation.
// (ID3_Err): Added error types SmallBuffer and ReadOnly.
//
// Revision 1.6  1999/11/16 22:50:15  scott
// * error.h (ID3_Err): Added new ID3_Err: ID3E_BadData.
//
// Revision 1.5  1999/11/15 20:15:13  scott
// Made constructor public.  Added new interface to error reporting
// to allow for more descriptive error messages (this should still be
// worked on).  Made private member variable names more descriptive.
// Added const qualifier to appropriate methods.
//
// Revision 1.4  1999/11/04 04:32:11  scott
// Initial revision
//
// Revision 1.3  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
