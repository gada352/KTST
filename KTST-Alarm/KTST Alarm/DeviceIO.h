#pragma once


#define DEVICEIO_ERROR_SUCCESS						0
#define DEVICEIO_ERROR_OPEN_COMM_FAILURE			1001
#define DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST			1002
#define DEVICEIO_ERROR_DEVICE_DOES_NOT_EXIST		1003
#define DEVICEIO_ERROR_CHANNEL_DOES_NOT_EXIST		1004
#define DEVICEIO_ERROR_DEVICE_DOES_NOT_DEFINE		1005
#define DEVICEIO_ERROR_CHANNEL_DOES_NOT_DEFINE		1006
#define DEVICEIO_ERROR_CHANNEL_UNSUPPORT_COMMAND	1007
#define DEVICEIO_ERROR_COMMUNICATE_FAILURE			1008
#define DEVICEIO_ERROR_UNOPEN_DEVICE				1009
#define DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL		1010
#define DEVICEIO_ERROR_NOTIFYPROC_UNDEFINE			1011
#define DEVICEIO_ERROR_SAMPLE_RATE_IS_ZERO			1012
#define DEVICEIO_ERROR_SET_TIMER_FAILURE			1013
#define DEVICEIO_ERROR_DEVICE_HAS_EXISTED			1014
#define DEVICEIO_ERROR_HANDLE_IS_INVALID			2000
#define DEVICEIO_ERROR_INDEX_IS_INVALID				2001
#define DEVICEIO_ERROR_UNLOAD_DLL					2002
#define DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION		2003
#define DEVICEIO_ERROR_DLLTYPE						2004 //bwh-2007-6-11;
#define DEVICE_OPC_CLIENT_TYPE						17   //bwh-08-8-19;
#define DEVICE_OPC_SERVER_TYPE						16   //bwh-08-8-19;


typedef int (*NOTIFYPROC)(int nTypeID, int nCardID, int nReason, int nContext, short *pBuffer);
typedef int (*OUTPUT_PROC)(int nTypeID, int nCardID, int nChannel, int nValue);
typedef int (*INPUT_PROC)(int nTypeID, int nCardID, int nChannel, int &nValue);
typedef int (*PUTS_PROC)(int nTypeID, int nCardID, int nChannelStart, int nNumber, int nValue[]);
typedef int (*SETNOTIFYPROC)(int nTypeID, int nCardID, NOTIFYPROC NotifyProc, int nContext);
typedef int (*SETSAMPLEPARAM)(int nTypeID, int nCardID, DWORD dwSampleRate, BOOL bWork[], WORD wGain[], WORD wChannelNum);
typedef int (*ONOFF_PROC)(int nTypeID, int nCardID);
typedef int (*ISCHANNELDEFINENEEDED)(int nTypeID, int nCardID, BOOL &bNeed);
typedef int (*CHANNELDEFINE)(int nTypeID, int nCardID, CWnd *pWnd);
typedef int (*GETDEVICETYPEINFO)(int &nTypeID, char szTypeName[]);	//注意：此处为char，不是TCHAR
typedef int (*CLOSEALLCOMM)();
typedef int (*GETDEVICESAMPLEINFO)(int &nTypeID, int &nSampleBit, int &nBigSampleFreq, int &nMaxChNum);
typedef int (*GETDEVICEPROPINFO)(int &nTypeID, DWORD &dwDeviceprop);
typedef int (*CUSTOMBUILD)(DWORD dwCBuild/*=0*/);
typedef int (*INITIALIZE)(char szName[]);	//注意：此处为char，不是TCHAR

typedef struct _DeviceTypeStruct
{
	short nTypeID;
	CString strTypeName;
	CString strDllName;
	short nCardNumber;
	HINSTANCE hInstance;
	OUTPUT_PROC DigitalOutput;
	INPUT_PROC DigitalInput;
	OUTPUT_PROC AnalogOutput;
	INPUT_PROC AnalogInput;
	PUTS_PROC DigitalOutputs;
	PUTS_PROC DigitalInputs;
	PUTS_PROC AnalogOutputs;
	PUTS_PROC AnalogInputs;
	SETNOTIFYPROC SetNotifyProc;
	SETSAMPLEPARAM SetSampleParam;
	ONOFF_PROC Close;
	ONOFF_PROC Stop;
	ONOFF_PROC Start;
	ONOFF_PROC Open;
	ISCHANNELDEFINENEEDED IsChannelDefineNeeded;
	CHANNELDEFINE ChannelDefine;
	GETDEVICETYPEINFO GetDeviceTypeInfo;
	CLOSEALLCOMM CloseAllComm;
	GETDEVICESAMPLEINFO GetDeviceSampleInfo;
	GETDEVICEPROPINFO GetDevicePropInfo;
	CUSTOMBUILD CustomBuild;
	INITIALIZE Initialize;

	_DeviceTypeStruct()
	{
		nTypeID = -1;
		strTypeName.Empty();
		strDllName.Empty();
		nCardNumber = 0;
		hInstance = NULL;
		DigitalOutput = NULL;
		DigitalInput = NULL;
		AnalogOutput = NULL;
		AnalogInput = NULL;
		DigitalOutputs = NULL;
		DigitalInputs = NULL;
		AnalogOutputs = NULL;
		AnalogInputs = NULL;
		SetNotifyProc = NULL;
		SetSampleParam = NULL;
		Close = NULL;
		Stop = NULL;
		Start = NULL;
		Open = NULL;
		IsChannelDefineNeeded = NULL;
		ChannelDefine = NULL;
		GetDeviceTypeInfo = NULL;
		CloseAllComm=NULL;
		GetDeviceSampleInfo = NULL;
		GetDevicePropInfo = NULL;
		CustomBuild = NULL;
		Initialize = NULL;
	}
public:
	~_DeviceTypeStruct()
	{
		if(hInstance)
			::FreeLibrary(hInstance);
	}

}DEVICETYPESTRUCT, *PDEVICETYPESTRUCT;


