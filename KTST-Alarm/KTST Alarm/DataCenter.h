#pragma once

#include "RequestData.h"
#include "Device.h"

class CDataCenter
{
public:
	static CDataCenter* GetInstance ();
	static void Release ();

public:
	BOOL Start ();
	BOOL Stop ();

	BOOL Request (int nDeviceNo, int nChannelNo);

public:
	void OnTimer ();						//外部框架定时器调用1s来一次
	LRESULT OnUdpReceiveMessage (WPARAM wParam, LPARAM lParam);

	void OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam);

public:
	RequestData* GetRequestData (int nDeviceNo);
	CollectionDevice* GetCollectionDevice (int nDeviceNo);

	void GetServerInfo (CString &strAppServerIP, UINT &nAppServerPortNo);

private:
	static VOID CALLBACK TimerProc (HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);

private:
	BOOL InitialServerInfo ();
	BOOL InitialAlarmOutputDevice ();

	BOOL IsCollectionDevice (int nDeviceType);


private:
	UINT_PTR m_nTimerID;				//定时器ID
	BOOL m_bStart;						//是否启动

	CRequestDataList m_listRequestData;				//请求数据链表
	CCollectionDeviceList m_listCollectionDevice;	//采集设备链表
	CDeviceList m_listAlarmOutputDevice;			//报警输出设备链表，存储短信和继电器设备
// 	CSMSDeviceList m_listSMSDevice;					//短信设备链表
// 	CRelayDeviceList m_listRelayDevice;				//继电器设备链表

private:
	CString m_strAppServerIP;			//Server的IP地址
	UINT m_nAppServerPortNo;			//Server的端口号

private:
	CDataCenter(void);
	~CDataCenter(void);

private:
	static CDataCenter *m_pDataCenter;

private:
	CDataCenter(const CDataCenter &);	//防止外部调用拷贝构造函数
	CDataCenter& operator = (const CDataCenter &);	//防止直接赋值

private:
	class AutoRelease
	{
	public:
		AutoRelease(){}
		~AutoRelease()
		{
			CDataCenter::Release();
		}
	};
	static AutoRelease ar;		//程序结束时会调用静态成员的析构函数
};
