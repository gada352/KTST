#pragma once

#import <msxml3.dll>

#define PATH_DB_SERVER_IP				_T("/ROOT/DATABASE/IP")
#define PATH_DB_SERVER_PORT				_T("/ROOT/DATABASE/PORT")
#define PATH_DB_SERVER_USER_NAME		_T("/ROOT/DATABASE/USERNAME")
#define PATH_DB_SERVER_USER_PWD			_T("/ROOT/DATABASE/PASSWORD")
#define PATH_DB_NAME					_T("/ROOT/DATABASE/DBNAME")
#define PATH_LOGIN_USER_NAME			_T("/ROOT/USER/USERNAME")
#define PATH_AUTO_CONNECT_DB			_T("/ROOT/CFG/AUTOCONNECTDB")

//登录信息
typedef struct LoginInfo
{
	CString strDBSvrIP;				//数据库服务器IP地址
	CString strDBSvrPort;			//数据库服务器端口号
	CString strDBSvrUserName;		//数据库服务器用户名
	CString strDBSvrUserPwd;		//数据库服务器用户密码
	CString strDBName;				//连接的数据库名称
	CString strLoginUserName;		//软件登录名称
	BOOL bAutoConnectDB;			//是否自动连接数据库

	LoginInfo ()
	{
		strDBSvrIP.Empty();
		strDBSvrPort.Empty();
		strDBSvrUserName.Empty();
		strDBSvrUserPwd.Empty();
		strDBName.Empty();
		strLoginUserName.Empty();
		bAutoConnectDB = FALSE;
	}

	LoginInfo (const LoginInfo &li)
	{
		Copy (li);
	}

	void Copy (const LoginInfo &li)
	{
		strDBSvrIP = li.strDBSvrIP;
		strDBSvrPort = li.strDBSvrPort;
		strDBSvrUserName = li.strDBSvrUserName;
		strDBSvrUserPwd = li.strDBSvrUserPwd;
		strDBName = li.strDBName;
		strLoginUserName = li.strLoginUserName;
		bAutoConnectDB = li.bAutoConnectDB;
	}
}LoginInfo;


class CXMLProfile
{
public:
	CXMLProfile(const CString &strFilePath);
	~CXMLProfile(void);

public:
	void SetFilePath (const CString &strFilePath);
	CString GetFilePath ();

public:
	BOOL SetLoginInfo (const LoginInfo &li);
	BOOL GetLoginInfo (LoginInfo &li);

	BOOL SetDBServerIP (const CString &strDBSvrIP);
	BOOL GetDBServerIP (CString &strDBSvrIP);

	BOOL SetDBServerPort (const CString &strDBSvrPort);
	BOOL GetDBServerPort (CString &strDBSvrPort);

	BOOL SetDBServerUserName (const CString &strDBSvrUserName);
	BOOL GetDBServerUserName (CString &strDBSvrUserName);

	BOOL SetDBServerUserPwd (const CString &strDBSvrUserPwd);
	BOOL GetDBServerUserPwd (CString &strDBSvrUserPwd);

	BOOL SetDBName (const CString &strDBName);
	BOOL GetDBName (CString &strDBName);

	BOOL SetLoginUserName (const CString &strLoginUserName);
	BOOL GetLoginUserName (CString &strLoginUserName);

	BOOL SetAutoConnectDB (BOOL bAutoConnectDB);
	BOOL GetAutoConnectDB (BOOL &bAutoConnectDB);

private:
	BOOL SetText (const CString &strPath, const CString &strValue);
	BOOL GetText (const CString &strPath, CString &strValue);

	BOOL SetText (MSXML2::IXMLDOMElementPtr pRoot, const CString &strPath, const CString &strValue);
	BOOL GetText (MSXML2::IXMLDOMElementPtr pRoot, const CString &strPath, CString &strValue);


private:
	CString m_strFilePath;
};
