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
// ID3Frame.cpp : Implementation of CID3Frame
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
#include "ID3Frame.h"
#include "ID3Field.h"

/////////////////////////////////////////////////////////////////////////////
// CID3Frame

CID3Frame::CID3Frame()
{
	m_Frame = NULL;
	m_TagParent = NULL;
}

CID3Frame::~CID3Frame()
{
	if(m_TagParent != NULL)
	{
		m_TagParent->Release();
	}
	m_Frame = NULL;
}

IID3Frame* CID3Frame::CreateObject(IID3Tag* TagParent, ID3_Frame* Frame)
{
	CComObject<CID3Frame>* pRetVal = new CComObject<CID3Frame>;
	pRetVal->m_Frame = Frame;
	pRetVal->m_TagParent = TagParent;
	TagParent->AddRef();
	return (IID3Frame*)pRetVal;
}

STDMETHODIMP CID3Frame::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IID3Frame
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CID3Frame::get_Field(eID3FieldTypes FieldType, IID3Field** pVal)
{
	try
	{
		*pVal = NULL;
		ID3_Field* Field = &(m_Frame->Field((enum ID3_FieldID)FieldType));
		*pVal = CID3Field::CreateObject(this, Field);
		(*pVal)->AddRef();
		return S_OK;
	}
	catch (ID3_Error err)
	{
		if(err.GetErrorID() == ID3E_FieldNotFound)
		{
			return S_OK;
		}
		else
		{
			return AtlReportError(CLSID_ID3Frame, err.GetErrorType(), IID_IID3Frame, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
		}
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Frame, "An unexpected error has occurred", IID_IID3Frame, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Frame::Clear()
{
	try
	{
		m_Frame->Clear();
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Frame, err.GetErrorType(), IID_IID3Frame, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Frame, "An unexpected error has occurred", IID_IID3Frame, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Frame::get_ID(eID3FrameTypes *pVal)
{
	try
	{
		*pVal = ID3_NOFRAME;
		*pVal = (eID3FrameTypes)m_Frame->GetID();
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Frame, err.GetErrorType(), IID_IID3Frame, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Frame, "An unexpected error has occurred", IID_IID3Frame, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Frame::put_ID(eID3FrameTypes newVal)
{
	try
	{
		m_Frame->SetID((ID3_FrameID)newVal);
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Frame, err.GetErrorType(), IID_IID3Frame, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Frame, "An unexpected error has occurred", IID_IID3Frame, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Frame::get_FrameName(BSTR *pVal)
{
	LPWSTR Desc;
	try
	{
		*pVal = NULL;
		ID3_FrameID FrameID = m_Frame->GetID();
		switch(FrameID)
		{
		case ID3_NOFRAME:
			Desc = L"No Frame";
			break;
		case ID3_ORIGALBUM:
			Desc = L"Original Album";
			break;
		case ID3_PUBLISHER:
			Desc = L"Publisher";
			break;
		case ID3_ENCODEDBY:
			Desc = L"Encoded By";
			break;
		case ID3_ENCODERSETTINGS:
			Desc = L"Encoder Settings";
			break;
		case ID3_ORIGFILENAME:
			Desc = L"Original Filename";
			break;
		case ID3_LANGUAGE:
			Desc = L"Language";
			break;
		case ID3_PARTINSET:
			Desc = L"Part In Set";
			break;
		case ID3_DATE:
			Desc = L"Date";
			break;
		case ID3_TIME:
			Desc = L"Time";
			break;
		case ID3_RECORDINGDATES:
			Desc = L"Recording Dates";
			break;
		case ID3_MEDIATYPE:
			Desc = L"Media Type";
			break;
		case ID3_FILETYPE:
			Desc = L"File Type";
			break;
		case ID3_NETRADIOSTATION:
			Desc = L"Netradio Station";
			break;
		case ID3_NETRADIOOWNER:
			Desc = L"Netradio Owner";
			break;
		case ID3_LYRICIST:
			Desc = L"Lyricist";
			break;
		case ID3_ORIGARTIST:
			Desc = L"Original Artist";
			break;
		case ID3_ORIGLYRICIST:
			Desc = L"Original Lyricist";
			break;
		case ID3_SUBTITLE:
			Desc = L"Subtitle";
			break;
		case ID3_MIXARTIST:
			Desc = L"Mix Artist";
			break;
		case ID3_USERTEXT:
			Desc = L"User Text";
			break;
		case ID3_CONTENTGROUP:
			Desc = L"Content Group";
			break;
		case ID3_TITLE:
			Desc = L"Title";
			break;
		case ID3_LEADARTIST:
			Desc = L"Lead Artist";
			break;
		case ID3_BAND:
			Desc = L"Band";
			break;
		case ID3_ALBUM:
			Desc = L"Album";
			break;
		case ID3_YEAR:
			Desc = L"Year";
			break;
		case ID3_CONDUCTOR:
			Desc = L"Conductor";
			break;
		case ID3_COMPOSER:
			Desc = L"Composer";
			break;
		case ID3_COPYRIGHT:
			Desc = L"Copyright";
			break;
		case ID3_CONTENTTYPE:
			Desc = L"Content Type";
			break;
		case ID3_TRACKNUM:
			Desc = L"Track Num";
			break;
		case ID3_COMMENT:
			Desc = L"Comment";
			break;
		case ID3_WWWAUDIOFILE:
			Desc = L"WWW Audio File";
			break;
		case ID3_WWWARTIST:
			Desc = L"WWW Artist";
			break;
		case ID3_WWWAUDIOSOURCE:
			Desc = L"WWW Audio Source";
			break;
		case ID3_WWWCOMMERCIALINFO:
			Desc = L"WWW Commercial Info";
			break;
		case ID3_WWWCOPYRIGHT:
			Desc = L"WWW Copyright";
			break;
		case ID3_WWWPUBLISHER:
			Desc = L"WWW Publisher";
			break;
		case ID3_WWWPAYMENT:
			Desc = L"WWW Payment";
			break;
		case ID3_WWWRADIOPAGE:
			Desc = L"WWW Radio Page";
			break;
		case ID3_WWWUSER:
			Desc = L"WWW User";
			break;
		case ID3_INVOLVEDPEOPLE:
			Desc = L"Involved People";
			break;
		case ID3_UNSYNCEDLYRICS:
			Desc = L"Unsynced Lyrics";
			break;
		case ID3_PICTURE:
			Desc = L"Picture";
			break;
		case ID3_GENERALOBJECT:
			Desc = L"General Object";
			break;
		case ID3_UNIQUEFILEID:
			Desc = L"Unique File ID";
			break;
		case ID3_PLAYCOUNTER:
			Desc = L"Play Counter";
			break;
		case ID3_POPULARIMETER:
			Desc = L"Popularity Meter";
			break;
		case ID3_GROUPINGREG:
			Desc = L"Grouping Reg";
			break;
		case ID3_CRYPTOREG:
			Desc = L"Crypto  Reg";
			break;
		default:
			Desc = L"Not Known";
			break;
		}
		*pVal = SysAllocString(Desc);
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Frame, err.GetErrorType(), IID_IID3Frame, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Frame, "An unexpected error has occurred", IID_IID3Frame, E_UNEXPECTED);
	}
}