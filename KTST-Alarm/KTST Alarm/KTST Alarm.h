
// KTST Alarm.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CKTSTAlarmApp:
// 有关此类的实现，请参阅 KTST Alarm.cpp
//

class CKTSTAlarmApp : public CWinApp
{
public:
	CKTSTAlarmApp();
	~CKTSTAlarmApp();

public:
	BOOL ConnectDB ();

	static CDeviceIO* GetDeviceIOPtr ();
	static CADOConnection* GetADOConnectionPtr ();
	static CCommunicate* GetCommunicatePtr ();

	CString GetMainPath ();
	CString GetCFGFilePath ();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()

private:
	BOOL IsAlreadyExistProc (LPCTSTR lpAppName);

private:
	static CDeviceIO m_DeviceIO;				//设备管理对象
	static CADOConnection m_ADOConnection;		//数据库连接类对象
	static CCommunicate *m_pCommunicate;		//通讯类对象
};

extern CKTSTAlarmApp theApp;