typedef struct _DeviceStruct
{
	short nTypeID;
	short nCardID;
	CString strCardName;
	short nTotalChannel;

	_DeviceStruct()
	{
		nTypeID = -1;
		nCardID = -1;
		strCardName.Empty();
		nTotalChannel = 0;
	}
}DEVICESTRUCT, *PDEVICESTRUCT;


class CDeviceIO  
{
public:
	CDeviceIO(TCHAR szName[]);
	virtual ~CDeviceIO ();

public:
	void Initialize(TCHAR szName[], BOOL bLoadLibrary=FALSE);

	int RegAddDevice (int nDeviceID, int nTypeID, CString strDevName, int nChannelNum);
	int RegDeleteDevice (int nDeviceID, int nTypeID);
	int RegModDevice (int nDeviceID, int nTypeID, CString strDevName, int nChannelNum);

	int CustomBuild (int nTypeID, DWORD dwCBuild=0);
	int GetDevicePropInfo (int &nTypeID, DWORD &dwDeviceprop);
	int MoadDeRg (int nTypeID, int nDeviceID, int nTtlNum, CString strName, int nDevPropFlag=-1);
	int DltDeRg (int nTypeID, int nDeviceID);
	int GetLcdeGid (TCHAR szLocalGUID[]);
	int GetDeviceSampleInfo (int &nTypeID, int &nSampleBit, int &nBigSampleFreq, int &nMaxChNum);
	int CloseAllComm (int nTypeID);
	int GetTypeDefaultInfo (int &nTypeID, TCHAR szTypeName[]);

	int DigitalOutput (int nHandle, int nChannel, int nValue);
	int DigitalInput (int nHandle, int nChannel, int &nValue);
	int AnalogOutput (int nHandle, int nChannel, int nValue);
	int AnalogInput (int nHandle, int nChannel, int &nValue);
	int DigitalOutputs (int nHandle, int nChannelStart, int nNumber, int nValue[]);
	int DigitalInputs (int nHandle, int nChannelStart, int nNumber, int nValue[]);
	int AnalogOutputs (int nHandle, int nChannelStart, int nNumber, int nValue[]);
	int AnalogInputs (int nHandle, int nChannelStart, int nNumber, int nValue[]);
	int SetNotifyProc (int nHandle, NOTIFYPROC NotifyProc, int nContext);
	int SetSampleParam (int nHandle, DWORD dwSampleRate, BOOL bWork[], WORD wGain[], WORD wChannelNum);
	int Close (int nHandle);
	int Stop (int nHandle);
	int Start (int nHandle);
	int Open (int nHandle);

	int GetCardTypeNum (int &nNumber);
	int GetCardNum (int nTypeID, int &nNumber);
	int GetChannelNum (int nHandle, int &nNumber);
	int GetCardHandle (int nTypeID, int nCardID, int &nHandle);
	int GetCardTypeName (int nTypeID, TCHAR szTypeName[]);
	int GetCardName (int nHandle, TCHAR szCardName[]);
	int IsChannelDefineNeeded (int nHandle, BOOL &bNeed);
	int ChannelDefine (int nHandle, CWnd *pWnd);
	int GetCardTypeID (int nIndex, int &nTypeID);
	int GetCardID (int nTypeID, int nIndex, int &nCardID);

private:
	BOOL GetIndexByHandle (int nHandle, int &iType, int &iCard);
	int GetCardIndexByID (int nTypeID, int nCardID);
	int GetTypeIndexByID (int nTypeID);
	void FreeAllLibrary ();
	void LoadAllLibrary ();
	BOOL RegGetAllTypeAndCard ();

private:
	DEVICETYPESTRUCT m_deviceTypeArray[_MAX_DEVICE_TYPE];
	DEVICESTRUCT m_deviceArray[_MAX_DEVICE];
	int m_nCardTypeNum;
	int m_nCardArrLen;
	CString m_strProseg;
};
