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
// ID3Tag.h : Declaration of the CID3Tag
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 05 Jan 2000   John Adcock           Original Release    
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __ID3TAG_H_
#define __ID3TAG_H_

#include "resource.h"       // main symbols
#include <id3/tag.h>

/////////////////////////////////////////////////////////////////////////////
// CID3Tag
class ATL_NO_VTABLE CID3Tag : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CID3Tag, &CLSID_ID3Tag>,
	public ISupportErrorInfo,
	public IDispatchImpl<IID3Tag, &IID_IID3Tag, &LIBID_ID3COMLib>
{
public:
	CID3Tag();
	~CID3Tag();

DECLARE_REGISTRY_RESOURCEID(IDR_ID3TAG)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CID3Tag)
	COM_INTERFACE_ENTRY(IID3Tag)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IID3Tag
public:
	STDMETHOD(get_PercentVolumeAdjust)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_PercentVolumeAdjust)(/*[in]*/ double newVal);
	STDMETHOD(get_TagCreated)(/*[out, retval]*/ DATE *pVal);
	STDMETHOD(put_TagCreated)(/*[in]*/ DATE newVal);
	STDMETHOD(get_Popularity)(/*[in]*/ BSTR EMailAddress, /*[out, retval]*/ short *pVal);
	STDMETHOD(put_Popularity)(/*[in]*/ BSTR EMailAddress, /*[in]*/ short newVal);
	STDMETHOD(get_PlayCount)(/*[in]*/ BSTR EMailAddress, /*[out, retval]*/ long *pVal);
	STDMETHOD(put_PlayCount)(/*[in]*/ BSTR EMailAddress, /*[in]*/ long newVal);
	STDMETHOD(FindFrameString)(/*[in]*/ eID3FrameTypes FrameID, /*[in]*/ eID3FieldTypes FieldType, /*[in]*/ BSTR FindString, /*[in]*/ VARIANT_BOOL CreateNewIfNotFound, /*[out, retval]*/ IID3Frame** pVal);
	STDMETHOD(get_HasLyrics)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_HasV2Tag)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_HasV1Tag)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_LastPlayed)(/*[out, retval]*/ DATE *pVal);
	STDMETHOD(put_LastPlayed)(/*[in]*/ DATE newVal);
	STDMETHOD(get_Track)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Track)(/*[in]*/ long newVal);
	STDMETHOD(get_Year)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Year)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Genre)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Genre)(/*[in]*/ long newVal);
	STDMETHOD(get_Comment)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Comment)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Title)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Title)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Album)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Album)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Artist)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Artist)(/*[in]*/ BSTR newVal);
	STDMETHOD(StripV2Tag)();
	STDMETHOD(SaveV2Tag)();
	STDMETHOD(StripV1Tag)();
	STDMETHOD(SaveV1Tag)();
	STDMETHOD(get_FrameByNum)(/*[in]*/ long FrameNum, /*[out, retval]*/ IID3Frame** pVal);
	STDMETHOD(get_NumFrames)(/*[out, retval]*/ long *pVal);
	STDMETHOD(FindFrame)(/*[in]*/ eID3FrameTypes FrameID, /*[in]*/ VARIANT_BOOL CreateNewIfNotFound, /*[out, retval]*/ IID3Frame** pVal);
	STDMETHOD(get_HasChanged)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(Clear)();
	STDMETHOD(Link)(BSTR* FileName);
	STDMETHOD(get__NewEnum)(/*[out, retval]*/ IUnknown** pRetVal);
protected:
	ID3_Tag* m_ID3Tag;
};

#endif //__ID3TAG_H_
