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
// EnumFields.cpp : Implementation of CEnumFields
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 05 Jan 2000   John Adcock           Original Release    
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ID3COM.h"
#include "EnumFields.h"
#include <comdef.h>

/////////////////////////////////////////////////////////////////////////////
// CEnumFields
CEnumFields::CEnumFields()
{
	m_Tag = NULL;
	m_CurrentNum = 0;
}

CEnumFields::~CEnumFields()
{
	if(m_Tag != NULL)
	{
		m_Tag->Release();
	}
}

IEnumVARIANT* CEnumFields::CreateObject(IID3Tag* Tag)
{
	CComObject<CEnumFields>* pRetVal = new CComObject<CEnumFields>;
	pRetVal->m_Tag = Tag;
	pRetVal->m_Tag->AddRef();
	return pRetVal;
}

STDMETHODIMP CEnumFields::Next(ULONG celt, VARIANT* rgelt, ULONG * pceltFetched)
{
	long NumItems;
	m_Tag->get_NumFrames(&NumItems);
	for(long i(0); i < (long)((pceltFetched==NULL)?1:(*pceltFetched)); i++)
	{
		if(m_CurrentNum + i >= NumItems)
		{
			return S_FALSE;
		}
		else
		{
			IID3Frame* Frame;
			m_Tag->get_FrameByNum(m_CurrentNum + i, &Frame);
			*(_variant_t*)rgelt = Frame;
			Frame->Release();
			rgelt++;
		}
	}
	m_CurrentNum += ((pceltFetched==NULL)?1:(*pceltFetched));
	return S_OK;
}

STDMETHODIMP CEnumFields::Skip(ULONG celt)
{
	m_CurrentNum += celt;
	return S_OK;
}

STDMETHODIMP CEnumFields::Reset(void)
{
	m_CurrentNum = 0;
	return S_OK;
}

STDMETHODIMP CEnumFields::Clone(IEnumVARIANT ** ppenum)
{
	CComObject<CEnumFields>* pRetVal = new CComObject<CEnumFields>;
	pRetVal->m_Tag = m_Tag;
	pRetVal->m_CurrentNum = m_CurrentNum;
	pRetVal->m_Tag->AddRef();
	*ppenum = pRetVal;
	return S_OK;
}

