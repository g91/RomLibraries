// XMLManager.h: interface for the CXMLManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLMANAGER_H__E7EC5F0E_F1E2_482F_955F_8BFE84484871__INCLUDED_)
#define AFX_XMLMANAGER_H__E7EC5F0E_F1E2_482F_955F_8BFE84484871__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#import "msxml4.dll" named_guids raw_interfaces_only
#define CHECKHR(x) {hr = x; if (FAILED(hr)) goto CleanUp;}
#define SAFERELEASE(p) {if (p) {(p)->Release(); p = NULL;}}
class CXMLManager  
{
public:
	static ::IXMLDOMDocument * CreateXmlDocument( void );
	CString GetStringValue(::IXMLDOMNode *pNode);
	::IXMLDOMNode* GetChild(::IXMLDOMNode* pNode, CString name);
	CString GetNodeName(::IXMLDOMNode* pNode);
	double GetDoubleAttribute(::IXMLDOMNode* pNode, CString attName);
	long GetIntegerAttribute(::IXMLDOMNode* pNode, CString attName);
	CString GetStringAttribute(::IXMLDOMNode* pNode, CString attName);
	HRESULT LoadDocument(::IXMLDOMDocument *pDoc, CString fileName);
	void AddWhiteSpaceToNode(::IXMLDOMDocument* pDom,::IXMLDOMNode* xmlNode,CString bstrWs)  ;
	HRESULT SetStringAttribute(::IXMLDOMNode* pNode, CString attName, CString str);
	HRESULT SetDoubleAttribute(::IXMLDOMNode* pNode, CString attName, double d);
	HRESULT SetIntegerAttribute(::IXMLDOMNode* pNode, CString attName, long i);
	::IXMLDOMNode * CreateDOMNode(::IXMLDOMDocument* pDoc, int type, CString nodeName,CString desc);
	HRESULT SaveDocument(::IXMLDOMDocument *pDoc, CString fileName);
	CXMLManager();
	virtual ~CXMLManager();

};

#endif // !defined(AFX_XMLMANAGER_H__E7EC5F0E_F1E2_482F_955F_8BFE84484871__INCLUDED_)
