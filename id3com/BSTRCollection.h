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
// BSTRCollection.h : Declaration of the CTextCollection
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 18 Aug 2000   Philip Oldaker        Added Picture Functionality
//
/////////////////////////////////////////////////////////////////////////////


#ifndef __BSTRCOLLECTION_H_
#define __BSTRCOLLECTION_H_

#include "resource.h"       // main symbols

struct _CopyVariantFromAdaptBstr {
	static HRESULT copy(VARIANT* p1, CAdapt<CComBSTR>* p2) {
    p1->vt = VT_BSTR;
    p1->bstrVal = p2->m_T.Copy();
    return (p1->bstrVal ? S_OK : E_OUTOFMEMORY);
  }
	
  static void init(VARIANT* p)    { VariantInit(p); }
  static void destroy(VARIANT* p) { VariantClear(p); }
};

struct _CopyBstrFromAdaptBstr {
	static HRESULT copy(BSTR* p1, CAdapt<CComBSTR>* p2) {
    *p1 = SysAllocString(p2->m_T);
    return (p1 ? S_OK : E_OUTOFMEMORY);
  }
	
  static void init(BSTR* p)    { }
  static void destroy(BSTR* p) { SysFreeString(*p); }
};


/////////////////////////////////////////////////////////////////////////////
// CBSTRCollection
template <class TCPPCLASS,const CLSID *TCLASSID,typename TI,const IID *piid>
class ATL_NO_VTABLE CBSTRCollectionImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<TCPPCLASS, TCLASSID>,
	public ISupportErrorInfo,
	public IObjectSafetyImpl<TCPPCLASS, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
	public ICollectionOnSTLImpl<IDispatchImpl<TI, piid>,
							std::vector< CAdapt< CComBSTR > >,
							BSTR,
							_CopyBstrFromAdaptBstr,
							CComEnumOnSTL<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT,
							_CopyVariantFromAdaptBstr,
							std::vector< CAdapt< CComBSTR > > > >
{
public:
	CBSTRCollectionImpl()
	{
	}

BEGIN_COM_MAP(TCPPCLASS)
	COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

	STDMETHODIMP AddText(LPCTSTR pszText)
	{
		CComBSTR bstr(pszText);
		m_coll.push_back(bstr);
		return S_OK;
	}
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] = 
		{
			piid
		};
		for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i],riid))
				return S_OK;
		}
		return S_FALSE;
	}
// IBSTRCollection
public:
};

#endif //__BSTRCOLLECTION_H_
