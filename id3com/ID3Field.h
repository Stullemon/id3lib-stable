// ID3Field.h : Declaration of the CID3Field

#ifndef __ID3FIELD_H_
#define __ID3FIELD_H_

#include "resource.h"       // main symbols
#include <id3/tag.h>
#include <id3.h>

/////////////////////////////////////////////////////////////////////////////
// CID3Field
class ATL_NO_VTABLE CID3Field : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CID3Field, &CLSID_ID3Field>,
	public ISupportErrorInfo,
	public IDispatchImpl<IID3Field, &IID_IID3Field, &LIBID_ID3COMLib>
{
public:
	CID3Field();
	~CID3Field();

	static IID3Field* CreateObject(IID3Frame* FrameParent, ID3_Field* Field);

DECLARE_REGISTRY_RESOURCEID(IDR_ID3FIELD)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CID3Field)
	COM_INTERFACE_ENTRY(IID3Field)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IID3Field
public:
	STDMETHOD(get_NumTextItems)(/*[out, retval]*/ long *pVal);
	STDMETHOD(CopyDataFromFile)(BSTR FileName);
	STDMETHOD(CopyDataToFile)(BSTR FileName);
	STDMETHOD(Clear)();
	STDMETHOD(get_Long)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Long)(/*[in]*/ long newVal);
	STDMETHOD(get_Text)(/*[in]*/ long ItemNum, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Text)(/*[in]*/ long ItemNum, /*[in]*/ BSTR newVal);
protected:
	ID3_Field* m_Field;
	IID3Frame* m_FrameParent;
};

#endif //__ID3FIELD_H_
