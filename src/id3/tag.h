// $Id: tag.h,v 1.6 1999/11/15 20:20:30 scott Exp $

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

struct ID3V1_Tag
{
  char sID[3];
  char sTitle[30];
  char sArtist[30];
  char sAlbum[30];
  char sYear[4];
  char sComment[30];
  uchar ucGenre;
};

class ID3_Tag
{
public:
  ID3_Tag(char *name = NULL);
  ~ID3_Tag(void);
  
  void       Clear(void);
  bool       HasChanged(void) const;
  void       SetUnsync(bool newSync);
  void       SetExtendedHeader(bool ext);
  void       SetCompression(bool comp);
  void       SetPadding(bool pad);
  void       AddFrame(ID3_Frame *newFrame, bool freeWhenDone = false);
  void       AddFrames(ID3_Frame *newFrames, luint numFrames, bool freeWhenDone = false);
  void       RemoveFrame(ID3_Frame *oldFrame);
  luint      Render(uchar *buffer);
  luint      Size(void) const;
  void       Parse(uchar header[ID3_TAGHEADERSIZE], uchar *buffer);
  luint      Link(char *fileInfo);
  void       Update(void);
  void       Strip(bool v1Also = true);
  ID3_Frame *Find(ID3_FrameID id);
  ID3_Frame *Find(ID3_FrameID id, ID3_FieldID fld, luint data);
  ID3_Frame *Find(ID3_FrameID id, ID3_FieldID fld, char *data);
  ID3_Frame *Find(ID3_FrameID id, ID3_FieldID fld, wchar_t *data);
  luint      NumFrames(void) const;
  ID3_Frame *GetFrameNum(luint num) const;
  ID3_Frame *operator[](luint num) const;
  
private:
  void      SetupTag(char *fileInfo);
  void      SetVersion(uchar ver, uchar rev);
  void      ClearList(ID3_Elem *list);
  void      DeleteElem(ID3_Elem *cur);
  void      AddBinary(uchar *buffer, luint size);
  void      ExpandBinaries(uchar *buffer, luint size);
  void      ProcessBinaries(ID3_FrameID whichFrame = ID3FID_NOFRAME, bool attach = true);  // this default means all frames
  void      RemoveFromList(ID3_Elem *which, ID3_Elem **list);
  ID3_Elem *GetLastElem(ID3_Elem *list);
  ID3_Elem *Find(ID3_Frame *frame);
  luint     PaddingSize(luint curSize) const;
  void      GenerateTempName(void);
  void      RenderExtHeader(uchar *buffer);
  void      OpenLinkedFile(void);
  void      RenderToHandle(void);
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
  FILE     *__fFileHandle;    // a handle to the file we are linked to
  char      __sFileName[256];  // the name of the file we are linked to
  char      __sTempName[256];  // a temp name in case we need a temp file for copying stuff
  luint     __ulFileSize;      // the size of the file (without any tag)
  luint     __ulOldTagSize;    // the size of the old tag (if any)
  luint     __ulExtraBytes;    // extra bytes to strip from end of file (ID3v1 and Lyrics3 tags)
  bool      __bHasV1Tag;       // does the file have an ID3v1 tag attached?
  static luint s_ulInstances;// how many ID3_Tag objects are floating around in this app?
}
;

ID3_Tag& operator<<(ID3_Tag& tag, ID3_Frame& frame);
ID3_Tag& operator<<(ID3_Tag& tag, ID3_Frame *frame);

#endif

// $Log: tag.h,v $
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
