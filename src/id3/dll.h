// $Id: dll.h,v 1.5 1999/11/25 18:15:06 scott Exp $

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


#ifndef ID3LIB_DLLHEADERS_H
#define ID3LIB_DLLHEADERS_H

#include <id3/globals.h>

class ID3_Field;
class ID3_Frame;
class ID3_Tag;

struct ID3_VerInfo
{
  char name[30];
  luint version;
  luint revision;
};


void        ID3_GetVersion(ID3_VerInfo *info);
// tag wrappers
ID3_Tag    *ID3Tag_New(void);
void        ID3Tag_Delete(ID3_Tag *tag);
void        ID3Tag_Clear(ID3_Tag *tag);
bool        ID3Tag_HasChanged(ID3_Tag *tag);
void        ID3Tag_SetUnsync(ID3_Tag *tag, bool unsync);
void        ID3Tag_SetExtendedHeader(ID3_Tag *tag, bool ext);
void        ID3Tag_SetCompression(ID3_Tag *tag, bool comp);
void        ID3Tag_SetPadding(ID3_Tag *tag, bool pad);
void        ID3Tag_AddFrame(ID3_Tag *tag, ID3_Frame *frame);
void        ID3Tag_AddFrames(ID3_Tag *tag, ID3_Frame *frames, luint num);
void        ID3Tag_RemoveFrame(ID3_Tag *tag, ID3_Frame *frame);
void        ID3Tag_Parse(ID3_Tag *tag, uchar header[ID3_TAGHEADERSIZE], uchar *buffer);
luint       ID3Tag_Link(ID3_Tag *tag, char *fileName);
void        ID3Tag_Update(ID3_Tag *tag);
void        ID3Tag_Strip(ID3_Tag *tag, bool v1Also);
ID3_Frame  *ID3Tag_FindFrameWithID(ID3_Tag *tag, ID3_FrameID id);
ID3_Frame  *ID3Tag_FindFrameWithINT(ID3_Tag *tag, ID3_FrameID id, ID3_FieldID fld, luint data);
ID3_Frame  *ID3Tag_FindFrameWithASCII(ID3_Tag *tag, ID3_FrameID id, ID3_FieldID fld, char *data);
ID3_Frame  *ID3Tag_FindFrameWithUNICODE(ID3_Tag *tag, ID3_FrameID id, ID3_FieldID fld, wchar_t *data);
ID3_Frame  *ID3Tag_GetFrameNum(ID3_Tag *tag, luint num);
luint       ID3Tag_NumFrames(ID3_Tag *tag);
// frame wrappers
void        ID3Frame_Clear(ID3_Frame *frame);
void        ID3Frame_SetID(ID3_Frame *frame, ID3_FrameID id);
ID3_FrameID ID3Frame_GetID(ID3_Frame *frame);
ID3_Field  *ID3Frame_GetField(ID3_Frame *frame, ID3_FieldID name);
// field wrappers
void        ID3Field_Clear(ID3_Field *field);
luint       ID3Field_Size(ID3_Field *field);
luint       ID3Field_GetNumTextItems(ID3_Field *field);
void        ID3Field_SetINT(ID3_Field *field, luint data);
luint       ID3Field_GetINT(ID3_Field *field);
void        ID3Field_SetUNICODE(ID3_Field *field, wchar_t *string);
luint       ID3Field_GetUNICODE(ID3_Field *field, wchar_t *buffer, luint maxChars, luint itemNum);
void        ID3Field_AddUNICODE(ID3_Field *field, wchar_t *string);
void        ID3Field_SetASCII(ID3_Field *field, char *string);
luint       ID3Field_GetASCII(ID3_Field *field, char *buffer, luint maxChars, luint itemNum);
void        ID3Field_AddASCII(ID3_Field *field, char *string);
void        ID3Field_SetBINARY(ID3_Field *field, uchar *data, luint size);
void        ID3Field_GetBINARY(ID3_Field *field, uchar *buffer, luint buffLength);
void        ID3Field_FromFile(ID3_Field *field, char *fileName);
void        ID3Field_ToFile(ID3_Field *field, char *fileName);

#endif

// $Log: dll.h,v $
// Revision 1.5  1999/11/25 18:15:06  scott
// Removed many of the enums and typedefs to globals.h to reduce redundancy.
//
// Revision 1.4  1999/11/04 04:15:54  scott
// Added cvs Id and Log tags to beginning and end of file, respectively.
//
