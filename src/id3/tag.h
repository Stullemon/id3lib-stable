// $Id: tag.h,v 1.8 1999/11/25 19:22:18 scott Exp $
//
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

#ifndef ID3LIB_TAG_H
#define ID3LIB_TAG_H

#include <wchar.h>
#include <stdio.h>
#include <id3/types.h>
#include <id3/frame.h>
#include <id3/header_frame.h>
#include <id3/header_tag.h>
#include <id3/version.h>

// for file buffers etc
#define BUFF_SIZE (65536)

struct ID3_Elem
{
  ID3_Elem  *pNext;
  ID3_Frame *pFrame;
  uchar     *acBinary;
  bool       bTagOwns;
};

/**
 * The length of an id3v1 tag
 *
 * @see ID3V1_Tag
 */
const luint LEN_V1         = 128;
/**
 * The length of the id field in an id3v1 tag
 *
 * @see ID3V1_Tag#sID
 */
const luint LEN_V1_ID      =   3;
/**
 * The length of the title field in an id3v1 tag
 *
 * @see ID3V1_Tag#sTitle
 */
const luint LEN_V1_TITLE   =  30;
/**
 * The length of the artist field in an id3v1 tag
 *
 * @see ID3V1_Tag@sArtist
 */
const luint LEN_V1_ARTIST  =  30;
/**
 * The length of the album field in an id3v1 tag
 *
 * @see ID3V1_Tag#sAlbum
 */
const luint LEN_V1_ALBUM   =  30;
/**
 * The length of the year field in an id3v1 tag
 *
 * @see ID3V1_Tag#sYear
 */
const luint LEN_V1_YEAR    =   4;
/**
 * The length of the comment field in an id3v1 tag
 *
 * @see ID3V1_Tag#sComment
 */
const luint LEN_V1_COMMENT =  30;
/**
 * The length of the genre field in an id3v1 tag
 *
 * @see ID3V1_Tag#ucGenre
 */
const luint LEN_V1_GENRE   =   1;

/**
 * A structure for storing the contents of an id3v1 tag.
 * @short A structure for storing the contents of an id3v1 tag.
 * @author Dirk Mahoney and Scott Thomas Haug (sth2@cs.wustl.edu)
 * @version $Id: tag.h,v 1.8 1999/11/25 19:22:18 scott Exp $
 * @see ID3_Tag
 */
struct ID3V1_Tag
{
  /**
   * String for storing the "TAG" identifier.
   *
   * @see LEN_V1_ID
   */
  char sID     [1 + LEN_V1_ID];
  /**
   * String for storing the title
   *
   * @see LEN_V1_TITLE
   */
  char sTitle  [1 + LEN_V1_TITLE];
  /**
   * String for storing the artist
   *
   * @see LEN_V1_ARTIST
   */
  char sArtist [1 + LEN_V1_ARTIST];
  /**
   * String for storing the album
   *
   * @see LEN_V1_ALBUM
   */
  char sAlbum  [1 + LEN_V1_ALBUM];
  /**
   * String for storing the year
   *
   * @see LEN_V1_YEAR
   */
  char sYear   [1 + LEN_V1_YEAR];
  /**
   * String for storing the comment
   *
   * @see LEN_V1_COMMENT
   */
  char sComment[1 + LEN_V1_COMMENT];
  /**
   * Byte for storing the genre
   *
   * @see LEN_V1_GENRE
   */
  uchar ucGenre;
};

/**
 * String used for the description field of a comment tag converted from an
 * id3v1 tag to an id3v2 tag
 *
 * @see #ID3V1_Tag
 */
const char * const STR_V1_COMMENT_DESC = "ID3v1_Comment";

/**
 * Flag used to represent the id3v1 tag
 *
 * @see ID3_Tag#Update
 * @see ID3_Tag#Strip
 */
enum ID3_TagTypes
{
  NO_TAG        =      0,
  V1_TAG        = 1 << 0,
  V2_TAG        = 1 << 1,
  LYRICS_TAG    = 1 << 2
};

