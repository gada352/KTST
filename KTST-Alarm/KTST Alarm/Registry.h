#pragma once

#define REG_BUF_LEN							1024
#define REG_ROOT_KEY						_T("SoftWare\\Kingtop")
#define REG_APP_SUBKEY						_T("KTST-Alarm")
#define REG_APP_MAIN_PATH					_T("MainPath")
#define REG_APP_BACK_PATH					_T("BackPath")
#define REG_APP_DB_SVR_NAME					_T("DBSvrName")
#define REG_APP_DB_SVR_PORT					_T("DBSvrPort")
#define REG_APP_DB_SVR_USER_NAME			_T("DBSvrUserName")
#define REG_APP_DB_SVR_USER_PWD				_T("DBSvrUserPwd")
#define REG_APP_DB_NAME						_T("DBName")
#define REG_APP_USER_NAME					_T("UserName")
#define REG_AUTO_CONNECT_DB					_T("AutoConnectDB")
#define REG_SYS_AUTO_RUN_KEY				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")

#define REG_DEVICE_SUBKEY					_T("Device")
#define REG_GUID_SUBKEY						_T("Guid")
#define REG_CLASS_PARAM_SUBKEY				_T("ClassParam")
#define REG_CHL_DEF_SUBKEY					_T("ChannelSetting")
#define REG_TYPE_NUMBER_VALUE				_T("TypeNumber")
#define REG_TYPE_ID_VALUE					_T("TypeID")
#define REG_TYPE_ID_KEY_NAME				_T("Type%d")
#define REG_TYPE_NAME_VALUE					_T("TypeName")
#define REG_DLL_NAME_VALUE					_T("DllName")
#define REG_DEVICE_NUMBER_VALUE				_T("DeviceNumber")
#define REG_DEVICE_NO_VALUE					_T("DeviceNo")
#define REG_DEVICE_NO_KEY_NAME				_T("Device%d")
#define REG_DEVICE_NAME_VALUE				_T("DeviceName")
#define REG_TOTAL_CHANNEL_VALUE				_T("ChannelNumber")
#define REG_COMM_NAME_VALUE					_T("CommName")
#define REG_BAUD_RATE_VALUE					_T("BaudRate")
#define REG_DATA_BITS_VALUE					_T("DataBits")
#define REG_STOP_BITS_VALUE					_T("StopBits")
#define REG_PARITY_VALUE					_T("Parity")
#define REG_SLAVE_ID_VALUE					_T("SlaveID")
#define REG_CHX_VALUE						_T("Ch")
#define REG_DEVICE_PROPERTY_FLAG			_T("DevProperty")


#define REG_ERROR_SUCCESS					1
#define REG_ERROR_OPEN_ROOT_KEY_FAILURE		101
#define REG_ERROR_CREATE_KEY_FAILURE		102
#define REG_ERROR_OPEN_KEY_FAILURE			103
#define REG_ERROR_DELETE_KEY_FAILURE		104
#define REG_ERROR_CLOSE_KEY_FAILURE			105
#define REG_ERROR_ENUM_KEY_FAILURE			106
#define REG_ERROR_ENUM_VALUE_FAILURE		107
#define REG_ERROR_SET_VALUE_FAILURE			108
#define REG_ERROR_QUERY_VALUE_FAILURE		109
#define REG_ERROR_DELETE_VALUE_FAILURE		110


class CRegistry  
{
public:
	CRegistry();
	CRegistry(CString);
	virtual ~CRegistry();

public:
	BOOL RegReadDBSvrName(CString &strDBSvrName);
	BOOL RegWriteDBSvrName(CString strDBSvrName);

	BOOL RegReadDBSvrPort(CString &strDBSvrPort);
	BOOL RegWriteDBSvrPort(CString strDBSvrPort);

	BOOL RegReadDBSvrUserName(CString &strServerUserName);
	BOOL RegWriteDBSvrUserName(CString strServerUserName);

	BOOL RegReadDBSvrUserPwd(CString &strServerUserPwd);
	BOOL RegWriteDBSvrUserPwd(CString strServerUserPwd);

	BOOL RegReadDBName(CString &strDBName);
	BOOL RegWriteDBName(CString strDBName);

	BOOL RegReadUserName(CString &strUserName);
	BOOL RegWriteUserName(CString strUserName);

	BOOL RegReadMainPath(CString &strMainPath);
	BOOL RegWriteMainPath(CString strMainPath);

	BOOL RegReadAutoConnectDB(BOOL &bAutoConnectDB);
	BOOL RegWriteAutoConnectDB(BOOL bAutoConnectDB);

	BOOL RegReadAutoRunService(BOOL &bAutoRunService, CString &strRunPath);
	BOOL RegWriteAutoRunService(BOOL bAutoRunService, CString strRunPath);

public:
	int RegDeleteValue(DWORD hKey, LPCTSTR lpValueName);
	int RegQueryValue(DWORD hKey, LPCTSTR lpValueName, DWORD &dwType, LPBYTE lpValue, DWORD &cbValue);
	int RegSetValue(DWORD hKey, LPCTSTR lpValueName, DWORD dwType, LPBYTE lpValue, DWORD cbValue);
	int RegEnumValue(DWORD hKey, DWORD dwIndex, LPTSTR lpValueName, LPDWORD lpcbValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
	int RegEnumKey(DWORD hKey, DWORD dwIndex, LPTSTR lpName, DWORD cbName);
	int RegCloseKey(DWORD hKey);
	int RegOpenKey(DWORD hKey, LPCTSTR lpSubKey, DWORD &hSubKey);
	int RegDeleteKey(DWORD hKey, LPCTSTR lpSubKey);
	int RegCreateKey(DWORD hKey, LPCTSTR lpSubKey, DWORD &hSubKey);
	int OpenRootKey(DWORD &hKey);

private:
	CString m_sProseg;

};
