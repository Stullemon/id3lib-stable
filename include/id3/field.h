// $Id: field.h,v 1.11 2000/04/09 22:34:57 eldamitri Exp $

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug

// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where to
// send such submissions.  See the AUTHORS file for a list of people who have
// contributed to id3lib.  See the ChangeLog file for a list of changes to
// id3lib.  These files are distributed with id3lib at
// http://download.sourceforge.net/id3lib/

#ifndef ID3LIB_FIELD_H
#define ID3LIB_FIELD_H

#include <stdlib.h>
#include "types.h"
#include "error.h"

// field flags
enum ID3_FieldFlags 
{
  ID3FF_NONE       =      0,
  ID3FF_NULL       = 1 << 0,
  ID3FF_NULLDIVIDE = 1 << 1,
  ID3FF_ADJUSTENC  = 1 << 2,
  ID3FF_ADJUSTEDBY = 1 << 3
};

// Enumeration of the types of field types
enum ID3_FieldType
{
  ID3FTY_INTEGER        = 0,
  ID3FTY_BITFIELD,
  ID3FTY_BINARY,
  ID3FTY_ASCIISTRING,
  ID3FTY_UNICODESTRING
};

// Used for version control
enum ID3_VerCtl
{
  ID3VC_HIGHER  = 0,
  ID3VC_LOWER
};

struct ID3_FieldDef
{
  ID3_FieldID   eID;
  ID3_FieldType eType;
  lsint         lFixedLength;
  uchar         ucVersion;
  uchar         ucRevision;
  ID3_VerCtl    eControl;
  luint         ulFlags;
  ID3_FieldID   eLinkedField;
  static const ID3_FieldDef* DEFAULT;
};

class ID3_Frame;
class ID3_Tag;

// Structure used for defining how frames are defined internally.
struct ID3_FrameDef
{
  ID3_FrameID   eID;
  char          sShortTextID[3 + 1];
  char          sLongTextID[4 + 1];
  lsint         lPriority;    // currently unused
  bool          bTagDiscard;
  bool          bFileDiscard;
  bool        (*parseHandler) (ID3_Frame *frame);
  ID3_FieldDef *aeFieldDefs;
};

/** The representative class of an id3v2 field.
    
    <p>

    As a general rule, you need never create an object of this type.  id3lib
    uses them internally as part of the id3_frame class.  You must know how to
    interact with these objects, though, and that's what this section is about.

    <p>

    The ID3_Field contains many overloaded methods to provide these facilities
    for four different data types: integers, ASCII strings, Unicode strings,
    and binary data.

    <p>

    An integer field supports the <a href="#Get">Get</a>, <a
    href="#Set">Set</a>, and <a href="#operator">operator</a>= methods.

    <p>

    Both types of strings support the <a
    href="#GetNumTextItems">GetNumTextItems</a> method

    <p>

    An ASCII string field supports the <a href="#Get">Get</a>, <a
    href="#Set">Set</a>, <a href="#Add">Add</a>, and <a
    href="#operator">operator</a>= methods.
    
    <p>

    A Unicode string field also supports the <a href="#Get">Get</a>, <a
    href="#Set">Set</a>, <a href="#Add">Add</a>, and <a
    href="#operator">operator</a>= methods.  Without elaborating, the Unicode
    methods behave exactly the same as their ASCII counterparts, taking
    unicode_t pointers in place of char pointers.
    
    <p>

    All strings in id3lib are handled internally as Unicode.  This means that
    when you set a field with an ASCII source type, it will be converted and
    stored internally as a Unicode string.  id3lib will handle all necessary
    conversions when parsing, rendering, and <a href="#Get">Get</a>ing.  If you
    set a field as an ASCII string, then try to read the string into a
    unicode_t buffer, id3lib will automatically convert the string into Unicode
    so this will function as expected.  The same holds true in reverse.  Of
    course, when converting from Unicode to ASCII, you will experience problems
    when the Unicode string contains characters that don't map to ISO-8859-1.

    <p>

    A binary field supports the <a href="#Get">Get</a>, <a href="#Set">Set</a>,
    <a href="#FromFile">FromFile</a>, and <a href="#ToFile">ToFile</a> methods.
    The binary field holds miscellaneous data that can't easily be described
    any other way, such as a JPEG image.
    
    <p>

    As a general implementation note, you should be prepared to support all
    fields in an id3lib frame, even if all fields in the id3lib version of the
    frame aren't present in the id3v2 version.  This si because of frames like
    the picture frame, which changed slightly from one version of the id3v2
    standard to the next (the IMAGEFORMAT format in 2.0 changed to a MIMETYPE
    in 3.0).  If you support all id3lib fields in a given frame, id3lib can
    generate the correct id3v2 frame for the id3v2 version you wish to support.
    Alternatively, just support the fields you know will be used in, say, 3.0
    if you only plan to generate 3.0 tags.

    @author Dirk Mahoney
    @version $Id: field.h,v 1.11 2000/04/09 22:34:57 eldamitri Exp $
    @see ID3_Tag
    @see ID3_Frame
    @see ID3_Err 
*/
class ID3_Field
{
  friend ID3_Frame;
  friend ID3_Tag;
public:
  ~ID3_Field(void);
  
