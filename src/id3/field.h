// $Id: field.h,v 1.8 1999/11/25 19:11:40 scott Exp $

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


#ifndef ID3LIB_FIELD_H
#define ID3LIB_FIELD_H

#include <wchar.h>
#include <id3/types.h>
#include <id3/error.h>

// field flags
enum ID3_FieldFlags 
{
  ID3FF_NONE       =      0,
  ID3FF_NULL       = 1 << 0,
  ID3FF_NULLDIVIDE = 1 << 1,
  ID3FF_ADJUSTENC  = 1 << 2,
  ID3FF_ADJUSTEDBY = 1 << 3
};

/**
 * Enumeration of the types of field types: integer, bitfield, binary, ascii
 * string, or uncode string
 */
enum ID3_FieldType
{
  ID3FTY_INTEGER        = 0,
  ID3FTY_BITFIELD,
  ID3FTY_BINARY,
  ID3FTY_ASCIISTRING,
  ID3FTY_UNICODESTRING
};

/**
 * Used for version control
 */
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
};

class ID3_Frame;
class ID3_Tag;

/**
 * Structure used for defining how frames are defined internally.
 */
struct ID3_FrameDef
{
  ID3_FrameID   eID;
  char         *sShortTextID;
  char         *sLongTextID;
  lsint         lPriority;    // currently unused
  bool          bTagDiscard;
  bool          bFileDiscard;
  bool        (*parseHandler) (ID3_Frame *frame);
  ID3_FieldDef *aeFieldDefs;
};

/** 
 * As a general rule, you need never create an object of this type.  id3lib
 * uses them internally as part of the id3_fame class.  You must know how to
 * interact with these objects, though, and that's what this section is about.
 *
 * The ID3_Field contains many overloaded methods to provide these facilities
 * for four different data types: integers, ASCII strings, Unicode strings, and
 * binary data.
 *
 * An integer field supports the @ref #Get, @ref #Set(), and @ref #operator=
 * methods.
 *
 * Both types of strings support the @ref #GetNumTextItems method
 *
 * An ASCII string field supports the @ref #Get, @ref #Set, @ref #Add, and 
 * @ref #operator= methods.
 *
 * A Unicode string field also supports the @ref #Get, @ref #Set, @ref #Add,
 * and @ref #operator= methods.  Without elaborating, the Unicode methods
 * behave exactly the same as their ASCII counterparts, taking wchar_t pointers
 * in place of char pointers.  
 *
 * All strings in id3lib are handled internally as Unicode.  This means that
 * when you set a field with an ASCII source type, it will be converted and
 * stored internally as a Unicode string.  id3lib will handle all necessary
 * conversions when parsing, rendering, and #@ref Get ing.  If you set a field
 * as an ASCII string, then try to read the string into a wchar_t buffer,
 * id3lib will automatically convert the string into Unicode so this will
 * function as expected.  The same holds true in reverse.  Of course, when
 * converting from Unicode to ASCII, you will experience problems when the
 * Unicode string contains characters that don't map to ISO-8859-1.
 *
 * A binary field supports the @ref #Get, @ref #Set, @ref #FromFile, and @ref
 * #ToFile methods.  The binary field holds miscellaneous data that can't
 * easily be described any other way, such as a JPEG image.
 * 
 * As a general implementation note, you should be prepared to support all
 * fields in an id3lib frame, even if all fields in the id3lib version of the
 * frame aren't present in the id3v2 version.  This si because of frames like
 * the picture frame, which changed slightly from one version of the id3v2
 * standard to the next (the IMAGEFORMAT format in 2.0 changed to a MIMETYPE in
 * 3.0).  If you support all id3lib fields in a given frame, id3lib can
 * generate the correct id3v2 frame for the id3v2 version you wish to support.
 * Alternatively, just support the fields you know will be used in, say, 3.0 if
 * you only plan to generate 3.0 tags.  
 *
 * @short The representative class of an id3v2 field.
 * @author Dirk Mahoney
 * @version #Id#
 * @see ID3_Tag
 * @see ID3_Frame
 * @see ID3_Err
 */
class ID3_Field
{
  friend ID3_Frame;
  friend ID3_Tag;
public:
  ~ID3_Field(void);
  
