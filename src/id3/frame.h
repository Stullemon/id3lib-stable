// $Id: frame.h,v 1.7 1999/11/25 19:15:15 scott Exp $

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

#include <id3/types.h>
#include <id3/field.h>
#include <id3/header_frame.h>

class ID3_Tag;

/**
 * id3lib defines frames in a funny way.  Using some nice c++ conventions,
 * ID3_Frame class objects appear to be quite polymorphic; that is, they can
 * take on many forms.  The same ID3_Frame class provides the facilities for
 * the implementation of a complex APIC frame and for a simple text frame.
 * Here's how to use an ID3_Frame object...
 *
 * @short The representative class of an id3v2 frame.
 * @author Dirk Mahoney
 * @version $Id: frame.h,v 1.7 1999/11/25 19:15:15 scott Exp $
 * @see ID3_Tag
 * @see ID3_Field
 * @see ID3_Err
 */
class ID3_Frame
{
  friend ID3_Tag;
public:
  /** 
   * The optional parameter to the ID3_Frame constructor is of the type
   * @ref ID3_FrameID, which will internally set the frame's structure.  See
   * @ref #SetID() for more details.
   *
   * @param id The type of frame to create
   * @see ID3_FrameID
   */
  ID3_Frame(ID3_FrameID id = ID3FID_NOFRAME);

  /**
   */
  ~ID3_Frame(void);
  
  /** 
   * Like its @ref ID3_Tag counterpart, @ref #Clear takes no parameters and
   * returns nothing.  It just clears the frame of all data and resets the
   * frame such that it can take on the form of any id3v2 frame that id3lib
   * supports.
   *
   * @see ID3_Tag#Clear
   */
  void        Clear(void);

  /**
   * This method sets up the internal structure of an ID3_Frame object so
   * that it represents any id3v2 frame that id3lib suppors.  Given an
   * @ref ID3_FrameID (a list of which is found in <id3/field.h>), @ref #SetID
   * will structure the object according to the frame you wish to implement.
   *
   * Either using this call or via the constructor, this must be the first
   * command performed on an ID3_Frame object.  
   *
   * <pre>
   * myFrame.SetID(ID3FID_TITLE);
   * </pre>
   *
   * @param id The type of frame this frame should be set to
   */
  void        SetID(ID3_FrameID id);

  /** 
   * Useful in conjunction with the @ref ID3_Tag#GetFrame method, this
   * method returns the type of frame that the object represents, just so you
   * know what to expect.
   *
   * @returns The type, or id, of the frame
   */
  ID3_FrameID GetID(void) const;

  /**
   * This is a go-between method that returns a pointer to one of the frame's
   * internal fields.  A list of fields that are in given frames appears in
   * <id3/field.cpp>.  This method returns a reference to the field in
   * question, so that the result can be used as though it were a field
   * itself...
   *
   * <pre>
   * ID3_TextEnc enc;
   * enc = (ID3_TextEnc) myFrame.Field(ID3FN_TEXTENC).Get();
   * </pre>
   *
   * @param name The name of the field to be retrieved
   * @returns A reference to the desired field
   */
  ID3_Field  &Field(ID3_FieldID name) const;
  
private:
  char        __sEncryptionID[256]; // encryption method used with this frame
  char        __sGroupingID[256];   // the group to which this frame belongs
  bool        __bCompression;       // should we try to compress?
  bool        __bHasChanged;        // frame changed since last parse/render?
  bitset      __auiFieldBits;       // which fields are present?
  ID3_FrameID __FrameID;            // what frame are we?
  uchar       __ucVersion;          // what version tag?
  uchar       __ucRevision;         // what revision tag?
  luint       __ulNumFields;        // how many fields are in this frame?
  ID3_Field **__apFields;           // an array of field object pointers
protected:
  bool        HasChanged(void ) const;
  void        SetVersion(uchar ver, uchar rev);
  void        Parse(uchar *buffer, luint size);
  void        UpdateStringTypes(void);
  void        UpdateFieldDeps(void);
  luint       Size(void);
  luint       Render(uchar *buffer);
  lsint       FindField(ID3_FieldID name) const;
}
;

// $Log: frame.h,v $
// Revision 1.7  1999/11/25 19:15:15  scott
// Added doc++/kdoc/javadoc-like documentation, mostly transcribed
// from Dirk's ID3Lib Documentation MSWord doc.
//
// Revision 1.6  1999/11/15 20:18:11  scott
// Made ID3_Tag a friend of ID3_Frame to allow access to the private
// members in ID3_Frame.  Made variable names more descriptive.
// Added const qualifier to appropriate methods.
//
// Revision 1.5  1999/11/04 04:32:11  scott
// Initial revision
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