  /** Clears any data and frees any memory associated with the field

      @see ID3_Tag::Clear
      @see ID3_Frame::Clear
  */
  void          Clear(void);
  /** Returns the size of a field.

      The value returned is dependent on the type of the field.  For ASCII
      strings, this returns the number of characters in the field, no including
      any NULL-terminator.  The same holds true for Unicode---it returns the
      number of characters in the field, not bytes, and this does not include
      the Unicode BOM, which isn't put in a Unicode string obtained by the <a
      href="#Get">Get</a> method anyway.  For binary and integer fields, this
      returns the number of bytes in the field.

      <pre>luint howBig = myFrame.Field(ID3FN_DATA).Size(); </pre>

      @return The size of the field, either in bytes (for binary or integer
              fields) or characters (for strings).
  */
  luint         Size(void);

  /** Returns the number of items in a text list.

      <pre>luint numItems = myFrame.Field(ID3FN_TEXT).GetNumItems();</pre>
      
      @return The number of items in a text list.
   */
  luint         GetNumTextItems(void);
  // integer field functions
  /** A shortcut for the Set method.

     <pre>myFrame.Field(ID3FN_PICTURETYPE) = 0x0B;</pre>

     @param newData The data to set this field to.
     @see Set
  */
  ID3_Field    &operator= (const luint newData);
  /** Sets the value of the field to the specified integer.

      @param newData The data to set this field to.
  */
  void          Set(const luint newData);
  /** Returns the value of the integer field.

     <pre>luint picType = myFrame.Field(ID3FN_PICTURETYPE).Get();</pre>
     
     @return The value of the integer field
   */
  luint         Get(void) const;
  // ASCII string field functions
  /** Shortcut for the Set operator.

      @param string The string to set this field to
      @see ID3_Field::Set(const luint)
  */
  ID3_Field    &operator= (const char *string);

  /** Copies the supplied string to the field.
      
      <p>
      
      You may dispose of the source string after a call to this method.
      
      <pre>myFrame.Field(ID3FN_TEXT).Set("ID3Lib is very cool!");</pre>
  */
  void          Set(const char *string);
  /** Copies the contents of the field into the supplied buffer, up to the
      number of characters specified; for fields with multiple entries, the
      optional third parameter indicates which of the fields to retrieve.

      <p>

      The third parameter is useful when using text lists (see ID3_Field::Add
      for more details).  The default value for this third parameter is 1,
      which returns the entire string if the field contains only one item.

      <p>
      
      It returns the number of characters (not bytes necessarily, and not
      including any NULL terminator) of the supplied buffer that are now used.
      
      <pre>
      char myBuffer[1024];
      luint charsUsed = myFrame.Field(ID3FN_TEXT).Get(buffer, 1024);</pre>
      
      It fills the buffer with as much data from the field as is present in the
      field, or as large as the buffer, whichever is smaller.
      
      <pre>
      char myBuffer[1024];
      luint charsUsed = myFrame.Field(ID3FN_TEXT).Get(buffer, 1024, 3);</pre>
      
      This fills the buffer with up to the first 1024 characters from the third
      element of the text list.
      
      @param buffer   Where the field's data is copied to
      @param maxChars The maximum number of characters to copy to the buffer.
      @param itemNum  For fields with multiple items (such as the involved
                      people frame, the item number to retrieve.
      @see ID3_Field::Add
  */
  luint       Get(char *buffer, const luint maxChars, const luint itemNum = 1);

