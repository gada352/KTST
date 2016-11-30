
#include "stdafx.h"

#include "DeviceIO.h"
#include "shlwapi.h"

#include "Registry.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destructionle
//////////////////////////////////////////////////////////////////////

CDeviceIO::~CDeviceIO()
{
	FreeAllLibrary();

}


CDeviceIO::CDeviceIO(TCHAR szName[])
{
// 	//bwh-10-12-31为该进程初始化COM库(防止另存为对话框死掉，必须初始化COM库)；
// 	g_bComInitialized = FALSE;
// 
// 	HRESULT hr =CoInitialize(NULL);
// 	g_bComInitialized = SUCCEEDED (hr);	

	Initialize (szName, TRUE);
}


void CDeviceIO::Initialize(TCHAR szName[], BOOL bLoadLibrary/*=FALSE*/)
{
	m_nCardTypeNum = 0;
	m_nCardArrLen = 0;
	m_strProseg = szName;


	if(!RegGetAllTypeAndCard())
	{
		m_nCardTypeNum = 0;
		m_nCardArrLen = 0;
	}

	if (bLoadLibrary)
		LoadAllLibrary();

	//由于ModbusRTU动态库的Initialize、GetDeviceTypeInfo接口为char并且是多字节编码，所以此处要转换一下字符串2015-07-31-zjb
	CStringA strA(szName);
	char chName[REG_BUF_LEN];
	strcpy (chName, (LPCSTR)strA);

	for(int i=0;i<m_nCardTypeNum;i++)
	{
		if(m_deviceTypeArray[i].hInstance&&m_deviceTypeArray[i].Initialize)
			m_deviceTypeArray[i].Initialize(chName);
	}
}


int CDeviceIO::GetCardID(int nTypeID, int nIndex, int &nCardID)
{
	nCardID = -1;

	int index = GetTypeIndexByID(nTypeID);
	if(index < 0)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;

	if(nIndex < 0 || nIndex >= m_deviceTypeArray[index].nCardNumber)
		return DEVICEIO_ERROR_INDEX_IS_INVALID;

	int i;
	int n=0;
	for(i=0;i<m_nCardArrLen;i++)
	{
		if(m_deviceArray[i].nTypeID==nTypeID)
		{
			if(nIndex==n)
				break;
			n++;
		}
	}

	nCardID = m_deviceArray[i].nCardID;

	return DEVICEIO_ERROR_SUCCESS;
}


int CDeviceIO::GetCardTypeID(int nIndex, int &nTypeID)
{
	nTypeID = -1;

	if(nIndex < 0 || nIndex >= m_nCardTypeNum)
		return DEVICEIO_ERROR_INDEX_IS_INVALID;

	nTypeID = m_deviceTypeArray[nIndex].nTypeID;

	return DEVICEIO_ERROR_SUCCESS;
}


int CDeviceIO::ChannelDefine(int nHandle, CWnd *pWnd)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].ChannelDefine)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].ChannelDefine(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		pWnd);

	return iResult;
}


int CDeviceIO::IsChannelDefineNeeded(int nHandle, BOOL &bNeed)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].IsChannelDefineNeeded)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].IsChannelDefineNeeded(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		bNeed);

	return iResult;
}


int CDeviceIO::GetCardName(int nHandle, TCHAR szCardName[])
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;
	
	_tcscpy(szCardName, (LPCTSTR )m_deviceArray[iCard].strCardName);

	return DEVICEIO_ERROR_SUCCESS;
}


int CDeviceIO::GetCardTypeName(int nTypeID, TCHAR szTypeName[])
{
	int index = GetTypeIndexByID(nTypeID);
	if(index < 0)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;

	_tcscpy(szTypeName, (LPCTSTR )m_deviceTypeArray[index].strTypeName);

	return DEVICEIO_ERROR_SUCCESS;
}


int CDeviceIO::GetCardHandle(int nTypeID, int nCardID, int &nHandle)
{
	nHandle = NULL;

	int iType = GetTypeIndexByID(nTypeID);
	if(iType < 0)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;

	int iCard = GetCardIndexByID(nTypeID, nCardID);
	if(iCard < 0)
		return DEVICEIO_ERROR_DEVICE_DOES_NOT_EXIST;

	nHandle = (iType << 16) + iCard + 1;

	return DEVICEIO_ERROR_SUCCESS;
}


int CDeviceIO::GetChannelNum(int nHandle, int &nNumber)
{
	nNumber = 0;

	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	nNumber = m_deviceArray[iCard].nTotalChannel;

	return DEVICEIO_ERROR_SUCCESS;
}


int CDeviceIO::GetCardNum(int nTypeID, int &nNumber)
{
	nNumber = 0;

	int index = GetTypeIndexByID(nTypeID);
	if(index < 0)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;

	nNumber = m_deviceTypeArray[index].nCardNumber;

	return DEVICEIO_ERROR_SUCCESS;
}


