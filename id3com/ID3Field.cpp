// ID3Field.cpp : Implementation of CID3Field
#include "stdafx.h"
#include "ID3COM.h"
#include "ID3Field.h"

/////////////////////////////////////////////////////////////////////////////
// CID3Field

CID3Field::CID3Field()
{
	m_Field = NULL;
	m_FrameParent = NULL;
}

CID3Field::~CID3Field()
{
	if(m_FrameParent != NULL)
	{
		m_FrameParent->Release();
	}
	m_Field = NULL;
}

IID3Field* CID3Field::CreateObject(IID3Frame* FrameParent, ID3_Field* Field)
{
	CComObject<CID3Field>* pRetVal = new CComObject<CID3Field>;
	pRetVal->m_Field = Field;
	pRetVal->m_FrameParent = FrameParent;
	FrameParent->AddRef();
	return (IID3Field*)pRetVal;
}

STDMETHODIMP CID3Field::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IID3Field
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CID3Field::get_Text(long ItemNum, BSTR *pVal)
{
	luint TryChars = 0;
	luint GotChars = 0;
	try
	{
		*pVal = NULL;
		unicode_t* sText = NULL;
		size_t nText = m_Field->Size();
		sText = new unicode_t[nText + 1];
		try 
		{
			m_Field->Get(sText, nText);
			sText[nText] = '\0';
			*pVal = SysAllocString(sText);
		}
		catch (...)
		{
			*pVal = NULL;
		}
		delete [] sText;
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Field, err.GetErrorType(), IID_IID3Field, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Field, "An unexpected error has occurred", IID_IID3Field, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Field::put_Text(long ItemNum, BSTR newVal)
{
	try
	{
		m_Field->Set(newVal);
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Field, err.GetErrorType(), IID_IID3Field, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Field, "An unexpected error has occurred", IID_IID3Field, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Field::get_Long(long *pVal)
{
	try
	{
		*pVal = m_Field->Get();
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Field, err.GetErrorType(), IID_IID3Field, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Field, "An unexpected error has occurred", IID_IID3Field, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Field::put_Long(long newVal)
{
	try
	{
		m_Field->Set(newVal);
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Field, err.GetErrorType(), IID_IID3Field, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Field, "An unexpected error has occurred", IID_IID3Field, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Field::Clear()
{
	try
	{
		m_Field->Clear();
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Field, err.GetErrorType(), IID_IID3Field, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Field, "An unexpected error has occurred", IID_IID3Field, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Field::CopyDataToFile(BSTR FileName)
{
	USES_CONVERSION;
	try
	{
		m_Field->ToFile(OLE2A(FileName));
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Field, err.GetErrorType(), IID_IID3Field, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Field, "An unexpected error has occurred", IID_IID3Field, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Field::CopyDataFromFile(BSTR FileName)
{
	USES_CONVERSION;
	try
	{
		m_Field->FromFile(OLE2A(FileName));
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Field, err.GetErrorType(), IID_IID3Field, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Field, "An unexpected error has occurred", IID_IID3Field, E_UNEXPECTED);
	}
}

STDMETHODIMP CID3Field::get_NumTextItems(long *pVal)
{
	try
	{
		*pVal = m_Field->GetNumTextItems();
		return S_OK;
	}
	catch (ID3_Error err)
	{
		return AtlReportError(CLSID_ID3Field, err.GetErrorType(), IID_IID3Field, CUSTOM_CTL_SCODE(1000 + err.GetErrorID()));
	}
	catch (...)
	{
		return AtlReportError(CLSID_ID3Field, "An unexpected error has occurred", IID_IID3Field, E_UNEXPECTED);
	}
}