const luint BOTH_ID3_TAGS = V1_TAG | V2_TAG;
const luint ALL_TAG_TYPES = BOTH_ID3_TAGS | LYRICS_TAG;

/** 
 * This is the 'container' class for everything else.  It is through an
 * ID3_Tag that most of the productive stuff happens.  Let's look at what's
 * required to start using ID3v2 tags.
 *
 * <pre>
 * #include <id3/tag.h>
 * </pre>
 *
 * This simple #include does it all.  In order to read an existing tag...
 *
 * <pre>
 * ID3_Tag myTag;
 * myTag.Link("something.mp3");
 * </pre>
 *
 * That is all there is to it.  Now all you have to do is use the @ref #Find
 * method to locate the frames you are interested in...
 *
 * <pre>
 * ID3_Frame *myFrame;
 * if (myTag.Find(ID3FID_TITLE) == myFrame)
 * {
 *   char title[1024];
 *   myFrame->Field(ID3FN_TEXT).Get(title, 1024);
 *   cout << "Title: " << title << endl;
 * }
 * </pre>
 *
 * This code snippet locates the TITLE frame and copies the contents of the
 * text field into a buffer and displays the buffer.  Not difficult, eh?
 *
 * When using the @ref #Link method of an ID3_Tag object, you automatically
 * gain access to any ID3v1/1.1, ID3v2, and Lyrics3 v2.0 tags present in the
 * file.  The class will automaticaly parse and convert any of these foreign
 * tag formats into ID3v2 tags.  Also, id3lib will correctly parse any
 * correctly formatted 'CDM' frames from the unreleased 2.01 draft taht id3lib
 * 2.16 supports.
 *
 * @short The representative class of an id3 tag.
 * @author Dirk Mahoney
 * @version $Id: tag.h,v 1.8 1999/11/25 19:22:18 scott Exp $
 * @see ID3_Frame
 * @see ID3_Field
 * @see ID3_Err
 */
class ID3_Tag
{
public:
  /**
   * The ID3_Tag constructor can be called without any parameters.
   * Alternatively, it can accept a single parameter that is a pointer to a
   * file name.  If this file exists, it will be opened and all
   * id3lib-supported tags will be parsed and converted to id3v2 if necessary.
   * After the conversion, the file will remain unchanged, and will continue to
   * do so until you use the @ref #Update method on the tag (if you choose to
   * @ref #Update at all).
   *
   * @param name The filename of the mp3 file to link to
   */
  ID3_Tag(char *name = NULL);
  ~ID3_Tag(void);
  
  /**
   * This method simply clears the object and disassociates it from any files.
   * It frees any resources for which the object is responsible, and the object
   * is now free to be used again for any new or existing tag.
   */
  void       Clear(void);

  /**
   * This method indicates whether the tag has been altered since the last
   * parse, render, or update.  If you have a tag linked to a file, you do not
   * need this method since the @ref #Update method will check for
   * changes before writing the tag.
   *
   * This method is primarily intended as a status indicator for applications
   * and for applications that use the @ref #Parse and @ref #Render
   * methods.
   *
   * Setting a field, changed the ID of an attached frame, setting or grouping
   * or encryption IDs, and clearing a frame or field all constitute a change
   * to the tag, as do calls to the @ref #SetUnsync,
   * @ref #SetExtendedHeader, @ref #SetPadding, and
   * @ref #SetCompression methods.
   *
   * <pre>
   * if (myTag.HasChanged())
   * {
   *   // render and output the tag
   * }
   * </pre>
   *
   * @return Whether or not the tag has been altered.
   */
  bool       HasChanged(void) const;

  /** 
   * This method takes a single boolean parameter and tells the object whether
   * you wish the resulting binary tag to be unsynchronised if unsync is
   * required.  If you call this method with 'false' as the parameter, the
   * binary tag will not be unsync'ed, regardless of whether the tag should be.
   * This option is useful when the file is only going to be used by
   * ID3v2-compliant software.  See the id3v2 standard document for futher
   * details on unsync.
   *
   * Be default, tags are created without unsync.
   *
   * <pre>
   * myTag.SetUnsync(false);
   * </pre>
   *
   * @param bSync Whether the tag should be unsynchronized
   */
  void       SetUnsync(bool bSync);

