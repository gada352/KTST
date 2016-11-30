#pragma once

#include "CollectionDevice.h"


#define REQUEST_INTERVAL			60			//请求数据时间间隔（秒）
#define RECEIVE_TIME_OUT			60			//接收数据超时（秒）

class RequestData
{
public:
	RequestData(void);
	~RequestData(void);

public:
	void SetRequestDevice (CollectionDevice *pDevice);
	CollectionDevice* GetRequestDevice ();

	int GetRequestDeviceNo ();

public:
	BOOL Request ();
	void OnReceive (int nChannelCount, byte *pChBuf);

public:
	void OnTimer ();

public:
	BOOL m_bReceiveData;					//是否刷新过数据
	COleDateTime m_odtLastRequestTime;		//每隔120秒请求一次
	COleDateTime m_odtLastReceiveTime;		//记录上次接收数据的时间，为了和当前时间比较，大于60s就将通道状态设置为故障。

private:
	CollectionDevice *m_pDevice;			//请求对应的采集设备的通道数据

};
typedef CTypedPtrList<CPtrList, RequestData*> CRequestDataList;
