// $Id: frame.h,v 1.3 1999/12/09 02:45:46 scott Exp $
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

#include "types.h"
#include "field.h"
#include "header_frame.h"

class ID3_Tag;

/** The representative class of an id3v2 frame.
    
    <p>
    
    id3lib defines frames in a funny way.  Using some nice c++ conventions,
    ID3_Frame class objects appear to be quite polymorphic; that is, they can
    take on many forms.  The same ID3_Frame class provides the facilities for
    the implementation of a complex APIC frame and for a simple text frame.
    
    @author Dirk Mahoney
    @version $Id: frame.h,v 1.3 1999/12/09 02:45:46 scott Exp $
    @see ID3_Tag
    @see ID3_Field
    @see ID3_Err
*/
class ID3_Frame
{
  friend ID3_Tag;
public:
  /** Default constructor; accepts as a default parameter the type of frame
      to create.
      
      <p>
      
      The parameter which will internally set the frame's structure.  See
      <a href="#SetID">SetID</a> for more details.
     
     @param id The type of frame to create
     @see ID3_FrameID
     @see SetID
  */
  ID3_Frame(ID3_FrameID id = ID3FID_NOFRAME);

  /// Destructor.
  ~ID3_Frame(void);
  
  /** Clears the frame of alld ata and resets the frame such that it can take
      on the form of any id3v2 frame that id3lib supports.
     
      @see ID3_Tag::Clear
  */
  void        Clear(void);

  /** Establishes the internal structure of an ID3_Frame object so
      that it represents the id3v2 frame indicated by the parameter
      
      <p>
      
      Given an ID3_FrameID (a list of which is found in &lt;id3/field.h&gt;),
      <a href="#SetID">SetID</a> will structure the object according to the
      frame you wish to implement.
      
      <p>
      
      Either using this call or via the constructor, this must be the first
      command performed on an ID3_Frame object.  
      
      <pre>myFrame.SetID(ID3FID_TITLE);</pre>
      
      @param id The type of frame this frame should be set to
      @see ID3_FrameID
  */
  void        SetID(ID3_FrameID id);

  /** Returns the type of frame that the object represents.
      
      <p>
      
      Useful in conjunction with ID3_Tag's Find method
      
      @returns The type, or id, of the frame
      @see ID3_Tag::Find
  */
  ID3_FrameID GetID(void) const;
  
  /** Returns a reference to the frame's internal field indicated by the 
      parameter.
      
      <p>

      A list of fields that are in given frames appears in
      &lt;id3/field.cpp&gt;.  This method returns a reference to the field in
      question so that the result can be used as though it were a field
      itself.
     
     <pre>
     ID3_TextEnc enc;
     enc = (ID3_TextEnc) myFrame.Field(ID3FN_TEXTENC).Get(); </pre>
     
     @param name The name of the field to be retrieved
     @returns A reference to the desired field
  */
  ID3_Field  &Field(ID3_FieldID name) const;
  
  ID3_Frame  &operator=( const ID3_Frame &rFrame );

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
// Revision 1.3  1999/12/09 02:45:46  scott
// (class ID3_Frame): Added operator= method declaration.
//
// Revision 1.2  1999/12/02 22:45:28  scott
// Changed all of the #include <id3/*> to #include "*" to help ensure that
// the sources are searched for in the right places.
//
// Revision 1.1  1999/12/01 17:16:02  scott
// moved from src/id3 to include/id3
//
// Revision 1.9  1999/11/29 19:26:18  scott
// Updated the leading license information of the file to reflect new maintainer.
//
// Revision 1.8  1999/11/29 19:06:50  scott
// Updated documentation to work better with doc++ (probably doesn't work
// well with kdoc anymore).
//
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
