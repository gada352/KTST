#include "StdAfx.h"
#include "XMLProfile.h"


CXMLProfile::CXMLProfile(const CString &strFilePath)
{
	this->SetFilePath(strFilePath);
}

CXMLProfile::~CXMLProfile(void)
{
}

void CXMLProfile::SetFilePath (const CString &strFilePath)
{
	m_strFilePath = strFilePath;
}

CString CXMLProfile::GetFilePath ()
{
	return m_strFilePath;
}

BOOL CXMLProfile::SetLoginInfo (const LoginInfo &li)
{
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (FAILED(hr))
		return FALSE;

	VARIANT_BOOL vb = pDoc->load((_variant_t )this->GetFilePath ());
	if (vb == VARIANT_FALSE)
		return FALSE;

	MSXML2::IXMLDOMElementPtr pRoot;
	hr = pDoc->get_documentElement(&pRoot);
	if (FAILED(hr))
		return FALSE;

	if (!this->SetText (pRoot, PATH_DB_SERVER_IP, li.strDBSvrIP))
		return FALSE;

	if (!this->SetText (pRoot, PATH_DB_SERVER_PORT, li.strDBSvrPort))
		return FALSE;

	if (!this->SetText (pRoot, PATH_DB_SERVER_USER_NAME, li.strDBSvrUserName))
		return FALSE;

	if (!this->SetText (pRoot, PATH_DB_SERVER_USER_PWD, li.strDBSvrUserPwd))
		return FALSE;

	if (!this->SetText (pRoot, PATH_DB_NAME, li.strDBName))
		return FALSE;

	if (!this->SetText (pRoot, PATH_LOGIN_USER_NAME, li.strLoginUserName))
		return FALSE;

	CString strValue = li.bAutoConnectDB ? _T("1"):_T("0");
	if (!this->SetText (pRoot, PATH_AUTO_CONNECT_DB, strValue))
		return FALSE;

	hr = pDoc->save((_variant_t )this->GetFilePath ());
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL CXMLProfile::GetLoginInfo (LoginInfo &li)
{
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (FAILED(hr))
		return FALSE;

	VARIANT_BOOL vb = pDoc->load((_variant_t )this->GetFilePath ());
	if (vb == VARIANT_FALSE)
		return FALSE;

	MSXML2::IXMLDOMElementPtr pRoot;
	hr = pDoc->get_documentElement(&pRoot);
	if (FAILED(hr))
		return FALSE;

	LoginInfo liTemp;

	if (!this->GetText (pRoot, PATH_DB_SERVER_IP, liTemp.strDBSvrIP))
		return FALSE;

	if (!this->GetText (pRoot, PATH_DB_SERVER_PORT, liTemp.strDBSvrPort))
		return FALSE;

	if (!this->GetText (pRoot, PATH_DB_SERVER_USER_NAME, liTemp.strDBSvrUserName))
		return FALSE;

	if (!this->GetText (pRoot, PATH_DB_SERVER_USER_PWD, liTemp.strDBSvrUserPwd))
		return FALSE;

	if (!this->GetText (pRoot, PATH_DB_NAME, liTemp.strDBName))
		return FALSE;

	if (!this->GetText (pRoot, PATH_LOGIN_USER_NAME, liTemp.strLoginUserName))
		return FALSE;

	CString strValue;
	if (!this->GetText (pRoot, PATH_AUTO_CONNECT_DB, strValue))
		return FALSE;
	if (strValue == _T("1"))
		liTemp.bAutoConnectDB = TRUE;
	else
		liTemp.bAutoConnectDB = FALSE;

	li.Copy(liTemp);

	return TRUE;
}

BOOL CXMLProfile::SetDBServerIP (const CString &strDBSvrIP)
{
	return SetText (PATH_DB_SERVER_IP, strDBSvrIP);
}

BOOL CXMLProfile::GetDBServerIP (CString &strDBSvrIP)
{
	return GetText (PATH_DB_SERVER_IP, strDBSvrIP);
}

BOOL CXMLProfile::SetDBServerPort (const CString &strDBSvrPort)
{
	return SetText (PATH_DB_SERVER_PORT, strDBSvrPort);
}

BOOL CXMLProfile::GetDBServerPort (CString &strDBSvrPort)
{
	return GetText (PATH_DB_SERVER_PORT, strDBSvrPort);
}

BOOL CXMLProfile::SetDBServerUserName (const CString &strDBSvrUserName)
{
	return SetText (PATH_DB_SERVER_USER_NAME, strDBSvrUserName);
}

BOOL CXMLProfile::GetDBServerUserName (CString &strDBSvrUserName)
{
	return GetText (PATH_DB_SERVER_USER_NAME, strDBSvrUserName);
}

BOOL CXMLProfile::SetDBServerUserPwd (const CString &strDBSvrUserPwd)
{
	return SetText (PATH_DB_SERVER_USER_PWD, strDBSvrUserPwd);
}

BOOL CXMLProfile::GetDBServerUserPwd (CString &strDBSvrUserPwd)
{
	return GetText (PATH_DB_SERVER_USER_PWD, strDBSvrUserPwd);
}

BOOL CXMLProfile::SetDBName (const CString &strDBName)
{
	return SetText (PATH_DB_NAME, strDBName);
}

BOOL CXMLProfile::GetDBName (CString &strDBName)
{
	return GetText (PATH_DB_NAME, strDBName);
}

BOOL CXMLProfile::SetLoginUserName (const CString &strLoginUserName)
{
	return SetText (PATH_LOGIN_USER_NAME, strLoginUserName);
}

BOOL CXMLProfile::GetLoginUserName (CString &strLoginUserName)
{
	return GetText (PATH_LOGIN_USER_NAME, strLoginUserName);
}

BOOL CXMLProfile::SetAutoConnectDB (BOOL bAutoConnectDB)
{
	CString strValue = bAutoConnectDB ? _T("1"):_T("0");

	return SetText (PATH_AUTO_CONNECT_DB, strValue);
}

BOOL CXMLProfile::GetAutoConnectDB (BOOL &bAutoConnectDB)
{
	CString strValue;
	if (!GetText (PATH_AUTO_CONNECT_DB, strValue))
		return FALSE;

	if (strValue == _T("1"))
		bAutoConnectDB = TRUE;
	else
		bAutoConnectDB = FALSE;

	return TRUE;
}

BOOL CXMLProfile::SetText (const CString &strPath, const CString &strValue)
{
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (FAILED(hr))
		return FALSE;

	VARIANT_BOOL vb = pDoc->load((_variant_t )this->GetFilePath ());
	if (vb == VARIANT_FALSE)
		return FALSE;

	MSXML2::IXMLDOMElementPtr pRoot;
	hr = pDoc->get_documentElement(&pRoot);
	if (FAILED(hr))
		return FALSE;

	if (!this->SetText (pRoot, strPath, strValue))
		return FALSE;

	hr = pDoc->save((_variant_t )this->GetFilePath ());
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL CXMLProfile::GetText (const CString &strPath, CString &strValue)
{
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (FAILED(hr))
		return FALSE;

	VARIANT_BOOL vb = pDoc->load((_variant_t )this->GetFilePath ());
	if (vb == VARIANT_FALSE)
		return FALSE;

	MSXML2::IXMLDOMElementPtr pRoot;
	hr = pDoc->get_documentElement(&pRoot);
	if (FAILED(hr))
		return FALSE;

	if (!this->GetText (pRoot, strPath, strValue))
		return FALSE;

	return TRUE;
}

BOOL CXMLProfile::SetText (MSXML2::IXMLDOMElementPtr pRoot, const CString &strPath, const CString &strValue)
{
	if (pRoot == NULL)
		return FALSE;

	MSXML2::IXMLDOMNodePtr pNode;
	pNode = pRoot->selectSingleNode ((_bstr_t)strPath);
	if (pNode == NULL)
		return FALSE;

//	_variant_t vt = pNode->PutnodeValue ();
	pNode->Puttext ((_bstr_t )strValue);

	return TRUE;
}

BOOL CXMLProfile::GetText (MSXML2::IXMLDOMElementPtr pRoot, const CString &strPath, CString &strValue)
{
	if (pRoot == NULL)
		return FALSE;

	MSXML2::IXMLDOMNodePtr pNode;
	pNode = pRoot->selectSingleNode ((_bstr_t)strPath);
	if (pNode == NULL)
		return FALSE;

	strValue = (LPCSTR )pNode->Gettext ();

	return TRUE;
}