  /** For fields which support this feature, adds a string to the list of
      strings currently in the field.
      
      <p>
      
      This is useful for using id3v2 frames such as the involved people list,
      composer, and part of set.  You can use the <a
      href="#GetNumItems">GetNumItems</a> method to find out how many such
      items are in a list.
     
      <pre>myFrame.Field(ID3FN_TEXT).Add("this is a test");</pre>
      
      @param string The string to set this field to.
  */
  void          Add(const char *string );

  // Unicode string field functions
  /** Shortcut for the Set operator.

      <p>
      
      Peforms similarly as the ASCII assignment operator, taking a unicode_t
      string as a parameter rather than an ascii string.

      @see #Add
   */
  ID3_Field    &operator= (const unicode_t *string);
  /** Copies the supplied unicode string to the field.
      
      <p>
      
      Peforms similarly as the ASCII <a href="#Set">Set</a> method, taking a
      unicode_t string as a parameter rather than an ascii string.
      
      @param string The unicode string to set this field to.
      @see #Add
  */
  void          Set(const unicode_t *string);

  /** Copies the contents of the field into the supplied buffer, up to the
      number of characters specified; for fields with multiple entries, the
      optional third parameter indicates which of the fields to retrieve.

      <p>

     Peforms similarly as the ASCII <a href="#Get">Get</a> method, taking a
     unicode_t string as a parameter rather than an ascii string.  The maxChars
     parameter still represents the maximum number of characters, not bytes.
     
     <pre>
     unicode_t myBuffer[1024];
     luint charsUsed = myFrame.Field(ID3FN_TEXT).Get(buffer, 1024);</pre> 
     
     @param buffer   Where the field's data is copied to
     @param maxChars The maximum number of characters to copy to the buffer.
     @param itemNum  For fields with multiple items (such as the involved
                     people frame, the item number to retrieve.
     @see #Get
  */
  luint         Get(unicode_t *buffer, const luint maxChars, const luint itemNum = 1);
  /** For fields which support this feature, adds a string to the list of
      strings currently in the field.
      
      <p>
      
     Peforms similarly as the ASCII <a href="#Add">Add</a> method, taking a
     unicode_t string as a parameter rather than an ascii string.
  */
  void          Add(const unicode_t *string);
  // binary field functions
  /** Copies the supplied unicode string to the field.
      
      <p>
      
      Again, like the string types, the binary <a href="#Set">Set</a> function
      copies the data so you may dispose of the source data after a call to
      this method.
      
      @param newData The data to assign to this field.
      @param newSize The number of bytes to be copied from the data array.
  */
  void          Set(const uchar *newData, const luint newSize);
  /** Copies the field's internal string to the buffer.
      
      <p>
      
      It copies the data in the field into the buffer, for as many bytes as the
      field contains, or the size of buffer, whichever is smaller.
      
      <pre>
      uchar buffer[1024];
      myFrame.Field(ID3FN_DATA).Get(buffer, sizeof(buffer));</pre>
      
      @param buffer Where to copy the contents of the field.
      @param length The number of bytes in the buffer
  */
  void          Get(uchar *buffer, const luint length);
  /** Copies binary data from the file specified to the field.
      
      <pre>
      myFrame.Field(ID3FN_DATA).FromFile("mypic.jpg");</pre>
      
      @param info The name of the file to read the data from.
  */
  void          FromFile(const char *info);
  /** Copies binary data from the field to the specified file.
      
      <pre>
      myFrame.Field(ID3FN_DATA).ToFile("output.bin");</pre>

      @param info The name of the file to write the data to.
  */
  void          ToFile(const char *sInfo);
  
  ID3_Field    &operator=( const ID3_Field & );

private:
  luint         BinSize(const bool withExtras = true);
  bool          HasChanged(void);
  void          SetVersion(const uchar ver, const uchar rev);
  luint         Render(uchar *buffer);
  luint         Parse(const uchar *buffer, const luint posn, const luint buffSize);

private:
  // To prevent public instantiation, the constructor is made private
  ID3_Field(void);

