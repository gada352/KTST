#pragma once

#include "Device.h"
#include "RequestData.h"


#define MAX_SMS_TEXT_LEN		128			//��������Ŷ�̬�⴫�������ݵ���󳤶�

//�����������־
enum BufferFlag
{
	bfNull = -1,							//��
	bfSMS = 0,								//���Ͷ��Ÿ������û�
	bfDialUp = 1,							//�������Ѹ������û�
};

class SMSDevice:
	public Device
{
public:
	SMSDevice(void);
	virtual ~SMSDevice(void);

public:
	virtual BOOL Initialize ();
	virtual BOOL UnInitialize ();
	virtual BOOL Start ();
	virtual BOOL Stop ();
	virtual void OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam);

public:
	BOOL SendSMS (CString strText);

public:
	BOOL AnalyzeSMSDeviceBuffer ();
	BOOL SaveSMSDeviceBuffer ();

public:
	void ClearFollowingChannelList ();
	void AddFollowingChannel (int nDeviceNo, int nChannelNo);
	BOOL IsFollowingChannel (int nDeviceNo, int nChannelNo);

	void RequestAllFollowingChannelData ();

private:
	void ClearMemory ();

public:
	BOOL m_bDialUp;			//��ʶ���Ͷ��ź��Ƿ񲦺�����
	CFollowingChannelList m_listFollowingChannel;	//�����豸��ע��ͨ��

private:
	BOOL m_bStarted;			//��ʶ�豸�Ƿ��Ѿ�����
};
typedef CTypedPtrList<CPtrList, SMSDevice*> CSMSDeviceList;

