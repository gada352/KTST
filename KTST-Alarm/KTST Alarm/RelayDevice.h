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
	BOOL m_bOutputting;			//��ʶ�Ƿ�����ִ�����

	BOOL m_bStarted;			//��ʶ�豸�Ƿ��Ѿ�����
};
typedef CTypedPtrList<CPtrList, RelayDevice*> CRelayDeviceList;