  /** 
   * Regardless of the type of field, this method will clear any data and free
   * any memory associated with the field.  This method takes no parameters and
   * returns no results.
   *
   * @see ID3_Tag@Clear
   * @see ID3_Frame@Clear
   */
  void          Clear(void);
  /**
   * This method returns the size of a field.
   *
   * The value returned is dependent on the type of the field.  For ASCII
   * strings, this returns the number of characters in the field, no including
   * any NULL-terminator.  The same holds true for Unicode---it returns the
   * number of characters in the field, not bytes, and this does not include
   * the Unicode BOM, which isn't put in a Unicode string obtained by the
   * @ref #Get method anyway.  For binary and integer fields, this returns the
   * number of bytes in the field.
   *
   * <pre>
   * luint howBig = myFrame.Field(ID3FN_DATA).Size();
   * </pre>
   *
   * @returns The size of the field, either in bytes (for binary or integer
   *          fields) or characters (for strings).
   */
  luint         Size(void);

  /**
   * This method returns the number of items in a text list.
   *
   * <pre>
   * luint numItems = myFrame.Field(ID3FN_TEXT).GetNumItems();
   * </pre>
   *
   * @returns The number of items in a text list.
   */
  luint         GetNumTextItems(void);
  // integer field functions
  /**
   * The assignment operator is a shortcut for the Set method.
   *
   * <pre>
   * myFrame.Field(ID3FN_PICTURETYPE) = 0x0B;
   * </pre>
   *
   * @param newData The data to set this field to.
   * @see ID3_Field#Set
   */
  ID3_Field    &operator= (const luint newData);
  /**
   * This method takes a single integer parameter.  It sets the value of this 
   * field to the specified integer.
   *
   * @param newData The data to set this field to.
   */
  void          Set(const luint newData);
  /**
   * Taking no parameters, this method returns the value of the integer field.
   *
   * <pre>
   * luint picType = myFrame.Field(ID3FN_PICTURETYPE).Get();
   * </pre>
   *
   * @returns The value of the integer field
   */
  luint         Get(void);
  // ASCII string field functions
  /**
   * @param string The string to set this field to
   * @see #Set
   */
  ID3_Field    &operator= (const char *string);
  /**
   * This method copies the supplied string to the field, so you may dispose
   * of the source string after a call tho this method.
   *
   * <pre>
   * myFrame.Field(ID3FN_TEXT).Set("ID3Lib is very cool!");
   * </pre>
   */
  void          Set(const char *string);
  /**
   * This takes a pointer to a char buffer and an integer that says how big (in
   * characters) the supplied buffer is.  Optionally as the third parameter is
   * the item number you wish to retrieve.  This is useful when using text
   * lists (see @ref ID3_Field#Add for more details).  The default value for
   * this third parameter is 1, which returns the entire string if the field
   * contains only one item.
   *
   * It returns the number of characters (not bytes necessarily, and not
   * including any NULL terminator) of the supplied buffer that are now used.
   *
   * <pre>
   * char myBuffer[1024];
   * luint charsUsed = myFrame.Field(ID3FN_TEXT).Get(buffer, 1024);
   * </pre>
   *
   * It fills the buffer with as much data from the field as is present in the
   * field, or as large as the buffer, whichever is smaller.
   *
   * <pre>
   * char myBuffer[1024];
   * luint charsUsed = myFrame.Field(ID3FN_TEXT).Get(buffer, 1024, 3);
   * </pre>
   *
   * This fills the buffer with up to the first 1024 characters from the third
   * element of the text list.
   *
   * @param buffer   Where the field's data is copied to
   * @param maxChars The maximum number of characters to copy to the buffer.
   * @param itemNum  For fields with multiple items (such as the involved
                     people frame, the item number to retrieve.
   */
  luint       Get(char *buffer, const luint maxChars, const luint itemNum = 1);

