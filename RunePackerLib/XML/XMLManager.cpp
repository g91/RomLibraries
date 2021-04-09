// XMLManager.cpp: implementation of the CXMLManager class.
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include <atlbase.h> // Com Variant support

#include "XMLManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////
// Helper Function: Convert ascii string to BSTR
////////////////////////////////////////////////////////////////////////////
BSTR AsciiToBSTR(const char* pszFName)
{
    WCHAR wszURL[MAX_PATH];
    ::MultiByteToWideChar(CP_ACP, 0, pszFName, -1, wszURL, MAX_PATH);
    return SysAllocString(wszURL);
}
////////////////////////////////////////////////////////////////////////////
// Helper Function: Wait for async download to complete
////////////////////////////////////////////////////////////////////////////
HRESULT WaitForCompletion(::IXMLDOMDocument *pDoc)
{
    long            ready;
    MSG             msg;
    HRESULT         hr;

    CHECKHR(pDoc->get_readyState(&ready));
    while (ready != READYSTATE_COMPLETE)
    {
        GetMessage(&msg, NULL, 0, 0);
        DispatchMessage(&msg);
        CHECKHR(pDoc->get_readyState(&ready));
    }

    while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        GetMessage(&msg, NULL, 0, 0);
        DispatchMessage(&msg);
    }

