#include "stdafx.h"
#include "Registry.h"


CRegistry::CRegistry()
{

}

CRegistry::CRegistry(CString sProseg)
{
	m_sProseg = sProseg;
}

CRegistry::~CRegistry()
{

}

int CRegistry::OpenRootKey(DWORD &hKey)
{
	hKey = NULL;

	HKEY hRoot = HKEY_LOCAL_MACHINE;
	CString strSubKey = REG_ROOT_KEY;

	// begin ************************************************************************
	//判断变量m_sProseg是否为空，来决定打开根键时是打开到"SoftWare\\Kingtop"下，还是打开到"SoftWare\\Kingtop\\KTST"下

	while (m_sProseg.GetLength()>0 && (m_sProseg.GetAt(0)==' ' || m_sProseg.GetAt(0)=='\\'))
		m_sProseg = m_sProseg.Right(m_sProseg.GetLength()-1);

	while (m_sProseg.GetLength()>0 && (m_sProseg.GetAt(m_sProseg.GetLength()-1)==' ' || m_sProseg.GetAt(m_sProseg.GetLength()-1)=='\\'))
		m_sProseg = m_sProseg.Left(m_sProseg.GetLength()-1);

	if (m_sProseg.GetLength()>0)
		strSubKey += _T("\\") + m_sProseg;
	// end **************************************************************************
	
	if (::RegCreateKey(hRoot, strSubKey, (PHKEY)&hKey) != ERROR_SUCCESS)
		return REG_ERROR_OPEN_ROOT_KEY_FAILURE;
	
	return REG_ERROR_SUCCESS;
}

int CRegistry::RegCreateKey(DWORD hKey, LPCTSTR lpSubKey, DWORD &hSubKey)
{
	hSubKey = NULL;

	if(::RegCreateKey((HKEY)hKey, lpSubKey, (PHKEY)&hSubKey) != ERROR_SUCCESS)
		return REG_ERROR_CREATE_KEY_FAILURE;

	return REG_ERROR_SUCCESS;
}

int CRegistry::RegOpenKey(DWORD hKey, LPCTSTR lpSubKey, DWORD &hSubKey)
{
	hSubKey = NULL;

	if(::RegOpenKey((HKEY)hKey, lpSubKey, (PHKEY)&hSubKey) != ERROR_SUCCESS)
		return REG_ERROR_OPEN_KEY_FAILURE;

	return REG_ERROR_SUCCESS;
}

int CRegistry::RegDeleteKey(DWORD hKey, LPCTSTR lpSubKey)
{
	if(::RegDeleteKey((HKEY)hKey, lpSubKey) != ERROR_SUCCESS)
		return REG_ERROR_DELETE_KEY_FAILURE;

	return REG_ERROR_SUCCESS;
}

int CRegistry::RegCloseKey(DWORD hKey)
{
	if(::RegCloseKey((HKEY)hKey) != ERROR_SUCCESS)
		return REG_ERROR_CLOSE_KEY_FAILURE;

	return REG_ERROR_SUCCESS;
}

int CRegistry::RegEnumKey(DWORD hKey, DWORD dwIndex, LPTSTR lpName, DWORD cbName)
{
	if(::RegEnumKey((HKEY)hKey, dwIndex, lpName, cbName) != ERROR_SUCCESS)
		return REG_ERROR_ENUM_KEY_FAILURE;

	return REG_ERROR_SUCCESS;
}

int CRegistry::RegEnumValue(DWORD hKey, DWORD dwIndex, LPTSTR lpValueName, LPDWORD lpcbValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	if(::RegEnumValue((HKEY)hKey, dwIndex, lpValueName, lpcbValueName, NULL, lpType, lpData, lpcbData) != ERROR_SUCCESS)
		return REG_ERROR_ENUM_VALUE_FAILURE;

	return REG_ERROR_SUCCESS;
}

int CRegistry::RegSetValue(DWORD hKey, LPCTSTR lpValueName, DWORD dwType, LPBYTE lpValue, DWORD cbValue)
{
	if(::RegSetValueEx((HKEY)hKey, lpValueName, NULL, dwType, lpValue, cbValue) != ERROR_SUCCESS)
		return REG_ERROR_SET_VALUE_FAILURE;

	return REG_ERROR_SUCCESS;
}

int CRegistry::RegQueryValue(DWORD hKey, LPCTSTR lpValueName, DWORD &dwType, LPBYTE lpValue, DWORD &cbValue)
{
	if(::RegQueryValueEx((HKEY)hKey, lpValueName, NULL, &dwType, lpValue, &cbValue) != ERROR_SUCCESS)
		return REG_ERROR_QUERY_VALUE_FAILURE;

	return REG_ERROR_SUCCESS;
}

