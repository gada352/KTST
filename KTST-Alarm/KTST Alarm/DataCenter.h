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
	void OnTimer ();						//�ⲿ��ܶ�ʱ������1s��һ��
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
	UINT_PTR m_nTimerID;				//��ʱ��ID
	BOOL m_bStart;						//�Ƿ�����

	CRequestDataList m_listRequestData;				//������������
	CCollectionDeviceList m_listCollectionDevice;	//�ɼ��豸����
	CDeviceList m_listAlarmOutputDevice;			//��������豸�����洢���źͼ̵����豸
// 	CSMSDeviceList m_listSMSDevice;					//�����豸����
// 	CRelayDeviceList m_listRelayDevice;				//�̵����豸����

private:
	CString m_strAppServerIP;			//Server��IP��ַ
	UINT m_nAppServerPortNo;			//Server�Ķ˿ں�

private:
	CDataCenter(void);
	~CDataCenter(void);

private:
	static CDataCenter *m_pDataCenter;

private:
	CDataCenter(const CDataCenter &);	//��ֹ�ⲿ���ÿ������캯��
	CDataCenter& operator = (const CDataCenter &);	//��ֱֹ�Ӹ�ֵ

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
	static AutoRelease ar;		//�������ʱ����þ�̬��Ա����������
};