int CDeviceIO::GetCardTypeNum(int &nNumber)
{
	nNumber = m_nCardTypeNum;

	return DEVICEIO_ERROR_SUCCESS;
}


int CDeviceIO::Open(int nHandle)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].Open)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].Open(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID);

	return iResult;
}


int CDeviceIO::Start(int nHandle)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].Start)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].Start(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID);

	return iResult;
}


int CDeviceIO::Stop(int nHandle)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].Stop)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].Stop(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID);

	return iResult;
}


int CDeviceIO::Close(int nHandle)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].Close)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].Close(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID);

	return iResult;
}


int CDeviceIO::SetSampleParam(int nHandle, DWORD dwSampleRate, BOOL bWork[], WORD wGain[], WORD wChannelNum)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].SetSampleParam)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].SetSampleParam(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		dwSampleRate,
		bWork,
		wGain,
		wChannelNum);

	return iResult;
}


int CDeviceIO::SetNotifyProc(int nHandle, NOTIFYPROC NotifyProc, int nContext)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].SetNotifyProc)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].SetNotifyProc(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		NotifyProc,
		nContext);

	return iResult;
}


int CDeviceIO::AnalogInputs(int nHandle, int nChannelStart, int nNumber, int nValue[])
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].AnalogInputs)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].AnalogInputs(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		nChannelStart,
		nNumber,
		nValue);

	return iResult;
}


int CDeviceIO::AnalogOutputs(int nHandle, int nChannelStart, int nNumber, int nValue[])
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].AnalogOutputs)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].AnalogOutputs(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		nChannelStart,
		nNumber,
		nValue);

	return iResult;
}


int CDeviceIO::DigitalInputs(int nHandle, int nChannelStart, int nNumber, int nValue[])
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].DigitalInputs)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].DigitalInputs(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		nChannelStart,
		nNumber,
		nValue);

	return iResult;
}


int CDeviceIO::DigitalOutputs(int nHandle, int nChannelStart, int nNumber, int nValue[])
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].DigitalOutputs)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].DigitalOutputs(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		nChannelStart,
		nNumber,
		nValue);

	return iResult;
}


int CDeviceIO::AnalogInput(int nHandle, int nChannel, int &nValue)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].AnalogInput)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].AnalogInput(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		nChannel,
		nValue);

	return iResult;
}


int CDeviceIO::AnalogOutput(int nHandle, int nChannel, int nValue)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].AnalogOutput)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].AnalogOutput(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		nChannel,
		nValue);

	return iResult;
}


int CDeviceIO::DigitalInput(int nHandle, int nChannel, int &nValue)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].DigitalInput)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].DigitalInput(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		nChannel,
		nValue);

	return iResult;
}


int CDeviceIO::DigitalOutput(int nHandle, int nChannel, int nValue)
{
	int iType, iCard;
	if(!GetIndexByHandle(nHandle, iType, iCard))
		return DEVICEIO_ERROR_HANDLE_IS_INVALID;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].DigitalOutput)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].DigitalOutput(m_deviceArray[iCard].nTypeID,
		m_deviceArray[iCard].nCardID,
		nChannel,
		nValue);

	return iResult;
}


