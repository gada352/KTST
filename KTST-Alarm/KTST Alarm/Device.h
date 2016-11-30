#pragma once

#include "Channel.h"

class Device
{
public:
	Device(void);
	virtual ~Device(void);

public:
	virtual BOOL InitializeDeviceInfoFromDB (){return TRUE;}
	virtual BOOL InitializeChannelInfoFromDB (){return TRUE;}

public:
	virtual BOOL Initialize ();
	virtual BOOL UnInitialize ();
	virtual BOOL Start (){return TRUE;}
	virtual BOOL Stop (){return TRUE;}
	virtual void OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam){}
	virtual void OnTimer ();

public:
	virtual void SetDeviceInfo (const DeviceInfo &di);
	virtual void GetDeviceInfo (DeviceInfo &di);

public:
	//设备信息
	int m_nDeviceNo;
	CString m_strDeviceName;
	int m_nDeviceType;
	int m_nDriverType;
	int m_nChannelCount;
	CString m_strDeviceGUID;
	int m_nSampleInterval;
	BlobData<byte> m_bdDefBuf;

	int m_nOptFlag;

public:
	Channel* GetChannel (int nChannelNo);

public:
	void ClearMemory ();

protected:
	CChannelList m_listChannel;
};
typedef CTypedPtrList<CPtrList, Device*> CDeviceList;