  /** 
   * This method tells the tag whether to create an extended header when
   * rendering the tag.  This option is currently ignored as id3lib doesn't yet
   * create extended headers.  This option only applies when rendering tags for
   * id3v2 versions that support extended headers.
   *
   * By default, id3lib will generate extended headers for all tags in which
   * extended headers are supported.
   *
   * <pre>
   * myTag.SetExtendedHeader(true);
   * </pre>

   * @param bExt Whether to render an extended header
   */
  void       SetExtendedHeader(bool bExt);

  /** 
   * Under id3v2-3.0, there is support for the compression of data in tags.
   * The current standard defines compression on a frame-by-frame basis using
   * the zlib data compression standard.  Using this call, you tell the object
   * whether to attempt to compress frames or not.  If a frame doesn't compress
   * well, it will not be compressed, regardless of this setting.  This
   * method takes a single boolean parameter.  By default, id3lib will
   * attempt to compress all the frames in a tag.
   *
   * <pre>
   * myTag.SetCompression(true);
   * </pre>
   *
   * When creating tags for a version of id3v2 in which compression isn't
   * defined, the tags are simply rendered without compression to ensure
   * compliance to the relevant version of the standard.
   *
   * @param bComp Whether to render the tag's frames in a compressed format.
   */
  void       SetCompression(bool bComp);

  /** 
   * When using id3v2 tags in association with files, id3lib can optionally
   * add padding to the tags to ensure minmal file write times when updating
   * the tag in the future.
   * 
   * When the padding option is switched on, id3lib automatically creates
   * padding according to the 'ID3v2 Programming Guidelines'.  Specifically,
   * enough padding will be added to round out the entire file (song plus tag)
   * to an even multiple of 2K.  Padding will only be created when the tag is
   * attached to a file and that file is not empty (aside from a pre-existing
   * tag).
   *
   * My own addition to the guidelines for padding, and that which id3lib
   * follows, is that if frames are removed from a pre-existing tag (or the tag
   * simply shrinks because of other reasons), the new tag will continue to
   * stay the same size as the old tag (with padding making the difference of
   * course) until such time as the padding is greater than 4K.  When this
   * happens, the padding will be reduced and the new tag will be smaller than
   * the old.
   *
   * By default, padding is switched on.
   *
   * <pre>
   * myTag.SetPadding(false);
   * </pre>
   *
   * @param bPad Whether or not render the tag with padding.
   */
  void       SetPadding(bool bPad);

  /** 
   * To attach a frame object (see @ref ID3_Frame) to a tag object, you can
   * use the @ref #AddFrame or @ref #operator<< methods.  To use the
   * @ref AddFrame method, simply supply as it's sole parameter a
   * pointer to the @ref ID3_Frame object you wish to attach.
   *
   * <pre>
   * ID3_Frame myFrame;
   * myTag.AddFrame(&myFrame);
   * </pre> 
   *
   * As implied, this method attaches the frames to the tag---the tag does not
   * create its own copy of the frame.  Frames created by an application must
   * exist until the frame is removed or the tag is finished with it.
   *
   * @param pFrame A pointer to the frame that is being added to the tag.
   */
  void       AddFrame(ID3_Frame *pFrame);

  /** 
   * Similar to @ref #AddFrame, AddNewFrame also adds frames to the tag object.
   * However, unlike @ref #AddFrame, AddNewFrame accepts responsibility for
   * the added frame, and will delete the frame and its contents when the tag
   * goes out of scope or is deleted.
   *
   * <pre>
   * ID3_Frame *myFrame = new ID3_Frame;
   * myTag.AddFrame(myFrame);
   * </pre> 
   *
   * @param pNewFrame A pointer to the frame that is being added to the tag.
   */
  void       AddNewFrame(ID3_Frame *pNewFrame);