int CRegistry::RegDeleteValue(DWORD hKey, LPCTSTR lpValueName)
{
	if(::RegDeleteValue((HKEY)hKey, lpValueName) != ERROR_SUCCESS)
		return REG_ERROR_DELETE_VALUE_FAILURE;

	return REG_ERROR_SUCCESS;
}

//读服务器名称
BOOL CRegistry::RegReadDBSvrName(CString &strDBSvrName)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	cbValue = REG_BUF_LEN;
	if(RegQueryValue(hAppKey, REG_APP_DB_SVR_NAME, dwType, value, cbValue) == REG_ERROR_SUCCESS)
		strDBSvrName = (TCHAR *)value;
	else
	{
		RegCloseKey(hAppKey);
		strDBSvrName.Empty();
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//写服务器名称
BOOL CRegistry::RegWriteDBSvrName(CString strDBSvrName)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_SZ;
	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	::memcpy (value, (LPCTSTR )strDBSvrName, strDBSvrName.GetLength()*sizeof(TCHAR));
	cbValue = strDBSvrName.GetLength()*sizeof(TCHAR);
	if(RegSetValue(hAppKey, REG_APP_DB_SVR_NAME, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hAppKey);
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//读服务器端口
BOOL CRegistry::RegReadDBSvrPort(CString &strDBSvrPort)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	cbValue = REG_BUF_LEN;
	if(RegQueryValue(hAppKey, REG_APP_DB_SVR_PORT, dwType, value, cbValue) == REG_ERROR_SUCCESS)
		strDBSvrPort = (TCHAR *)value;
	else
	{
		RegCloseKey(hAppKey);
		strDBSvrPort.Empty();
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//写服务器端口
BOOL CRegistry::RegWriteDBSvrPort(CString strDBSvrPort)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_SZ;
	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	::memcpy (value, (LPCTSTR )strDBSvrPort, strDBSvrPort.GetLength()*sizeof(TCHAR));
	cbValue = strDBSvrPort.GetLength()*sizeof(TCHAR);
	if(RegSetValue(hAppKey, REG_APP_DB_SVR_PORT, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hAppKey);
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//读服务器用户名称
BOOL CRegistry::RegReadDBSvrUserName(CString &strServerUserName)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	cbValue = REG_BUF_LEN;
	if(RegQueryValue(hAppKey, REG_APP_DB_SVR_USER_NAME, dwType, value, cbValue) == REG_ERROR_SUCCESS)
		strServerUserName = (TCHAR *)value;
	else
	{
		RegCloseKey(hAppKey);
		strServerUserName.Empty();
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//写服务器用户名称
BOOL CRegistry::RegWriteDBSvrUserName(CString strServerUserName)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_SZ;
	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	::memcpy (value, (LPCTSTR )strServerUserName, strServerUserName.GetLength()*sizeof(TCHAR));
	cbValue = strServerUserName.GetLength()*sizeof(TCHAR);
	if(RegSetValue(hAppKey, REG_APP_DB_SVR_USER_NAME, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hAppKey);
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//读服务器用户密码
BOOL CRegistry::RegReadDBSvrUserPwd(CString &strServerUserPwd)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	cbValue = REG_BUF_LEN;
	if(RegQueryValue(hAppKey, REG_APP_DB_SVR_USER_PWD, dwType, value, cbValue) == REG_ERROR_SUCCESS)
		strServerUserPwd = (TCHAR *)value;
	else
	{
		RegCloseKey(hAppKey);
		strServerUserPwd.Empty();
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//写服务器用户密码
BOOL CRegistry::RegWriteDBSvrUserPwd(CString strServerUserPwd)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_SZ;
	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	::memcpy (value, (LPCTSTR )strServerUserPwd, strServerUserPwd.GetLength()*sizeof(TCHAR));
	cbValue = strServerUserPwd.GetLength()*sizeof(TCHAR);
	if(RegSetValue(hAppKey, REG_APP_DB_SVR_USER_PWD, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hAppKey);
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//读数据库名称
BOOL CRegistry::RegReadDBName(CString &strDBName)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	cbValue = REG_BUF_LEN;
	if(RegQueryValue(hAppKey, REG_APP_DB_NAME, dwType, value, cbValue) == REG_ERROR_SUCCESS)
		strDBName = (TCHAR *)value;
	else
	{
		RegCloseKey(hAppKey);
		strDBName.Empty();
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//写数据库名称
BOOL CRegistry::RegWriteDBName(CString strDBName)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_SZ;
	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	::memcpy (value, (LPCTSTR )strDBName, strDBName.GetLength()*sizeof(TCHAR));
	cbValue = strDBName.GetLength()*sizeof(TCHAR);
	if(RegSetValue(hAppKey, REG_APP_DB_NAME, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hAppKey);
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//读用户名称
BOOL CRegistry::RegReadUserName(CString &strUserName)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	cbValue = REG_BUF_LEN;
	if(RegQueryValue(hAppKey, REG_APP_USER_NAME, dwType, value, cbValue) == REG_ERROR_SUCCESS)
		strUserName = (TCHAR *)value;
	else
	{
		RegCloseKey(hAppKey);
		strUserName.Empty();
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//写用户名称
BOOL CRegistry::RegWriteUserName(CString strUserName)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_SZ;
	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	::memcpy (value, (LPCTSTR )strUserName, strUserName.GetLength()*sizeof(TCHAR));
	cbValue = strUserName.GetLength()*sizeof(TCHAR);
	if(RegSetValue(hAppKey, REG_APP_USER_NAME, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hAppKey);
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//读主路径
BOOL CRegistry::RegReadMainPath(CString &strMainPath)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);
	
	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	cbValue = REG_BUF_LEN;
	if(RegQueryValue(hAppKey, REG_APP_MAIN_PATH, dwType, value, cbValue) == REG_ERROR_SUCCESS)
		strMainPath = (TCHAR *)value;
	else
	{
		RegCloseKey(hAppKey);
		strMainPath.Empty();
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//写主路径
BOOL CRegistry::RegWriteMainPath(CString strMainPath)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_SZ;
	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	::memcpy (value, (LPCTSTR )strMainPath, strMainPath.GetLength()*sizeof(TCHAR));
	cbValue = strMainPath.GetLength()*sizeof(TCHAR);
	if(RegSetValue(hAppKey, REG_APP_MAIN_PATH, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hAppKey);
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//读自动连接数据库标志
BOOL CRegistry::RegReadAutoConnectDB(BOOL &bAutoConnectDB)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	cbValue = REG_BUF_LEN;
	if(RegQueryValue(hAppKey, REG_AUTO_CONNECT_DB, dwType, value, cbValue) == REG_ERROR_SUCCESS)
		bAutoConnectDB = *(int *)value;
	else
	{
		RegCloseKey(hAppKey);
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//写自动连接数据库标志
BOOL CRegistry::RegWriteAutoConnectDB(BOOL bAutoConnectDB)
{
	DWORD hRoot = 0;
	if(OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_APP_SUBKEY, hAppKey) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hRoot);
		return FALSE;
	}
	RegCloseKey(hRoot);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_DWORD;
	*(int *)value = bAutoConnectDB;
	cbValue = sizeof(int);
	if(RegSetValue(hAppKey, REG_AUTO_CONNECT_DB, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		RegCloseKey(hAppKey);
		return FALSE;
	}

	RegCloseKey(hAppKey);
	return TRUE;
}

//读自动运行服务标志
BOOL CRegistry::RegReadAutoRunService(BOOL &bAutoRunService, CString &strRunPath)
{
	DWORD hRoot = (DWORD )HKEY_LOCAL_MACHINE;
	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_SYS_AUTO_RUN_KEY, hAppKey) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	cbValue = REG_BUF_LEN;
	if(RegQueryValue(hAppKey, REG_APP_SUBKEY, dwType, value, cbValue) == REG_ERROR_SUCCESS)
	{
		bAutoRunService = TRUE;
		strRunPath = (TCHAR *)value;
	}
	else
	{
		bAutoRunService = FALSE;
		strRunPath.Empty();
	}

	RegCloseKey(hAppKey);

	return TRUE;
}

//写自动运行服务标志
BOOL CRegistry::RegWriteAutoRunService(BOOL bAutoRunService, CString strRunPath)
{
	DWORD hRoot = (DWORD )HKEY_LOCAL_MACHINE;
	DWORD hAppKey = 0;
	if(RegCreateKey(hRoot, REG_SYS_AUTO_RUN_KEY, hAppKey) != REG_ERROR_SUCCESS)
		return FALSE;

	if (bAutoRunService)	//设置为自动启动
	{
		DWORD dwType = 0;
		BYTE value[REG_BUF_LEN];
		DWORD cbValue = REG_BUF_LEN;

		if(RegQueryValue(hAppKey, REG_APP_SUBKEY, dwType, value, cbValue) == REG_ERROR_SUCCESS)
		{
			if ((CString )value == strRunPath)
			{
				RegCloseKey(hAppKey);
				return TRUE;
			}
		}

		dwType = REG_SZ;
		memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
		::memcpy (value, (LPCTSTR )strRunPath, strRunPath.GetLength()*sizeof(TCHAR));
		cbValue = strRunPath.GetLength()*sizeof(TCHAR);
		if(RegSetValue(hAppKey, REG_APP_SUBKEY, dwType, value, cbValue) != REG_ERROR_SUCCESS)
		{
			RegCloseKey(hAppKey);
			return FALSE;
		}
	}
	else	//取消自动启动
	{
		if(RegDeleteValue (hAppKey, REG_APP_SUBKEY) != REG_ERROR_SUCCESS)
		{
			RegCloseKey(hAppKey);
			return FALSE;
		}
	}

	RegCloseKey(hAppKey);

	return TRUE;
}
