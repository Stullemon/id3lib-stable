/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000 John Adcock.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
//	This file is subject to the terms of the GNU General Public License as
//	published by the Free Software Foundation.  A copy of this license is
//	included with this software distribution in the file COPYING.  If you
//	do not have a copy, you may obtain a copy by writing to the Free
//	Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
//	This software is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details
//
/////////////////////////////////////////////////////////////////////////////
// ID3Frame.h : Declaration of the CID3Frame
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 05 Jan 2000   John Adcock           Original Release    
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __ID3FRAME_H_
#define __ID3FRAME_H_

#include "resource.h"       // main symbols
#include <id3/tag.h>
#include <id3/dll.h>

/////////////////////////////////////////////////////////////////////////////
// CID3Frame
class ATL_NO_VTABLE CID3Frame : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CID3Frame, &CLSID_ID3Frame>,
	public ISupportErrorInfo,
	public IDispatchImpl<IID3Frame, &IID_IID3Frame, &LIBID_ID3COMLib>
{
public:
	CID3Frame();
	~CID3Frame();
	
	static IID3Frame* CreateObject(IID3Tag* TagParent, ID3_Frame* Frame);

DECLARE_REGISTRY_RESOURCEID(IDR_ID3FRAME)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CID3Frame)
	COM_INTERFACE_ENTRY(IID3Frame)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IID3Frame
public:
	STDMETHOD(get_FrameName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ID)(/*[out, retval]*/ eID3FrameTypes *pVal);
	STDMETHOD(put_ID)(/*[in]*/ eID3FrameTypes newVal);
	STDMETHOD(Clear)();
	STDMETHOD(get_Field)(/*[in]*/ eID3FieldTypes FieldType, /*[out, retval]*/ IID3Field** pVal);

protected:
	ID3_Frame* m_Frame;
	IID3Tag* m_TagParent;
};

#endif //__ID3FRAME_H_