  /**
   * As an alternative to the AddFrame method, operator<< can be used to attach
   * a frame to a tag.  The << operator supports the addition of a pointer to a
   * frame object, or the frame object itself.
   * 
   * <pre>
   * ID3_Frame *framePoint, frameObj;
   * myTag << framePoint;
   * myTag << frameObj;
   * </pre>
   *
   *
   * Both these methods attach the given frame to the tag---the tag does not
   * create its own copy of the frame.  Frames created by an application must
   * exist until the frame is removed or the tag is finished with it.
   *
   * @param frame The frame to be added to the tag.
   */
  ID3_Tag& operator<<(ID3_Frame &frame);
  /**
   * @param frame The pointer to the frame to be added to the tag.
   */
  ID3_Tag& operator<<(ID3_Frame *frame);

  /**
   * This method takes a pointer to an array of @ref ID3_Frame objects and a
   * number that specifies the number of objects in the array.  id3lib will
   * then go and add all the frames to the tag.
   *
   * <pre>
   * ID3_Frame myFrames[10];
   * myTag.AddFrames(myFrames, 10);
   * </pre>
   *
   * @see ID3_Frame#AddFrame
   * @param pNewFrames A pointer to an array of frames to be added to the tag.
   * @param nFrames The number of frames in the array pNewFrames. 
   */
  void AddFrames(ID3_Frame *pNewFrames, luint nFrames);

  /**
   * If you wish to remove a frame from a tag, use this method.  If you
   * already own the frame object in question, then you should already have a
   * pointer to the frame you want to delete.  If not, or if you wish to delete
   * a pre-existing frame (from a tag you have parsed, for example), the use
   * one of the @ref #Find methods to obtain a frame pointer to pass to
   * this method.
   *
   * <pre>
   * ID3_Frame *someFrame;
   * if (someFrame = myTag.Find(ID3FID_TITLE))
   *   myTag.RemoveFrame(someFrame);
   * </pre> 
   *
   * @see ID3_Tag#Find
   * @param pOldFrame A pointer to the frame that is to be removed from the tag
   */
  void       RemoveFrame(ID3_Frame *pOldFrame);

  /** 
   * This renders a binary image of the tag into the supplied buffer.  See
   * @ref #Size for an example.  This method returns the actual number
   * of the bytes of the buffer used to store the tag.  This will no more than
   * the size of the buffer itself, because @ref #Size over estimates the
   * required buffer size when padding is enabled.
   *
   * Before calling this method, it is advisable to call @ref #HasChanged
   * first as this will let you know whether you should bother rendering the
   * tag.
   *
   * @see ID3_IsTagHeader
   * @see ID3_Tag#HasChanged
   * @return The actual number of the bytes of the buffer used to store the tag
   * @param buffer The buffer that will contain the rendered tag.
   */
  luint      Render(uchar *buffer);

  /** 
   * This renders an id3v1.1 version of the tag into the supplied buffer.
   *
   * @return The actual number of the bytes of the buffer used to store the tag
   *         (should always be 128)
   * @param buffer The buffer that will contain the id3v1.1 tag.
   */
  luint      RenderV1(char *buffer);

  /**
   * This method returns an over estimate of the number of bytes required to
   * store a binary version of a tag.  When using @ref #Render to render a
   * binary tag to a memory buffer, first use the result of this call to
   * allocate a buffer of unsigned chars.
   *
   * <pre>
   * luint tagSize;
   * uchar *buffer;
   * if (myTag.HasChanged())
   * {
   *   if ((tagSize= myTag.Size()) > 0)
   *   {
   *     if (buffer = new uchar[tagSize])
   *     {
   *       luint actualSize = myTag.Render(buffer);
   *       // do something useful with the first
   *       // 'actualSize' bytes of the buffer,
   *       // like push it down a socket
   *       delete [] buffer;
   *     }
   *   }
   * }
   * </pre>
   *
   * @see
   * @return The (overestimated) number of bytes required to store a binary
   *         version of a tag
   */
  luint      Size(void) const;