CleanUp:    
    return S_OK;
}
////////////////////////////////////////////////////////////////////////////
// Helper function: Report parsing error information
////////////////////////////////////////////////////////////////////////////
HRESULT ReportError(::IXMLDOMParseError *pXMLError)
{
    long line, linePos;
    LONG errorCode;
    BSTR pBURL, pBReason;
    HRESULT hr;

    CHECKHR(pXMLError->get_line(&line));
    CHECKHR(pXMLError->get_linepos(&linePos));
    CHECKHR(pXMLError->get_errorCode(&errorCode));
    CHECKHR(pXMLError->get_url(&pBURL));
    CHECKHR(pXMLError->get_reason(&pBReason));

    DPF(0,_T("%S"), pBReason);
    if (line > 0)
    {
        DPF(0,_T("Error on line %d, position %d in \"%S\".\n"), 
            line, linePos, pBURL);
    }

CleanUp:

    SysFreeString(pBURL);
    SysFreeString(pBReason);

    return E_FAIL;
}
////////////////////////////////////////////////////////////////////////////
// Helper function: Check load results
////////////////////////////////////////////////////////////////////////////
HRESULT CheckLoad(::IXMLDOMDocument* pDoc)
{
    // And since we don't have the VARIANT_BOOL from load we
    // need to check the parse Error errorCode property to see
    // if everything went ok.
    ::IXMLDOMParseError  *pXMLError = NULL;
    LONG errorCode = E_FAIL;
    HRESULT hr;

    CHECKHR(pDoc->get_parseError(&pXMLError));
    CHECKHR(pXMLError->get_errorCode(&errorCode));
    if (errorCode != 0)
    {
        hr = ReportError(pXMLError);
        goto CleanUp;
    }
    else
    {
        _ftprintf(stderr, _T("XML document loaded successfully\n"));
    }

	
CleanUp:
    SAFERELEASE(pXMLError);
    return errorCode;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLManager::CXMLManager()
{

}

CXMLManager::~CXMLManager()
{

}

HRESULT CXMLManager::SaveDocument(::IXMLDOMDocument *pDoc, CString fileName)
{
    HRESULT hr = S_OK;
	BSTR pBFName = fileName.AllocSysString();
    CComVariant v(pBFName);
    CHECKHR(pDoc->save(v));

	::SysFreeString(pBFName);
CleanUp:
    return hr;
}

void     CXMLManager::  AddWhiteSpaceToNode(::IXMLDOMDocument*       pDom,::IXMLDOMNode*       xmlNode,CString       bstrWs)   
{   
	::IXMLDOMText       *pws=NULL;   
	::IXMLDOMNode       *pBuf=NULL;   

	BSTR bstr=bstrWs.AllocSysString();
	pDom->createTextNode(bstr,&pws);   
	xmlNode->appendChild(pws,&pBuf);   

	if       (pws)       pws->Release();   
	pws=NULL;   
	if       (pBuf)       pBuf->Release();   
	pBuf=NULL;   
	::SysFreeString(bstr);
}   



::IXMLDOMNode * CXMLManager::CreateDOMNode(::IXMLDOMDocument* pDoc, int type, CString nodeName,CString nodeURI)
{
	::IXMLDOMNode * node;
	BSTR bstrName = nodeName.AllocSysString();
	BSTR bstrURI = nodeURI.AllocSysString();
	VARIANT vtype;

    vtype.vt = VT_I4;
    V_I4(&vtype) = (int)type;

    pDoc->createNode(vtype, bstrName, bstrURI, &node);

	::SysFreeString(bstrName);
	::SysFreeString(bstrURI);
	return node;
}

HRESULT CXMLManager::SetIntegerAttribute(::IXMLDOMNode* pNode, CString attName, long i)
{
	::IXMLDOMElement* pEle = NULL;
    CComVariant v(i);
    HRESULT hr;
	BSTR bstrName = attName.AllocSysString();

    CHECKHR(pNode->QueryInterface(::IID_IXMLDOMElement, (void**)&pEle));
    pEle->setAttribute(bstrName, v);

	::SysFreeString(bstrName);
//	::SysFreeString(URI);

CleanUp:
    SAFERELEASE(pEle);
    return hr;

}

HRESULT CXMLManager::SetDoubleAttribute(::IXMLDOMNode* pNode, CString attName, double d)
{
	::IXMLDOMElement* pEle = NULL;
    CComVariant v(d);
    HRESULT hr;
	BSTR bstrName = attName.AllocSysString();

    CHECKHR(pNode->QueryInterface(::IID_IXMLDOMElement, (void**)&pEle));
    pEle->setAttribute(bstrName, v);

	::SysFreeString(bstrName);
	//::SysFreeString(URI);

CleanUp:
    SAFERELEASE(pEle);
    return hr;


}

HRESULT CXMLManager::SetStringAttribute(::IXMLDOMNode* pNode, CString attName, CString str)
{
    ::IXMLDOMElement* pEle = NULL;
	CComVariant v(str);
    HRESULT hr;
	BSTR bstrName = attName.AllocSysString();
	int aa = str.GetLength();
    CHECKHR(pNode->QueryInterface(::IID_IXMLDOMElement, (void**)&pEle));
    pEle->setAttribute(bstrName, v);

	::SysFreeString(bstrName);
	//::SysFreeString(URI);

	CleanUp:
    SAFERELEASE(pEle);
    return hr;
}

HRESULT CXMLManager::LoadDocument(::IXMLDOMDocument *pDoc, CString fileName)
{
	BSTR pBURL = fileName.AllocSysString();
    VARIANT_BOOL    vb;
    HRESULT         hr;
    VARIANT         vURL;

    // set asynchronous loading flag
    CHECKHR(pDoc->put_async(VARIANT_TRUE));

    // Load xml document from the given URL
    vURL.vt = VT_BSTR;
    V_BSTR(&vURL) = pBURL;
    pDoc->load(vURL, &vb);

    // Now wait for download to complete !
    WaitForCompletion(pDoc);

    CHECKHR(CheckLoad(pDoc));
	::SysFreeString(pBURL);
	//::SysFreeString(URI);
                    
CleanUp:

    return hr;
}

CString CXMLManager::GetStringAttribute(::IXMLDOMNode* pNode, CString attName)
{
	HRESULT hr = S_OK;
	::IXMLDOMElement* pEle = NULL;
	CString str;
	BSTR atrName = attName.AllocSysString();
	CComVariant v;
	CHECKHR(pNode->QueryInterface(::IID_IXMLDOMElement, (void**)&pEle));
	pEle->getAttribute(atrName,&v);
	str = v.bstrVal;	
::SysFreeString(atrName);

CleanUp:
    SAFERELEASE(pEle);
	return str;
}

long CXMLManager::GetIntegerAttribute(::IXMLDOMNode* pNode, CString attName)
{
	HRESULT hr = S_OK;
	::IXMLDOMElement* pEle = NULL;
	long val;
	CString str;
	BSTR atrName = attName.AllocSysString();
	CComVariant v;
	CHECKHR(pNode->QueryInterface(::IID_IXMLDOMElement, (void**)&pEle));
	pEle->getAttribute(atrName,&v);
	str = v.bstrVal;
	val = _ttol(str);

	::SysFreeString(atrName);
CleanUp:
	SAFERELEASE(pEle);
	return val;
}

double CXMLManager::GetDoubleAttribute(::IXMLDOMNode* pNode, CString attName)
{
	HRESULT hr = S_OK;
	::IXMLDOMElement* pEle = NULL;
	double val;
	CString str;
	BSTR atrName = attName.AllocSysString();
	CComVariant v;
	CHECKHR(pNode->QueryInterface(::IID_IXMLDOMElement, (void**)&pEle));

	pEle->getAttribute(atrName,&v);
	str = v.bstrVal;
	val = _tstof(str);

	::SysFreeString(atrName);
CleanUp:
	SAFERELEASE(pEle);
	return val;
}

CString CXMLManager::GetNodeName(::IXMLDOMNode *pNode)
{
	BSTR name;
	pNode->get_nodeName(&name);
	CString str(name);
	::SysFreeString(name);
	
	return name;
}

::IXMLDOMNode* CXMLManager::GetChild(::IXMLDOMNode *pNode, CString name)
{
	::IXMLDOMNodeList *pChildren = NULL;
	::IXMLDOMNode *pNext = NULL;
	BSTR tagName;
	pNode->get_childNodes(&pChildren);

	pChildren->nextNode(&pNext);
	while (pNext)
	{
		pNext->get_baseName(&tagName);
		CString str(tagName);
		::SysFreeString(tagName);
		if (str == name) 
			return pNext;
		SAFERELEASE(pNext);
		pChildren->nextNode(&pNext);
	}


	
	SAFERELEASE(pNext);
	SAFERELEASE(pChildren);
	return NULL;
}

CString CXMLManager::GetStringValue(::IXMLDOMNode *pNode)
{
//	VARIANT v;
	CString str;
//	v.vt = VT_BSTR;
	CComVariant v;
	pNode->get_nodeValue(&v);
	str = v.bstrVal;
	return str;
}

::IXMLDOMDocument * CXMLManager::CreateXmlDocument( )
{
	HRESULT hr = S_OK;
	::IXMLDOMDocument *pDoc  = NULL;
	//_variant_t vt = 0; 
	// Create an empty XML document
	//int x;
	CoInitialize(NULL);

	CHECKHR(CoCreateInstance(
		::CLSID_DOMDocument, 
		NULL, 
		CLSCTX_INPROC_SERVER,
		::IID_IXMLDOMDocument, 
		(void**)&pDoc));

	::IXMLDOMProcessingInstruction* pInstruction = NULL;

	::IXMLDOMNode* pNewNode = NULL;

	BSTR target=CString(_T("xml")).AllocSysString();
	BSTR data=CString(_T("version='1.0' encoding='UTF-8'")).AllocSysString();
//	HRESULT hr;
	hr = pDoc->createProcessingInstruction( target, 
		data, &pInstruction); 
	// VARIANT_BOOL    vb;
	pDoc->put_preserveWhiteSpace(VARIANT_TRUE);
	pDoc->appendChild(pInstruction,NULL);



	//hr =pDoc->insertBefore(pInstruction, vt, &pNewNode);

	//hr =pDoc->appendChild(pNewNode,NULL);
	SAFERELEASE(pInstruction);

	SAFERELEASE(pNewNode);//->Release();

::SysFreeString(target);
::SysFreeString(data);



	//XmlDocument* doc = new XmlDocument();
	//XmlDeclaration* decl = doc->CreateXmlDeclaration( "1.0" , "utf-8" ,"" );
	//doc->InsertBefore( decl , doc->FirstChild );
CleanUp:
	return pDoc;
}