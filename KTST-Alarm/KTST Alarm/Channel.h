#pragma once


class Channel
{
public:
	Channel(void);
	virtual ~Channel(void);

public:
	virtual BOOL Initialize (){return TRUE;}
	virtual BOOL UnInitialize (){return TRUE;}
	virtual void OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam){}
	virtual void OnTimer (){}

public:
	virtual void SetChannelInfo (const ChannelInfo &ci);
	virtual void GetChannelInfo (ChannelInfo &ci);

public:
	//通道信息
	int m_nDeviceNo;
	int m_nChannelNo;
	CString m_strChannelName;
	BOOL m_bWorkEnable;
	float m_fFullScaleHigh;
	float m_fFullScaleLow;
	float m_fCalibrationHigh;
	float m_fCalibrationLow;
	byte m_byDataType;
	byte m_byDecimalDigits;
	CString m_strUnitString;
	BlobData<byte> m_bdAlarmLevelData;
	BlobData<byte> m_bdAlarmRangeData;
	BlobData<byte> m_bdDefBuf;

	int m_nOptFlag;
};
typedef CTypedPtrList<CPtrList, Channel*> CChannelList;

