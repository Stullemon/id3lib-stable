/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000 Philip Oldaker.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
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
//
/////////////////////////////////////////////////////////////////////////////
// TextCollection.h : Declaration of the CTextCollection
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 18 Aug 2000   Philip Oldaker        Added Picture Functionality
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __TEXTCOLLECTION_H_
#define __TEXTCOLLECTION_H_

#include "resource.h"       // main symbols
#include "BSTRCollection.h"

/////////////////////////////////////////////////////////////////////////////
// CTextCollection
class CTextCollection : public CBSTRCollectionImpl<CTextCollection,&CLSID_TextCollection,ITextCollection,&IID_ITextCollection>
{
public:
	CTextCollection();

DECLARE_REGISTRY_RESOURCEID(IDR_TEXTCOLLECTION)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CTextCollection)
	COM_INTERFACE_ENTRY(ITextCollection)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ITextCollection
public:
};

#endif //__TEXTCOLLECTION_H_