  ID3_FieldID   __eName;           // the ID of this field
  ID3_FieldType __eType;           // what type is this field or should be
  lsint         __lFixedLength;    // length of field (fixed if positive)
  uchar         __ucIOVersion;     // specific version
  uchar         __ucIORevision;    // specific revision
  ID3_VerCtl    __eControl;        // render if ver/rev is higher, or lower than frame::version, frame::revision?
  luint         __ulFlags;         // special field flags
  uchar         __ucVersion;       // the version being rendered/parsed
  uchar         __ucRevision;      // the revision being rendered/parsed
  bool          __bHasChanged;     // field changed since last parse/render?
  uchar        *__sData;
  luint         __ulSize;
protected:
  luint RenderInteger(uchar *buffer);
  luint RenderASCIIString(uchar *buffer);
  luint RenderUnicodeString(uchar *buffer);
  luint RenderBinary(uchar *buffer);
  
  size_t ParseInteger(const uchar *buffer, luint posn, size_t nSize);
  size_t ParseASCIIString(const uchar *buffer, luint posn, size_t nSize);
  size_t ParseUnicodeString(const uchar *buffer, luint posn, size_t nSize);
  size_t ParseBinary(const uchar *buffer, luint posn, size_t nSize);
  
};

ID3_FrameDef *ID3_FindFrameDef(const ID3_FrameID id);
ID3_FrameID   ID3_FindFrameID(const char *id);

#endif

// $Log: field.h,v $
// Revision 1.11  2000/04/09 22:34:57  eldamitri
// (struct ID3_FieldDef): Added DEFAULT.
// (struct ID3_FrameDef): Made text id's fixed length char arrays rather
// than char ptrs.
//
// Revision 1.10  2000/04/08 04:32:36  eldamitri
// Changed new ANSI-standard C++ include headers to old-style headers.
//
// Revision 1.9  2000/04/07 04:45:31  eldamitri
// Minor cleanup for log comments.
//
// Revision 1.8  2000/04/05 05:20:52  eldamitri
// Updated initial comment information to reflect license, copyright
// change.
//
// Revision 1.7  2000/01/04 15:42:22  eldamitri
// For compilation with gcc 2.95.2 and better compatibility with ANSI/ISO
// standard C++, updated, rearranged, and removed (where necessary)
// #include directives.
//
// Revision 1.6  1999/12/26 16:40:04  scott
// (class ID3_Field): Cleaned up interface to Parse* methods.
//
// Revision 1.5  1999/12/17 16:05:02  scott
// Updated opening comment block.
//
// Revision 1.4  1999/12/15 06:37:39  scott
// Reformatted some comments.
//
// Revision 1.3  1999/12/09 02:45:41  scott
// (class ID3_Field): Added const to Get method.  Added operator= method
// declaration.
//
// Revision 1.2  1999/12/02 22:45:28  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places.
//
// Revision 1.1  1999/12/01 17:16:01  scott
// moved from src/id3 to include/id3
//
// Revision 1.10  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.9  1999/11/29 19:05:41  scott
// Updated documentation to work better with doc++ (probably doesn't work
// well with kdoc anymore).
//
// Revision 1.8  1999/11/25 19:11:40  scott
// Added doc++/kdoc/javadoc-like documentation, mostly transcribed
// from Dirk's ID3Lib Documentation MSWord doc.
// (ID3_FieldFlags ): Created enumeration to replace generic constants.
// (ID3_FieldID): Removed.  Replaced in globals.h.
// (ID3_FrameID): Removed.  Replaced in globals.h.
// (class ID3_Field): Made the constructor private.
//
// Revision 1.7  1999/11/19 17:19:58  scott
// Updated method interfaces to make appropriate parameters const.
//
// Revision 1.6  1999/11/15 20:15:36  scott
// Made ID3_Frame and ID3_Tag friends of the ID3_Field class to allow
// access to the private members in ID3_Field.  Made ID3FF field
// flags const luints, rather than #define's.  Made member variable
// names of classes and structs more descriptive.
//
// Revision 1.5  1999/11/04 04:32:11  scott
// Initial revision
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