BOOL CDeviceIO::RegGetAllTypeAndCard()
{
	m_nCardTypeNum = 0;
	m_nCardArrLen = 0;

	CRegistry reg(m_strProseg);

	DWORD hRoot = 0;
	if(reg.OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return FALSE;

	DWORD hDeviceKey = 0;
	if(reg.RegOpenKey(hRoot, REG_DEVICE_SUBKEY, hDeviceKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hRoot);
		return FALSE;
	}
	reg.RegCloseKey(hRoot);
	
	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	short nTypeNumber = 0;

	cbValue = sizeof(DWORD);
	if(reg.RegQueryValue(hDeviceKey, REG_TYPE_NUMBER_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hDeviceKey);
		return FALSE;
	}
	nTypeNumber = (short)*(DWORD *)value;

	int nCardTypeNum = 0;
	int nCardArrLen = 0;

	DWORD hTypeKey = 0;
	DWORD hCardKey = 0;

	TCHAR szKeyName[REG_BUF_LEN];

	short nTypeID = -1;
	CString strTypeName;
	CString strDllName;
	short nCardNumber = 0;

	short nCardID = -1;
	CString strCardName;
	short nTotalChannel = 0;

	for(int i=0,j;;i++)
	{
		memset(szKeyName, 0x0, REG_BUF_LEN*sizeof(TCHAR));
		if(reg.RegEnumKey(hDeviceKey, i, szKeyName, REG_BUF_LEN) != REG_ERROR_SUCCESS)
			break;

		if(reg.RegOpenKey(hDeviceKey, szKeyName, hTypeKey) != REG_ERROR_SUCCESS)
		{
			reg.RegCloseKey(hDeviceKey);
			return FALSE;
		}

		cbValue = sizeof(DWORD);
		if(reg.RegQueryValue(hTypeKey, REG_TYPE_ID_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
		{
			reg.RegCloseKey(hTypeKey);
			continue;
		}
		nTypeID = (short)*(DWORD *)value;


		memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
		cbValue = REG_BUF_LEN;
		if(reg.RegQueryValue(hTypeKey, REG_TYPE_NAME_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
		{
			reg.RegCloseKey(hTypeKey);
			continue;
		}
		strTypeName = (TCHAR *)value;

		memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
		cbValue = REG_BUF_LEN;
		if(reg.RegQueryValue(hTypeKey, REG_DLL_NAME_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
		{
			reg.RegCloseKey(hTypeKey);
			continue;
		}
		strDllName = (TCHAR *)value;

		cbValue = sizeof(DWORD);
		if(reg.RegQueryValue(hTypeKey, REG_DEVICE_NUMBER_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
		{
			reg.RegCloseKey(hTypeKey);
			continue;
		}
		nCardNumber = (short)*(DWORD *)value;

		int nCardNumCount = 0;
		for(j=0;;j++)
		{
			memset(szKeyName, 0x0, REG_BUF_LEN*sizeof(TCHAR));
			if(reg.RegEnumKey(hTypeKey, j, szKeyName, REG_BUF_LEN) != REG_ERROR_SUCCESS)
				break;
			
			if(_tcscmp(szKeyName, REG_CLASS_PARAM_SUBKEY) == 0)
				continue;
			
			if(reg.RegOpenKey(hTypeKey, szKeyName, hCardKey) != REG_ERROR_SUCCESS)
			{
				reg.RegCloseKey(hTypeKey);
				reg.RegCloseKey(hDeviceKey);
				return FALSE;
			}

			cbValue = sizeof(DWORD);
			if(reg.RegQueryValue(hCardKey, REG_DEVICE_NO_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
			{
				reg.RegCloseKey(hCardKey);
				continue;
			}
			nCardID = (short)*(DWORD *)value;

			memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
			cbValue = REG_BUF_LEN;
			if(reg.RegQueryValue(hCardKey, REG_DEVICE_NAME_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
			{
				reg.RegCloseKey(hCardKey);
				continue;
			}
			strCardName = (TCHAR *)value;

			cbValue = sizeof(DWORD);
			if(reg.RegQueryValue(hCardKey, REG_TOTAL_CHANNEL_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
			{
				reg.RegCloseKey(hCardKey);
				continue;
			}
			nTotalChannel = (short)*(DWORD *)value;

			m_deviceArray[nCardArrLen].nTypeID = nTypeID;
			m_deviceArray[nCardArrLen].nCardID = nCardID;
			m_deviceArray[nCardArrLen].strCardName = strCardName;
			m_deviceArray[nCardArrLen].nTotalChannel = nTotalChannel;

			nCardArrLen++;

			reg.RegCloseKey(hCardKey);

			nCardNumCount++;
		}

		if(nCardNumCount != nCardNumber)
		{
			reg.RegCloseKey(hTypeKey);
			reg.RegCloseKey(hDeviceKey);
			return FALSE;
		}

		m_deviceTypeArray[nCardTypeNum].nTypeID = nTypeID;
		m_deviceTypeArray[nCardTypeNum].strTypeName = strTypeName;
		m_deviceTypeArray[nCardTypeNum].strDllName = strDllName;
		m_deviceTypeArray[nCardTypeNum].nCardNumber = nCardNumber;

		nCardTypeNum++;

		reg.RegCloseKey(hTypeKey);
	}

	reg.RegCloseKey(hDeviceKey);

	if(nCardTypeNum > nTypeNumber)
		return FALSE;

	m_nCardTypeNum = nCardTypeNum;
	m_nCardArrLen = nCardArrLen;

	return TRUE;
}


int CDeviceIO::DltDeRg(int nTypeID, int nDeviceID)
{
	CRegistry reg(m_strProseg);
	int nTypei = GetTypeIndexByID(nTypeID);

	if(nTypei == -1)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;
	int nDevi = GetCardIndexByID(nTypeID, nDeviceID);

	DWORD hRoot = 0;
	if(reg.OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	DWORD hDeviceKey = 0;
	if(reg.RegOpenKey(hRoot, REG_DEVICE_SUBKEY, hDeviceKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hRoot);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hRoot);
	CString strKey;

	strKey.Format(REG_TYPE_ID_KEY_NAME, nTypeID);
	DWORD hTypeKey = 0;
	if(reg.RegOpenKey(hDeviceKey, strKey, hTypeKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hDeviceKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hDeviceKey);
	strKey.Format(REG_DEVICE_NO_KEY_NAME, nDeviceID);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;
	TCHAR szKeyName[REG_BUF_LEN];

	SHDeleteKey((HKEY)hTypeKey, strKey);
	int n = 0;
	for(int j=0;;j++)
	{
		memset(szKeyName, 0x0, REG_BUF_LEN*sizeof(TCHAR));
		if(reg.RegEnumKey(hTypeKey, j, szKeyName, REG_BUF_LEN) != REG_ERROR_SUCCESS)
			break;
		if(_tcscmp(szKeyName, REG_CLASS_PARAM_SUBKEY) == 0)
			continue;
		n++;
	}

	dwType = REG_DWORD;
	*(int *)value = n;
	cbValue = sizeof(int);
	if(reg.RegSetValue(hTypeKey, REG_DEVICE_NUMBER_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hTypeKey);
	if(nDevi == -1)
	{
	}
	else
	{
		for(int i=nDevi+1;i<m_nCardArrLen;i++)
		{
			m_deviceArray[i-1].nTypeID = m_deviceArray[i].nTypeID;
			m_deviceArray[i-1].nCardID = m_deviceArray[i].nCardID;
			m_deviceArray[i-1].nTotalChannel = m_deviceArray[i].nTotalChannel;
			m_deviceArray[i-1].strCardName = m_deviceArray[i].strCardName;
		}
		m_deviceTypeArray[nTypei].nCardNumber--;
		m_nCardArrLen--;
	}
	return DEVICEIO_ERROR_SUCCESS;
}


void CDeviceIO::LoadAllLibrary()
{
	for(int i=0;i<m_nCardTypeNum;i++)
	{
		if(!m_deviceTypeArray[i].hInstance)
		{
			m_deviceTypeArray[i].hInstance = ::LoadLibrary(m_deviceTypeArray[i].strDllName);

			if(m_deviceTypeArray[i].hInstance)
			{
				m_deviceTypeArray[i].DigitalOutput = (OUTPUT_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "DigitalOutput");
				m_deviceTypeArray[i].DigitalInput = (INPUT_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "DigitalInput");
				m_deviceTypeArray[i].AnalogOutput = (OUTPUT_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "AnalogOutput");
				m_deviceTypeArray[i].AnalogInput = (INPUT_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "AnalogInput");
				m_deviceTypeArray[i].DigitalOutputs = (PUTS_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "DigitalOutputs");
				m_deviceTypeArray[i].DigitalInputs = (PUTS_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "DigitalInputs");
				m_deviceTypeArray[i].AnalogOutputs = (PUTS_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "AnalogOutputs");
				m_deviceTypeArray[i].AnalogInputs = (PUTS_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "AnalogInputs");
				m_deviceTypeArray[i].SetNotifyProc = (SETNOTIFYPROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "SetNotifyProc");
				m_deviceTypeArray[i].SetSampleParam = (SETSAMPLEPARAM)::GetProcAddress(m_deviceTypeArray[i].hInstance, "SetSampleParam");
				m_deviceTypeArray[i].Close = (ONOFF_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "Close");
				m_deviceTypeArray[i].Stop = (ONOFF_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "Stop");
				m_deviceTypeArray[i].Start = (ONOFF_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "Start");
				m_deviceTypeArray[i].Open = (ONOFF_PROC)::GetProcAddress(m_deviceTypeArray[i].hInstance, "Open");
				m_deviceTypeArray[i].IsChannelDefineNeeded = (ISCHANNELDEFINENEEDED)::GetProcAddress(m_deviceTypeArray[i].hInstance, "IsChannelDefineNeeded");
				m_deviceTypeArray[i].ChannelDefine = (CHANNELDEFINE)::GetProcAddress(m_deviceTypeArray[i].hInstance, "ChannelDefine");
				m_deviceTypeArray[i].GetDeviceTypeInfo = (GETDEVICETYPEINFO)::GetProcAddress(m_deviceTypeArray[i].hInstance, "GetDeviceTypeInfo");
				m_deviceTypeArray[i].CloseAllComm = (CLOSEALLCOMM)::GetProcAddress(m_deviceTypeArray[i].hInstance, "CloseAllComm");				
				m_deviceTypeArray[i].GetDeviceSampleInfo = (GETDEVICESAMPLEINFO)::GetProcAddress(m_deviceTypeArray[i].hInstance, "GetDeviceSampleInfo");
				m_deviceTypeArray[i].GetDevicePropInfo = (GETDEVICEPROPINFO)::GetProcAddress(m_deviceTypeArray[i].hInstance, "GetDevicePropInfo");
				m_deviceTypeArray[i].CustomBuild = (CUSTOMBUILD)::GetProcAddress(m_deviceTypeArray[i].hInstance, "CustomBuild");
				m_deviceTypeArray[i].Initialize = (INITIALIZE)::GetProcAddress(m_deviceTypeArray[i].hInstance, "Initialize");
			}
			else
			{
				/*
				CString strInfo;
			//	strInfo.LoadString(IDS_LOADDLLFAIL);
				if(m_deviceTypeArray[i].strDllName=="")	
					strInfo.Format("Fail in loading %s.dll", m_deviceTypeArray[i].strTypeName);
				else
					strInfo.Format("Fail in loading %s", m_deviceTypeArray[i].strDllName);
				::AfxMessageBox(strInfo);
				*/
			}
		}
	}
}


void CDeviceIO::FreeAllLibrary()
{
	for(int i=0;i<m_nCardTypeNum;i++)
	{
		if(m_deviceTypeArray[i].hInstance)
		{
			BOOL bRes = ::FreeLibrary(m_deviceTypeArray[i].hInstance);

			m_deviceTypeArray[i].hInstance = NULL;
			m_deviceTypeArray[i].DigitalOutput = NULL;
			m_deviceTypeArray[i].DigitalInput = NULL;
			m_deviceTypeArray[i].AnalogOutput = NULL;
			m_deviceTypeArray[i].AnalogInput = NULL;
			m_deviceTypeArray[i].DigitalOutputs = NULL;
			m_deviceTypeArray[i].DigitalInputs = NULL;
			m_deviceTypeArray[i].AnalogOutputs = NULL;
			m_deviceTypeArray[i].AnalogInputs = NULL;
			m_deviceTypeArray[i].SetNotifyProc = NULL;
			m_deviceTypeArray[i].SetSampleParam = NULL;
			m_deviceTypeArray[i].Close = NULL;
			m_deviceTypeArray[i].Stop = NULL;
			m_deviceTypeArray[i].Start = NULL;
			m_deviceTypeArray[i].Open = NULL;
			m_deviceTypeArray[i].IsChannelDefineNeeded = NULL;
			m_deviceTypeArray[i].ChannelDefine = NULL;
			m_deviceTypeArray[i].GetDeviceTypeInfo = NULL;
			m_deviceTypeArray[i].CloseAllComm = NULL;
			m_deviceTypeArray[i].GetDeviceSampleInfo = NULL;
			m_deviceTypeArray[i].GetDevicePropInfo = NULL;
			m_deviceTypeArray[i].CustomBuild = NULL;
			m_deviceTypeArray[i].Initialize = NULL;
		}
	}
}


int CDeviceIO::GetTypeIndexByID(int nTypeID)
{
	for(int i=0;i<m_nCardTypeNum;i++)
	{
		if(m_deviceTypeArray[i].nTypeID == nTypeID)
		{
			return i;
		}
	}

	return -1;
}


int CDeviceIO::GetCardIndexByID(int nTypeID, int nCardID)
{
	for(int i=0;i<m_nCardArrLen;i++)
	{
		if(m_deviceArray[i].nTypeID == nTypeID &&
			m_deviceArray[i].nCardID == nCardID)
			return i;
	}

	return -1;
}


BOOL CDeviceIO::GetIndexByHandle(int nHandle, int &iType, int &iCard)
{
	iType = -1;
	iCard = -1;

	int ixType = nHandle >> 16;
	int ixCard = nHandle - (ixType << 16) - 1;

	if(ixType < 0 || ixType >= m_nCardTypeNum ||
		ixCard < 0 || ixCard >= m_nCardArrLen)
		return FALSE;

	iType = ixType;
	iCard = ixCard;

	return TRUE;
}


int CDeviceIO::GetTypeDefaultInfo(int &nTypeID, TCHAR szTypeName[])
{
	int iType;
	if((iType = GetTypeIndexByID(nTypeID)) == -1)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].GetDeviceTypeInfo)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	//由于ModbusRTU动态库的Initialize、GetDeviceTypeInfo接口为char并且是多字节编码，所以此处要转换一下字符串2015-07-31-zjb
	char chTypeName[REG_BUF_LEN];
	memset (chTypeName, 0x0, sizeof(chTypeName));

	int iResult = m_deviceTypeArray[iType].GetDeviceTypeInfo(nTypeID,
		chTypeName);

	CString str(chTypeName);
	_tcscpy(szTypeName, (LPCTSTR )str);

	return iResult;
}

int CDeviceIO::CloseAllComm(int nTypeID)
{
	int iType;
	if((iType = GetTypeIndexByID(nTypeID)) == -1)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;
	if(!m_deviceTypeArray[iType].CloseAllComm)//不能为空
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;
	int iRes = m_deviceTypeArray[iType].CloseAllComm();
	return iRes;
}


//获取设备采样位数、支持的采样频率、通道数
int CDeviceIO::GetDeviceSampleInfo(int &nTypeID, int &nSampleBit, int &nBigSampleFreq, int &nMaxChNum)
{
	int iType;
	if((iType = GetTypeIndexByID(nTypeID)) == -1)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].GetDeviceSampleInfo)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].GetDeviceSampleInfo(nTypeID, nSampleBit, 
		nBigSampleFreq, nMaxChNum);

	return iResult;
}


int CDeviceIO::GetLcdeGid(TCHAR szLocalGUID[])
{
	CRegistry reg(m_strProseg);
	GUID guid;
	DWORD hRoot = 0;

	if(reg.OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;

	DWORD hTargetKey = 0;

	if(reg.RegOpenKey(hRoot, REG_DEVICE_SUBKEY, hTargetKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hRoot);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hRoot);
	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = REG_BUF_LEN;
	CString strGUID;

	if(reg.RegQueryValue(hTargetKey, REG_GUID_SUBKEY, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		memset((BYTE *)&guid,0,sizeof(GUID));
		if(CoCreateGuid(&guid))
		{
			AfxMessageBox(_T("Create guid fails!"));
		}

		dwType = REG_SZ;
		memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
		memcpy(value, (BYTE *)&guid, sizeof(GUID));
		_stprintf((TCHAR *)value, _T("%08X%04X%04X%08X%08X"), guid.Data1, 
			guid.Data2, guid.Data3, *(DWORD *)guid.Data4, *(DWORD *)(guid.Data4 + sizeof(DWORD)));
		cbValue = 32*sizeof(TCHAR);	//GUID为128位、16个字节，十六进制用字符串表示为32个字符(1个字节两个字符)
		if(reg.RegSetValue(hTargetKey, REG_GUID_SUBKEY, dwType, value, cbValue) != REG_ERROR_SUCCESS)
		{
			reg.RegCloseKey(hTargetKey);
			return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
		}
	}

	reg.RegCloseKey(hTargetKey);
	_tcscpy(szLocalGUID, (TCHAR *)value);

	return DEVICEIO_ERROR_SUCCESS;
}


int CDeviceIO::MoadDeRg(int nTypeID, int nDeviceID, int nTtlNum, CString strName, int nDevPropFlag/*=-1*/)
{
	CRegistry reg(m_strProseg);
	int nTypei = GetTypeIndexByID(nTypeID);

	if(nTypei == -1)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;
	int nDevi = GetCardIndexByID(nTypeID, nDeviceID);

	DWORD hRoot = 0;
	if(reg.OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;

	DWORD hDeviceKey = 0;
	if(reg.RegOpenKey(hRoot, REG_DEVICE_SUBKEY, hDeviceKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hRoot);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hRoot);
	CString strKey;

	strKey.Format(REG_TYPE_ID_KEY_NAME, nTypeID);
	DWORD hTypeKey = 0;
	if(reg.RegOpenKey(hDeviceKey, strKey, hTypeKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hDeviceKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}
	reg.RegCloseKey(hDeviceKey);

	strKey.Format(REG_DEVICE_NO_KEY_NAME, nDeviceID);
	DWORD hCardKey = 0;

	if(reg.RegCreateKey(hTypeKey, strKey, hCardKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}
	
	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_DWORD;
	*(int *)value = nDeviceID;
	cbValue = sizeof(int);
	if(reg.RegSetValue(hCardKey, REG_DEVICE_NO_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		reg.RegCloseKey(hCardKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	dwType = REG_DWORD;
	*(int *)value = nTtlNum;
	cbValue = sizeof(int);
	if(reg.RegSetValue(hCardKey, REG_TOTAL_CHANNEL_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		reg.RegCloseKey(hCardKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	dwType = REG_SZ;
	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	_tcscpy((TCHAR *)value, strName);
	cbValue = strName.GetLength()*sizeof(TCHAR);
	if(reg.RegSetValue(hCardKey, REG_DEVICE_NAME_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		reg.RegCloseKey(hCardKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	if (nDevPropFlag >= 0)		//对于某些类型设备要将设备属性也写进注册表08-12-23-zhjb
	{
		dwType = REG_DWORD;
		*(int *)value = nDevPropFlag;
		cbValue = sizeof(int);
		if(reg.RegSetValue(hCardKey, REG_DEVICE_PROPERTY_FLAG, dwType, value, cbValue) != REG_ERROR_SUCCESS)
		{
			reg.RegCloseKey(hTypeKey);
			reg.RegCloseKey(hCardKey);
			return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
		}
	}

	TCHAR szKeyName[REG_BUF_LEN];
	reg.RegCloseKey(hCardKey);

	int n = 0;
	for(int j=0;;j++)
	{
		memset(szKeyName, 0x0, REG_BUF_LEN*sizeof(TCHAR));
		if(reg.RegEnumKey(hTypeKey, j, szKeyName, REG_BUF_LEN) != REG_ERROR_SUCCESS)
			break;
		if(_tcscmp(szKeyName, REG_CLASS_PARAM_SUBKEY) == 0)
			continue;
		n++;
	}

	dwType = REG_DWORD;
	*(int *)value = n;
	cbValue = sizeof(int);
	if(reg.RegSetValue(hTypeKey, REG_DEVICE_NUMBER_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hTypeKey);
	if(nDevi == -1)
	{
		m_deviceArray[m_nCardArrLen].nTypeID = nTypeID;
		m_deviceArray[m_nCardArrLen].nCardID = nDeviceID;
		m_deviceArray[m_nCardArrLen].nTotalChannel = nTtlNum;
		m_deviceArray[m_nCardArrLen].strCardName = strName;
		m_deviceTypeArray[nTypei].nCardNumber++;
		m_nCardArrLen++;
	}
	else
	{
		m_deviceArray[nDevi].nTotalChannel = nTtlNum;
		m_deviceArray[nDevi].strCardName = strName;
	}
	return DEVICEIO_ERROR_SUCCESS;
}


int CDeviceIO::CustomBuild(int nTypeID, DWORD dwCBuild)
{
	int iType;
	if((iType = GetTypeIndexByID(nTypeID)) == -1)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;
	if(!m_deviceTypeArray[iType].CustomBuild)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;
	int iResult = m_deviceTypeArray[iType].CustomBuild(dwCBuild);
	return iResult;
}

//获取设备属性
int CDeviceIO::GetDevicePropInfo(int &nTypeID, DWORD &dwDeviceprop)
{
	int iType;
	if((iType = GetTypeIndexByID(nTypeID)) == -1)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;

	if(!m_deviceTypeArray[iType].hInstance)
		return DEVICEIO_ERROR_UNLOAD_DLL;

	if(!m_deviceTypeArray[iType].GetDevicePropInfo)
		return DEVICEIO_ERROR_NO_DLLIMPORT_FUNCTION;

	int iResult = m_deviceTypeArray[iType].GetDevicePropInfo(nTypeID, 
		dwDeviceprop);

	return iResult;
}


int CDeviceIO::RegAddDevice(int nDeviceID, int nTypeID, CString strDevName, int nChannelNum)
{
	CRegistry reg(m_strProseg);
	int nTypei = GetTypeIndexByID(nTypeID);

	if(nTypei == -1)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;
	int nDevi = GetCardIndexByID(nTypeID, nDeviceID);
	if(nDevi != -1)
		return DEVICEIO_ERROR_DEVICE_HAS_EXISTED;
	DWORD hRoot = 0;
	if(reg.OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	DWORD hDeviceKey = 0;
	if(reg.RegOpenKey(hRoot, REG_DEVICE_SUBKEY, hDeviceKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hRoot);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hRoot);
	CString strKey;

	strKey.Format(REG_TYPE_ID_KEY_NAME, nTypeID);
	DWORD hTypeKey = 0;
	if(reg.RegOpenKey(hDeviceKey, strKey, hTypeKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hDeviceKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}
	reg.RegCloseKey(hDeviceKey);

	strKey.Format(REG_DEVICE_NO_KEY_NAME, nDeviceID);
	DWORD hCardKey = 0;

	if(reg.RegCreateKey(hTypeKey, strKey, hCardKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}
	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_DWORD;
	*(int *)value = nDeviceID;
	cbValue = sizeof(int);
	if(reg.RegSetValue(hCardKey, REG_DEVICE_NO_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		reg.RegCloseKey(hCardKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	dwType = REG_DWORD;
	*(int *)value = nChannelNum;
	cbValue = sizeof(int);
	if(reg.RegSetValue(hCardKey, REG_TOTAL_CHANNEL_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		reg.RegCloseKey(hCardKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	dwType = REG_SZ;
	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	_tcscpy((TCHAR *)value, strDevName);
	cbValue = strDevName.GetLength()*sizeof(TCHAR);
	if(reg.RegSetValue(hCardKey, REG_DEVICE_NAME_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		reg.RegCloseKey(hCardKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	TCHAR szKeyName[REG_BUF_LEN];
	reg.RegCloseKey(hCardKey);

	int n = 0;
	for(int j=0;;j++)
	{
		memset(szKeyName, 0x0, REG_BUF_LEN*sizeof(TCHAR));
		if(reg.RegEnumKey(hTypeKey, j, szKeyName, REG_BUF_LEN) != REG_ERROR_SUCCESS)
			break;
		if(_tcscmp(szKeyName, REG_CLASS_PARAM_SUBKEY) == 0)
			continue;
		n++;
	}

	dwType = REG_DWORD;
	*(int *)value = n;
	cbValue = sizeof(int);
	if(reg.RegSetValue(hTypeKey, REG_DEVICE_NUMBER_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hTypeKey);
	m_deviceArray[m_nCardArrLen].nTypeID = nTypeID;
	m_deviceArray[m_nCardArrLen].nCardID = nDeviceID;
	m_deviceArray[m_nCardArrLen].nTotalChannel = nChannelNum;
	m_deviceArray[m_nCardArrLen].strCardName = strDevName;
	m_deviceTypeArray[nTypei].nCardNumber = n;
	m_nCardArrLen++;

	return DEVICEIO_ERROR_SUCCESS;
}

int CDeviceIO::RegDeleteDevice(int nDeviceID, int nTypeID)
{
	CRegistry reg(m_strProseg);
	int nTypei = GetTypeIndexByID(nTypeID);

	if(nTypei == -1)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;
	int nDevi = GetCardIndexByID(nTypeID, nDeviceID);
	if(nDevi == -1)
		return DEVICEIO_ERROR_SUCCESS;
	DWORD hRoot = 0;
	if(reg.OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	DWORD hDeviceKey = 0;
	if(reg.RegOpenKey(hRoot, REG_DEVICE_SUBKEY, hDeviceKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hRoot);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hRoot);
	CString strKey;

	strKey.Format(REG_TYPE_ID_KEY_NAME, nTypeID);
	DWORD hTypeKey = 0;
	if(reg.RegOpenKey(hDeviceKey, strKey, hTypeKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hDeviceKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hDeviceKey);
	strKey.Format(REG_DEVICE_NO_KEY_NAME, nDeviceID);

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;
	TCHAR szKeyName[REG_BUF_LEN];

	SHDeleteKey((HKEY)hTypeKey, strKey);
	int n = 0;
	for(int j=0;;j++)
	{
		memset(szKeyName, 0x0, REG_BUF_LEN*sizeof(TCHAR));
		if(reg.RegEnumKey(hTypeKey, j, szKeyName, REG_BUF_LEN) != REG_ERROR_SUCCESS)
			break;
		if(_tcscmp(szKeyName, REG_CLASS_PARAM_SUBKEY) == 0)
			continue;
		n++;
	}

	dwType = REG_DWORD;
	*(int *)value = n;
	cbValue = sizeof(int);
	if(reg.RegSetValue(hTypeKey, REG_DEVICE_NUMBER_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hTypeKey);
	for(int i=nDevi+1;i<m_nCardArrLen;i++)
	{
		m_deviceArray[i-1].nTypeID = m_deviceArray[i].nTypeID;
		m_deviceArray[i-1].nCardID = m_deviceArray[i].nCardID;
		m_deviceArray[i-1].nTotalChannel = m_deviceArray[i].nTotalChannel;
		m_deviceArray[i-1].strCardName = m_deviceArray[i].strCardName;
	}
	m_deviceTypeArray[nTypei].nCardNumber=n;
	m_nCardArrLen--;

	return DEVICEIO_ERROR_SUCCESS;
}

int CDeviceIO::RegModDevice(int nDeviceID, int nTypeID, CString strDevName, int nChannelNum)
{
	CRegistry reg(m_strProseg);
	int nTypei = GetTypeIndexByID(nTypeID);

	if(nTypei == -1)
		return DEVICEIO_ERROR_TYPE_DOES_NOT_EXIST;
	int nDevi = GetCardIndexByID(nTypeID, nDeviceID);
	if(nDevi == -1)
		return DEVICEIO_ERROR_DEVICE_DOES_NOT_EXIST;
	DWORD hRoot = 0;
	if(reg.OpenRootKey(hRoot) != REG_ERROR_SUCCESS)
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;

	DWORD hDeviceKey = 0;
	if(reg.RegOpenKey(hRoot, REG_DEVICE_SUBKEY, hDeviceKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hRoot);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	reg.RegCloseKey(hRoot);
	CString strKey;

	strKey.Format(REG_TYPE_ID_KEY_NAME, nTypeID);
	DWORD hTypeKey = 0;
	if(reg.RegOpenKey(hDeviceKey, strKey, hTypeKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hDeviceKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}
	reg.RegCloseKey(hDeviceKey);

	strKey.Format(REG_DEVICE_NO_KEY_NAME, nDeviceID);
	DWORD hCardKey = 0;

	if(reg.RegCreateKey(hTypeKey, strKey, hCardKey) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	DWORD dwType = 0;
	BYTE value[REG_BUF_LEN];
	DWORD cbValue = 0;

	dwType = REG_DWORD;
	*(int *)value = nDeviceID;
	cbValue = sizeof(int);
	if(reg.RegSetValue(hCardKey, REG_DEVICE_NO_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		reg.RegCloseKey(hCardKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	dwType = REG_DWORD;
	*(int *)value = nChannelNum;
	cbValue = sizeof(int);
	if(reg.RegSetValue(hCardKey, REG_TOTAL_CHANNEL_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		reg.RegCloseKey(hCardKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}

	dwType = REG_SZ;
	memset (value, 0x0, REG_BUF_LEN*sizeof(BYTE));
	_tcscpy((TCHAR *)value, strDevName);
	cbValue = strDevName.GetLength()*sizeof(TCHAR);
	if(reg.RegSetValue(hCardKey, REG_DEVICE_NAME_VALUE, dwType, value, cbValue) != REG_ERROR_SUCCESS)
	{
		reg.RegCloseKey(hTypeKey);
		reg.RegCloseKey(hCardKey);
		return DEVICEIO_ERROR_REGISTRY_OPERATION_FAIL;
	}
	reg.RegCloseKey(hCardKey);

	reg.RegCloseKey(hTypeKey);
	m_deviceArray[nDevi].nTotalChannel = nChannelNum;
	m_deviceArray[nDevi].strCardName = strDevName;

	return DEVICEIO_ERROR_SUCCESS;
}
