#pragma once

#include "CollectionDevice.h"


#define REQUEST_INTERVAL			60			//��������ʱ�������룩
#define RECEIVE_TIME_OUT			60			//�������ݳ�ʱ���룩

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
	BOOL m_bReceiveData;					//�Ƿ�ˢ�¹�����
	COleDateTime m_odtLastRequestTime;		//ÿ��120������һ��
	COleDateTime m_odtLastReceiveTime;		//��¼�ϴν������ݵ�ʱ�䣬Ϊ�˺͵�ǰʱ��Ƚϣ�����60s�ͽ�ͨ��״̬����Ϊ���ϡ�

private:
	CollectionDevice *m_pDevice;			//�����Ӧ�Ĳɼ��豸��ͨ������

};
typedef CTypedPtrList<CPtrList, RequestData*> CRequestDataList;
