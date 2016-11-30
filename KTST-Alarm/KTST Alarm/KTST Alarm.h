
// KTST Alarm.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CKTSTAlarmApp:
// �йش����ʵ�֣������ KTST Alarm.cpp
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

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()

private:
	BOOL IsAlreadyExistProc (LPCTSTR lpAppName);

private:
	static CDeviceIO m_DeviceIO;				//�豸�������
	static CADOConnection m_ADOConnection;		//���ݿ����������
	static CCommunicate *m_pCommunicate;		//ͨѶ�����
};

extern CKTSTAlarmApp theApp;