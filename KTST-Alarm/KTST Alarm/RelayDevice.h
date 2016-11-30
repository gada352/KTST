#pragma once

#include "Device.h"

class RelayDevice:
	public Device
{
public:
	RelayDevice(void);
	virtual ~RelayDevice(void);

public:
	virtual BOOL InitializeChannelInfoFromDB ();

public:
	virtual BOOL Initialize ();
	virtual BOOL UnInitialize ();
	virtual BOOL Start ();
	virtual BOOL Stop ();
	virtual void OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer ();

public:
	BOOL Output ();

public:
	void ResetChannel (int nChannelNo);
	void ResetAllChannel ();

	void RequestAllFollowingChannelData ();

private:
	BOOL m_bOutputting;			//标识是否正在执行输出

	BOOL m_bStarted;			//标识设备是否已经启动
};
typedef CTypedPtrList<CPtrList, RelayDevice*> CRelayDeviceList;

