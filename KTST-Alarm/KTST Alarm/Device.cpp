#include "StdAfx.h"
#include "Device.h"


Device::Device(void)
{
	m_nDeviceNo = -1;
	m_strDeviceName.Empty();
	m_nDeviceType = 0;
	m_nDriverType = 0;
	m_nChannelCount = 0;
	m_strDeviceGUID.Empty();
	m_nSampleInterval = 60;
	m_bdDefBuf.Reset();

	m_nOptFlag = OPT_NULL;
}


Device::~Device(void)
{
	ClearMemory ();
}

BOOL Device::Initialize ()
{
	Channel *pChannel = NULL;
	POSITION pos = m_listChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = m_listChannel.GetNext(pos);
		if (pChannel)
		{
			if (!pChannel->Initialize ())
				return FALSE;
		}
	}

	return TRUE;
}

BOOL Device::UnInitialize ()
{
	Channel *pChannel = NULL;
	POSITION pos = m_listChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = m_listChannel.GetNext(pos);
		if (pChannel)
		{
			if (!pChannel->UnInitialize ())
				return FALSE;
		}
	}

	return TRUE;
}

void Device::OnTimer ()
{
	Channel *pChannel = NULL;
	POSITION pos = m_listChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = m_listChannel.GetNext(pos);
		if (pChannel)
			pChannel->OnTimer ();
	}
}

void Device::SetDeviceInfo (const DeviceInfo &di)
{
	m_nDeviceNo = di.nDeviceNo;
	m_strDeviceName = di.strDeviceName;
	m_nDeviceType = di.nDeviceType;
	m_nDriverType = di.nDriverType;
	m_nChannelCount = di.nChannelCount;
	m_strDeviceGUID = di.strDeviceGUID;
	m_nSampleInterval = di.nSampleInterval;
	m_bdDefBuf = di.bdDefBuf;

	m_nOptFlag = di.nOptFlag;
}

void Device::GetDeviceInfo (DeviceInfo &di)
{
	di.nDeviceNo = m_nDeviceNo;
	di.strDeviceName = m_strDeviceName;
	di.nDeviceType = m_nDeviceType;
	di.nDriverType = m_nDriverType;
	di.nChannelCount = m_nChannelCount;
	di.strDeviceGUID = m_strDeviceGUID;
	di.nSampleInterval = m_nSampleInterval;
	di.bdDefBuf = m_bdDefBuf;

	di.nOptFlag = m_nOptFlag;
}

//获得指定通道
Channel* Device::GetChannel (int nChannelNo)
{
	POSITION pos = m_listChannel.FindIndex(nChannelNo);
	if (pos == NULL)
		return NULL;

	return m_listChannel.GetAt(pos);
}

//清空内存
void Device::ClearMemory ()
{
	while (m_listChannel.GetCount())
		delete m_listChannel.RemoveHead();
}