  /** 
   * This will turn a binary tag into a series of @ref ID3_Frame objects 
   * attached to the specified tag.  
   *
   * <pre>
   * ID3_Tag myTag;
   * uchar header[ID3_TAGHEADERSIZE];
   * uchar *buffer;
   * luint tagSize;
   *
   * // get ID3_TAGHEADERSIZE from a socket or somewhere
   * ...
   *
   * if ((tagSize = ID3_IsTagHeader(ourSourceBuffer)) > -1)
   * {
   *   // read a further 'tagSize' bytes in
   *   // from our data source
   *   ...
   *
   *   if (buffer = new uchar[tagSize])
   *   {
   *     // now we will call ID3_Tag::Parse()
   *     // with these values (explained later)
   *     myTag.Parse(header, buffer);
   *
   *     // do something with the objects,
   *     // like look for titles, artists, etc.
   *     ...
   *
   *     // free the buffer
   *     delete [] buffer;
   *   }
   * }
   * </pre>
   *
   * @param header The byte header read in from the data source.
   * @param buffer The remainder of the tag (not including the data source) 
   *               read in from the data source.
   */
  void       Parse(uchar header[ID3_TAGHEADERSIZE], uchar *buffer);

  /** 
   * If you created your ID3_Tag object without supplying a parameter to the
   * constructor (maybe you create an array of ID3_Tag pointers), you can use
   * this method to attach the tag to a file.  This is the preferred method of
   * interacting with files, since id3lib can automatically do things like
   * parse foreign tag formats and handle padding when linked to a file.  When
   * a tag is linked to a file, you do not need to use the @ref #Size,
   * @ref #Render, or @ref #Parse methods or the @ref ID3_IsTagHeader
   * function---id3lib will take care of those details for you.  The single
   * parameter is a pointer to a file name.
   *
   * Link returns a 'luint' which is the byte position within the file that
   * the audio starts (i.e., where the id3v2 tag ends).
   *
   * <pre>
   * ID3_Tag *myTag;
   * if (myTag = new ID3_Tag)
   * {
   *   myTag->Link("mysong.mp3");
   *
   *   // do whatever we want with the tag
   *   ...
   *
   *   // setup all our rendering parameters
   *   myTag->SetUnsync(false);
   *   myTag->SetExtendedHeader(true);
   *   myTag->SetCompression(true);
   *   myTag->SetPadding(true);
   *
   *   // write any changes to the file
   *   myTag->Update()
   *
   *   // free the tag
   *   delete myTag;
   * }
   * </pre>
   *
   * @param fileInfo The filename of the file to link to.
   */
  luint      Link(char *fileInfo);

  /**
   * The method simply renders the tag and writes it to the file to which
   * this tag is attached.  Make sure you setup your rendering parameters
   * before calling the method.  See @ref #Link for an example of
   * this method in use.
   *
   * @param eTagFlag Flag which indicates which tag to update.
   */
  luint      Update(const luint ulTagFlag = V2_TAG);

  /** 
   * This method, when used with a tag attached to a file, will strip any
   * id3v1 or v2 tag (depending on the parameter) from that file.
   *
   * @param ulTagFlag Flag which indicates which tag to strip.  Possibilities
   *                  are V1_TAG, V2_TAG, and BOTH_TAGS.
   */
  luint      Strip(const luint ulTagFlag = BOTH_ID3_TAGS);

