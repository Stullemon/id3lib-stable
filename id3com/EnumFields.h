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
// EnumFields,h
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 05 Jan 2000   John Adcock           Original Release    
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __ENUMFIELDS_H_
#define __ENUMFIELDS_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CEnumFields
class ATL_NO_VTABLE CEnumFields : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IEnumVARIANT
{
public:
	CEnumFields();
	~CEnumFields();

	static IEnumVARIANT* CreateObject(IID3Tag* Tag);

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CEnumFields)
	COM_INTERFACE_ENTRY(IEnumVARIANT)
END_COM_MAP()

	STDMETHOD(Next)(ULONG celt, VARIANT* rgelt, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)(void);
	STDMETHOD(Clone)(IEnumVARIANT ** ppenum);
protected:
	IID3Tag* m_Tag;
	long m_CurrentNum;
};

#endif //__ENUMVBWORKITEMS_H_