  /**
   * This adds a string to the list of strings currently in the field.  This is
   * useful for using id3v2 frames such as the involved people list, composer,
   * and part of set.  You can use the @ref #GetNumItems method to find
   * out how many such items are in a list.
   *
   * <pre>
   * myFrame.Field(ID3FN_TEXT).Add("this is a test");
   * </pre>
   *
   * @param string The string to set this field to.
   */
  void          Add(const char *string );
  // Unicode string field functions
  /**
   * Peforms similarly as the ASCII assignment operator, taking a wchar_t
   * string as a parameter rather than an ascii string.
   *
   * @see #Add
   */
  ID3_Field    &operator= (const wchar_t *string);
  /** 
   * Peforms similarly as the ASCII @ref #Set method, taking a wchar_t string
   * as a parameter rather than an ascii string.
   *
   * @param string The unicode string to set this field to.
   * @see #Add
   */
  void          Set(const wchar_t *string);
  /** 
   * Peforms similarly as the ASCII @ref #Get method, taking a wchar_t string
   * as a parameter rather than an ascii string.  The maxChars parameter still
   * represents the maximum number of characters, not bytes.
   *
   * <pre>
   * wchar_t myBuffer[1024];
   * luint charsUsed = myFrame.Field(ID3FN_TEXT).Get(buffer, 1024);
   * </pre> 
   *
   * @param buffer   Where the field's data is copied to
   * @param maxChars The maximum number of characters to copy to the buffer.
   * @param itemNum  For fields with multiple items (such as the involved
                     people frame, the item number to retrieve.
   * @see #Get
   */
  luint         Get(wchar_t *buffer, const luint maxChars, const luint itemNum = 1);
  /** 
   * Peforms similarly as the ASCII @ref #Add method, taking a wchar_t string
   * as a parameter rather than an ascii string.
   */
  void          Add(const wchar_t *string);
  // binary field functions
  /**
   * This method takes a pointer to an unsigned char buffer and an integer that
   * specifies how big the data is in bytes.  Again, like the string types, the
   * binary @ref #Set function copies the data so you may dispose of the source
   * data after a call to this method.
   *
   * @param newData The data to assign to this field.
   * @param newSize The number of bytes in the data array.
   */
  void          Set(const uchar *newData, const luint newSize);
  /**
   * This takes the same parameters as the @ref #Set method.  It copies the
   * data in the field into the buffer, for as many bytes as the field
   * contains, or the size of buffer, whichever is smaller.
   *
   * <pre>
   * uchar buffer[1024];
   * myFrame.Field(ID3FN_DATA).Get(buffer, sizeof(buffer));
   * </pre>
   *
   * @param buffer Where to copy the contents of the field.
   * @param length The number of bytes in the buffer
   */
  void          Get(uchar *buffer, const luint length);
  /**
   * Taking a single char pointer as a parameter, this method takes the data
   * in the specified file and stores it in the field.  The file may be deleted
   * after a call to this method as the source data is copied into the field.
   *
   * <pre>
   * myFrame.Field(ID3FN_DATA).FromFile("mypic.jpg");
   * </pre>
   *
   * @param info The name of the file to read the data from.
   */
  void          FromFile(const char *info);
  /**
   * The exact reverse of @ref #FromFile, this method copies the data in the
   * field to the specified file.
   *
   * <pre>
   * myFrame.Field(ID3FN_DATA).ToFile("output.bin");
   * </pre>
   *
   * @param info The name of the file to write the data to.
   */
  void          ToFile(const char *sInfo);
  
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
  
  luint ParseInteger(const uchar *buffer, const luint posn, const luint buffSize);
  luint ParseASCIIString(const uchar *buffer, const luint posn, const luint buffSize);
  luint ParseUnicodeString(const uchar *buffer, const luint posn, const luint buffSize);
  luint ParseBinary(const uchar *buffer, const luint posn, const luint buffSize);
  
};

ID3_FrameDef *ID3_FindFrameDef(const ID3_FrameID id);
ID3_FrameID   ID3_FindFrameID(const char *id);

#endif

// $Log: field.h,v $
// Revision 1.8  1999/11/25 19:11:40  scott
// Added doc++/kdoc/javadoc-like documentation, mostly transcribed
// from Dirk's ID3Lib Documentation MSWord doc.
// (ID3_FieldFlags ): Created enumeration to replace generic constants.
// (ID3_FieldID): Removed.  Replaced in globals.h.
// (ID3_FrameID): Removed.  Replaced in globals.h.
// (class ID3_Field): Made the constructor private.
//
// Revision 1.7  1999/11/19 17:19:58  scott
// * field.h (class ID3_Field): Updated method interfaces to make
// appropriate parameters const.
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
