/* $Id: id3.h,v 1.2 2000/04/21 05:47:09 eldamitri Exp $
 * 
 * id3lib: a software library for creating and manipulating id3v1/v2 tags
 * Copyright 1999, 2000  Scott Thomas Haug
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.

 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

 * The id3lib authors encourage improvements and optimisations to be sent to
 * the id3lib coordinator.  Please see the README file for details on where to
 * send such submissions.  See the AUTHORS file for a list of people who have
 * contributed to id3lib.  See the ChangeLog file for a list of changes to
 * id3lib.  These files are distributed with id3lib at
 * http://download.sourceforge.net/id3lib/
 */

#ifndef __ID3LIB_ID3_H__
#define __ID3LIB_ID3_H__

#include "id3/globals.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef struct { char __dummy; } ID3Tag;
  typedef struct { char __dummy; } ID3Frame;
  typedef struct { char __dummy; } ID3Field;

  // tag wrappers
  ID3Tag     *ID3Tag_New                  (void);
  void        ID3Tag_Delete               (ID3Tag *tag);
  void        ID3Tag_Clear                (ID3Tag *tag);
  bool        ID3Tag_HasChanged           (ID3Tag *tag);
  void        ID3Tag_SetUnsync            (ID3Tag *tag, bool unsync);
  void        ID3Tag_SetExtendedHeader    (ID3Tag *tag, bool ext);
  void        ID3Tag_SetCompression       (ID3Tag *tag, bool comp);
  void        ID3Tag_SetPadding           (ID3Tag *tag, bool pad);
  void        ID3Tag_AddFrame             (ID3Tag *tag, ID3Frame *frame);
  void        ID3Tag_AddFrames            (ID3Tag *tag, ID3Frame *frames, 
                                           luint num);
  void        ID3Tag_RemoveFrame          (ID3Tag *tag, ID3Frame *frame);
  void        ID3Tag_Parse                (ID3Tag *tag, 
                                           uchar header[ID3_TAGHEADERSIZE],
                                           uchar *buffer);
  luint       ID3Tag_Link                 (ID3Tag *tag, char *fileName);
  void        ID3Tag_Update               (ID3Tag *tag);
  void        ID3Tag_Strip                (ID3Tag *tag, luint ulTagFlags);
  ID3Frame   *ID3Tag_FindFrameWithID      (ID3Tag *tag, ID3_FrameID id);
  ID3Frame   *ID3Tag_FindFrameWithINT     (ID3Tag *tag, ID3_FrameID id,
                                           ID3_FieldID fld, luint data);
  ID3Frame   *ID3Tag_FindFrameWithASCII   (ID3Tag *tag, ID3_FrameID id,
                                           ID3_FieldID fld, char *data);
  ID3Frame   *ID3Tag_FindFrameWithUNICODE (ID3Tag *tag, ID3_FrameID id,
                                           ID3_FieldID fld, unicode_t *data);
  ID3Frame   *ID3Tag_GetFrameNum          (ID3Tag *tag, luint num);
  luint       ID3Tag_NumFrames            (ID3Tag *tag);

  /* frame wrappers */
  ID3Frame   *ID3Frame_New                (void);
  ID3Frame   *ID3Frame_NewID              (ID3_FrameID id);
  void        ID3Frame_Delete             (ID3Frame *frame);
  void        ID3Frame_Clear              (ID3Frame *frame);
  void        ID3Frame_SetID              (ID3Frame *frame, ID3_FrameID id);
  ID3_FrameID ID3Frame_GetID              (ID3Frame *frame);
  ID3Field   *ID3Frame_GetField           (ID3Frame *frame, ID3_FieldID name);

  /* field wrappers */
  void        ID3Field_Clear              (ID3Field *field);
  luint       ID3Field_Size               (ID3Field *field);
  luint       ID3Field_GetNumTextItems    (ID3Field *field);
  void        ID3Field_SetINT             (ID3Field *field, luint data);
  luint       ID3Field_GetINT             (ID3Field *field);
  void        ID3Field_SetUNICODE         (ID3Field *field, 
                                           unicode_t *string);
  luint       ID3Field_GetUNICODE         (ID3Field *field, unicode_t *buffer,
                                           luint maxChars, luint itemNum);
  void        ID3Field_AddUNICODE         (ID3Field *field, 
                                           unicode_t *string);
  void        ID3Field_SetASCII           (ID3Field *field, char *string);
  luint       ID3Field_GetASCII           (ID3Field *field, char *buffer,
                                           luint maxChars, luint itemNum);
  void        ID3Field_AddASCII           (ID3Field *field, char *string);
  void        ID3Field_SetBINARY          (ID3Field *field, uchar *data,
                                           luint size);
  void        ID3Field_GetBINARY          (ID3Field *field, uchar *buffer,
                                           luint buffLength);
  void        ID3Field_FromFile           (ID3Field *field, char *fileName);
  void        ID3Field_ToFile             (ID3Field *field, char *fileName);

#ifdef __cplusplus
}
#endif __cplusplus

#endif /* __ID3LIB_ID3H__ */