  /**
   * This method takes a required @ref ID3_FrameID parameter that specifies
   * the ID of the frame you wish to locate.  If present in the tag, the first
   * such frame will be returned as a pointer to an @ref ID3_Frame object.  If
   * no frame is found, this returns NULL.  If there are multiple frames in the
   * tag with the same ID (which, for some frames, is allowed), then subsequent
   * calls to @ref Find will return subsequent frame pointers, wrapping if
   * necessary.
   *
   * <pre>
   * ID3_Frame *myFrame;
   * if (myFrame = myTag.Find(ID3FID_TITLE))
   * {
   *   // do something with the frame, like copy
   *   // the contents into a buffer, display the
   *   // contents in a window, etc.
   *   ...
   * }
   * </pre>
   *
   * You may optionally supply to more parameters ot this method, being an @ref
   * ID3_FieldID and a value of some sort.  Depending on the field name/ID you
   * supply, you may supply an integer, a char* or a wchar_t* as the third
   * parameter.  If you supply an @ref ID3_FrameID, you must also supply a data
   * value to compare against.
   *
   * This method will then return the first frame that has a matching frame ID,
   * and which has a field with the same name as that which you supplied in the
   * second parameter, whose calue matches that which you supplied as the third
   * parameter.  For example:
   *
   * <pre>
   * ID3_Frame *myFrame;
   * if (myFrame = myTag.Find(ID3FID_TITLE, ID3FN_TEXT, "Nirvana"))
   * {
   *   // found it, do something with it.
   *   ...
   * }
   * </pre>
   *
   * This example will return the first TITLE frame and whose TEXT field is
   * 'Nirvana'.  Currently there is no provision for things like 'contains',
   * 'greater than', or 'less than'.  If there happens to be more than one of
   * these frames, subsequent calls to the @ref Find method will return
   * subsequent frames and will wrap around to the beginning.
   *
   * Another example...
   *
   * <pre>
   * ID3_Frame *myFrame;
   * if (myFrame = myTag.Find(ID3FID_COMMENT, ID3FN_TEXTENC, ID3TE_UNICODE))
   * {
   *   // found it, do something with it.
   *   ...
   * }
   * </pre>
   *
   * This returns the first COMMENT frame that uses Unicode as its text
   * encdoing.
   *
   * @param id The ID of the frame that is to be located
   * @returns A pointer to the first frame found that has the given frame id,
   *          or NULL if no such frame.  
   */
  ID3_Frame *Find(ID3_FrameID id);

  /**
   */
  ID3_Frame *Find(ID3_FrameID id, ID3_FieldID fld, luint data);

  /**
   */
  ID3_Frame *Find(ID3_FrameID id, ID3_FieldID fld, char *data);

  /**
   */
  ID3_Frame *Find(ID3_FrameID id, ID3_FieldID fld, wchar_t *data);

  /**
   * This returns the number of frames present in the tag object.  This
   * includes only those frames that id3lib recognises.  This is used as the
   * upper bound on calls to the @ref #GetFrame() and @ref #operator[] methods.
   *
   * @returns The number of frames present in the tag object
   */
  luint      NumFrames(void) const;

  /**
   * This method (along with @ref #operator[]) returns a pointer to the given
   * frame number, or NULL if the specified index doesn't exist.  Indexing is
   * 0-based (that is, the first frame is number 0, and the last frame in a tag
   * that holds n frames is n-1).
   *
   * If you wish to have a more comlex searching facility, then at least for
   * now you will have to devise it yourself and implement it useing these
   * methods.
   *
   * @param nIndex The index of the frame that is to be retrieved
   * @returns A pointer to the requested frame, or NULL if no such frame. 
   */
  ID3_Frame *GetFrameNum(luint nIndex) const;

  /**
   * @param nIndex The index of the frame that is to be retrieved
   * @returns A pointer to the requested frame, or NULL if no such frame. 
   * @see #GetFrameNum
   */
  ID3_Frame *operator[](luint nIndex) const;
  
private:
  void      AddFrame(ID3_Frame *pNewFrame, bool bFreeWhenDone);
  void      AddFrames(ID3_Frame *newFrames, luint nFrames, bool freeWhenDone);
  void      SetupTag(char *fileInfo);
  void      SetVersion(uchar ver, uchar rev);
  void      ClearList(ID3_Elem *list);
  void      DeleteElem(ID3_Elem *cur);
  void      AddBinary(uchar *buffer, luint size);
  void      ExpandBinaries(uchar *buffer, luint size);
  void      ProcessBinaries(ID3_FrameID whichFrame = ID3FID_NOFRAME, bool attach = true);  // this default means all frames
  void      RemoveFromList(ID3_Elem *which, ID3_Elem **list);
  ID3_Elem *GetLastElem(ID3_Elem *list);
  ID3_Elem *Find(ID3_Frame *frame) const;
  luint     PaddingSize(luint curSize) const;
  void      RenderExtHeader(uchar *buffer);
  void      OpenFileForWriting(void);
  void      OpenFileForReading(void);
  void      CreateFile(void);
  bool      CloseFile(void);
  void      RenderV1ToHandle(void);
  void      RenderV2ToHandle(void);
  luint     ParseFromHandle(void);
  void      ParseID3v1(void);
  void      ParseLyrics3(void);
  luint     GetUnSyncSize(uchar *buffer, luint size);
  void      UnSync(uchar *destData, luint destSize, uchar *sourceData, luint sourceSize);
  luint     ReSync(uchar *binarySourceData, luint sourceSize);

