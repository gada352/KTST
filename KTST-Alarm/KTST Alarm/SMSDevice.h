#pragma once

#include "Device.h"
#include "RequestData.h"


#define MAX_SMS_TEXT_LEN		128			//单次向短信动态库传短信内容的最大长度

//缓冲区输出标志
enum BufferFlag
{
	bfNull = -1,							//空
	bfSMS = 0,								//发送短信给所有用户
	bfDialUp = 1,							//拨号提醒给所有用户
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
	BOOL m_bDialUp;			//标识发送短信后是否拨号提醒
	CFollowingChannelList m_listFollowingChannel;	//短信设备关注的通道

private:
	BOOL m_bStarted;			//标识设备是否已经启动
};
typedef CTypedPtrList<CPtrList, SMSDevice*> CSMSDeviceList;

