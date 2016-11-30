#pragma once

#include "Device.h"

class CollectionDevice:
	public Device
{
public:
	CollectionDevice(void);
	CollectionDevice(int nDeviceNo);
	virtual ~CollectionDevice(void);

public:
	virtual BOOL InitializeDeviceInfoFromDB ();
	virtual BOOL InitializeChannelInfoFromDB ();

public:
	virtual BOOL Initialize ();

public:
	BOOL SetChannelNotify (int nChannelNo, BOOL bNotify);

};
typedef CTypedPtrList<CPtrList, CollectionDevice*> CCollectionDeviceList;