  uchar     __ucVersion;      // what version tag?
  uchar     __ucRevision;     // what revision tag?
  ID3_Elem *__pFrameList; // the list of known frames currently attached to this tag
  ID3_Elem *__pBinaryList;// the list of yet-to-be-parsed frames currently attached to this tag
  ID3_Elem *__pFindCursor;// on which element in the frameList are we currently positioned?
  bool      __bSyncOn;         // should we unsync this tag when rendering?
  bool      __bCompression;    // should we compress frames when rendering?
  bool      __bPadding;        // add padding to tags?
  bool      __bExtendedHeader; // create an extended header when rendering?
  bool      __bHasChanged;     // has the tag changed since the last parse or render?
  bool      __bFileWritable;   // is the associated file (via Link) writable?
  FILE     *__fFileHandle;     // a handle to the file we are linked to
  char     *__sFileName;       // the name of the file we are linked to
  luint     __ulFileSize;      // the size of the file (without any tag)
  luint     __ulOldTagSize;    // the size of the old tag (if any)
  luint     __ulExtraBytes;    // extra bytes to strip from end of file (ID3v1 and Lyrics3 tags)
  bool      __bHasV1Tag;       // does the file have an ID3v1 tag attached?
  static luint s_ulInstances;  // how many ID3_Tag objects are floating around in this app?
}
;

#endif

// $Log: tag.h,v $
// Revision 1.8  1999/11/25 19:22:18  scott
// (): Added doc++/kdoc/javadoc-like documentation, mostly transcribed
// from Dirk's ID3Lib Documentation MSWord doc.
// (ID3_TagTypes): Added.  The different types of tags that can be parsed
// and/or rendered by id3lib.
// (class ID3_Tag): Changed the interface to AddFrame slightly.  Now there
// is two AddTag-like methods: AddTag and AddNewTag.  The former merely
// attaches the frame to the tag without taking repsonsibility for
// deallocating the frame when the tag goes out of scope or is deleted.
// The latter does take this responsibility.  Granted, there could be a
// better interface for this, but it works for now...  Update and Strip
// now return flags which indicate what tags were actually updated or
// stripped.  Removed OpenLinkedFile method; replaced with
// OpenFileForReading, OpenFileForWriting, and CreateFile.
//
// Revision 1.7  1999/11/19 19:07:13  scott
// * tag.h: Added new constants: STR_V1_COMMENT_DESC (to be added to
// the description of comments converted from id3v1 tags); V1_TAG,
// V2_TAG, and BOTH_TAGS (used for methods Strip and Update to
// determine which tag to act on); and LEN_V1, LEN_V1_ID,
// LEN_V1_TITLE, LEN_V1_ARTIST, LEN_V1_ALBUM, LEN_V1_YEAR,
// LEN_V1_COMMENT, LEN_V1_GENRE (the lengths of the id3v1 tag and its
// fields). Generalized ID3V1_Tag struct using newly defined
// constants.  Added 1 to each char array size for null terminator.
// Added const qualifier to appropriate methods and parameters.  Added
// declaration of RenderV1ToHandle method.  Renamed "RenderToHandle" to
// "RenderV2ToHandle".
//
// Revision 1.6  1999/11/15 20:20:30  scott
// Made variable names more descriptive.  Added const qualifier to
// appropriate methods.
//
// Revision 1.5  1999/11/04 04:32:11  scott
// Initial revision
//
// Revision 1.4  1999/11/04 04:15:55  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
