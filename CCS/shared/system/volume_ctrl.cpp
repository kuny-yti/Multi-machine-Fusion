
#include "volume_ctrl.h"

const IID IID_IDeviceTopology = __uuidof(IDeviceTopology);
const IID IID_IPart = __uuidof(IPart);
const IID IID_IConnector = __uuidof(IConnector);
const IID IID_IAudioInputSelector = __uuidof(IAudioInputSelector);


volume_ctrl::volume_ctrl(void)
:m_pEndptVolCtrl(NULL),
m_pEnumerator(NULL),
m_pDevice(NULL),
m_pVolumeLevel(NULL),
m_guidMyContext(GUID_NULL)
{
	VMin = 0.0;
	VMax = 0.0;
	VStep =0.0;

	m_eWinType = vGetWindowsType();
    //TRACE("Now windows is %d", m_eWinType);
	
	if(WindowsVista == m_eWinType|| Windows7 == m_eWinType)
	{
		vInitEndPoint(1);		
	}

}

volume_ctrl::~volume_ctrl(void)
{
	if(WindowsVista == m_eWinType|| Windows7 == m_eWinType)
	{
		vUninitEndPoint();
	}

	m_eWinType = UnknownOS;
}

int volume_ctrl::GetMaxVol()
{
	return MAX_VOL;
}

int volume_ctrl::GetMinVol()
{
	return 0;
}

int volume_ctrl::GetVolume(int nType)
{
	int nVol = 0;
	DWORD dwNowMaxVol = GetMaxVol();

	if(m_eWinType == WindowsVista || m_eWinType == Windows7)
	{
			DWORD dwNewVol = 0;
            vVolumeGet(nType, &dwNewVol);
			nVol = dwNewVol;
	}

	return nVol;
}

BOOL volume_ctrl::SetVolume(int nType,int nVol)
{
	BOOL bSet = FALSE;
	DWORD dwNowMaxVol = GetMaxVol();

	if(m_eWinType == WindowsVista || m_eWinType == Windows7)
	{
			int nWin7SetVol = nVol ;
            bSet = vVolumeSet(nType, nWin7SetVol);
	}

	return bSet;
}

BOOL volume_ctrl::SetMute(int nType,BOOL bMute)
{
	BOOL bSet = FALSE;

	if(m_eWinType == WindowsVista || m_eWinType == Windows7)
	{
        bSet = vMuteSet(nType, bMute);
	}

	return bSet;
}

BOOL volume_ctrl::GetMute(int nType)
{
	BOOL bMute = FALSE;

	if(m_eWinType == WindowsVista || m_eWinType == Windows7)
	{
		bMute  = vMuteGet(nType);
	}
	
	return bMute;
}


BOOL volume_ctrl::vInitEndPoint(int nType)
{
	if(m_pEndptVolCtrl != NULL)
		return FALSE;

	//g_EPVolEvents.SetCallback(fCallback); //set callback

	HRESULT hr = S_OK;

	CoInitialize(NULL);

	hr = CoCreateGuid(&m_guidMyContext);
	if(FAILED(hr))
		return FALSE;

	// Get enumerator for audio endpoint devices.
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IMMDeviceEnumerator),
		(void**)&m_pEnumerator);
	if(FAILED(hr))
		return FALSE;

	// Get default audio-rendering or audio-capture device.
	if(nType == 0)
		hr = m_pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_pDevice);
	else
		hr = m_pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &m_pDevice);

	if(FAILED(hr))
		return FALSE;

	hr = m_pDevice->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_ALL, NULL, (void**)&m_pEndptVolCtrl);

	
	if(FAILED(hr))
		return FALSE;

	/*if(g_pEndptVolCtrl)
	hr = g_pEndptVolCtrl->RegisterControlChangeNotify(
	(IAudioEndpointVolumeCallback*)&g_EPVolEvents);*/
	if(FAILED(hr))
		return FALSE;


}


BOOL volume_ctrl::vMuteSet(int nType,BOOL bMute)
{
	if(m_pEndptVolCtrl)
		vUninitEndPoint();
	
	vInitEndPoint(nType);
	
	if(m_pEndptVolCtrl)
		m_pEndptVolCtrl->SetMute(bMute, &m_guidMyContext);

	return TRUE;
}

BOOL volume_ctrl::vMuteGet(int nType)
{
	if(m_pEndptVolCtrl)
		vUninitEndPoint();

	vInitEndPoint(nType);

	BOOL bMute = FALSE;
	if(m_pEndptVolCtrl)
		m_pEndptVolCtrl->GetMute(&bMute);
	return bMute;
}

BOOL volume_ctrl::vVolumeSet(int nType,DWORD dwVolume)
{
	if(m_pEndptVolCtrl)
		vUninitEndPoint();

	vInitEndPoint(nType);

	if((int)dwVolume < 0)
		dwVolume = 0;
	if((int)dwVolume > MAX_VOL)
		dwVolume = MAX_VOL;

	if(m_pEndptVolCtrl)
	{
		HRESULT hr = m_pEndptVolCtrl->SetMasterVolumeLevelScalar((float)dwVolume / MAX_VOL, &m_guidMyContext);
		return SUCCEEDED(hr) ? TRUE : FALSE;
	}

	return FALSE;
}

BOOL volume_ctrl::vVolumeGet(int nType,DWORD* pdwVolume)
{
	if(m_pEndptVolCtrl)
		vUninitEndPoint();

	vInitEndPoint(nType);

	float fVolume;

	if(m_pEndptVolCtrl)
	{
		m_pEndptVolCtrl->GetMasterVolumeLevelScalar(&fVolume);
		if(pdwVolume != NULL)
			*pdwVolume = MAX_VOL * fVolume + 0.5;
	}

	return TRUE;
}


BOOL volume_ctrl::vUninitEndPoint()
{
	SAFE_RELEASE(m_pEnumerator);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pEndptVolCtrl);
	CoUninitialize();
	return TRUE;
}


BOOL volume_ctrl::SelectCaptureDevice(IMMDevice *pEndptDev)
{
	HRESULT hr = S_OK;
	DataFlow flow;
	IDeviceTopology *pDeviceTopology = NULL;
	IConnector *pConnFrom = NULL;
	IConnector *pConnTo = NULL;
	IPart *pPartPrev = NULL;
	IPart *pPartNext = NULL;
	IAudioInputSelector *pSelector = NULL;
	IAudioLoudness *pLoudness = NULL;
 
	// Get the endpoint device's IDeviceTopology interface.
	hr = pEndptDev->Activate(
		IID_IDeviceTopology, CLSCTX_ALL, NULL,
		(void**)&pDeviceTopology);
	EXIT_ON_ERROR(hr)

	// The device topology for an endpoint device always
	// contains just one connector (connector number 0).
	hr = pDeviceTopology->GetConnector(0, &pConnFrom);
	SAFE_RELEASE(pDeviceTopology)
	EXIT_ON_ERROR(hr)

	// Make sure that this is a capture device.
	hr = pConnFrom->GetDataFlow(&flow);
	EXIT_ON_ERROR(hr)

	if (flow != Out)
	{
		// Error -- this is a rendering device. 
	 	EXIT_ON_ERROR(hr = E_FAIL) //AUDCLNT_E_WRONG_ENDPOINT_TYPE;
	}

	// Outer loop: Each iteration traverses the data path
	// through a device topology starting at the input
	// connector and ending at the output connector.
	while (TRUE)
	{
		BOOL bConnected;
		hr = pConnFrom->IsConnected(&bConnected);
		EXIT_ON_ERROR(hr)

		// Does this connector connect to another device

		if (!bConnected)
		{

			// This is the end of the data path that
			// stretches from the endpoint device to the
			// system bus or external bus. Verify that
			// the connection type is Software_IO.
			ConnectorType  connType;
			hr = pConnFrom->GetType(&connType);
			EXIT_ON_ERROR(hr)

			if (connType == Software_IO) 
				break;  // finished 

			EXIT_ON_ERROR(hr = E_FAIL)
		} 

		// Get the connector in the next device topology,
		// which lies on the other side of the connection.
		hr = pConnFrom->GetConnectedTo(&pConnTo);
		EXIT_ON_ERROR(hr)
		SAFE_RELEASE(pConnFrom)

		// Get the connector's IPart interface.
		hr = pConnTo->QueryInterface(
		IID_IPart, (void**)&pPartPrev);
		EXIT_ON_ERROR(hr)
		SAFE_RELEASE(pConnTo)

		// Inner loop: Each iteration traverses one link in a
		// device topology and looks for input multiplexers.

		while (TRUE)
		{
			PartType parttype;
			UINT localId;
			IPartsList *pParts;

			// Follow downstream link to next part.
			hr = pPartPrev->EnumPartsOutgoing(&pParts);
			EXIT_ON_ERROR(hr)

			hr = pParts->GetPart(0, &pPartNext);
			pParts->Release();
			EXIT_ON_ERROR(hr)

			hr = pPartNext->GetPartType(&parttype);
			EXIT_ON_ERROR(hr)

			if (parttype == Connector)
			{
				// We've reached the output connector that
				// lies at the end of this device topology.
				hr = pPartNext->QueryInterface(
					IID_IConnector,
					(void**)&pConnFrom);
				EXIT_ON_ERROR(hr)

				SAFE_RELEASE(pPartPrev)
				SAFE_RELEASE(pPartNext)
				break;
			}

			LPWSTR pWcsName = NULL;
			hr = pPartNext->GetName(&pWcsName);
			EXIT_ON_ERROR(hr) 
  
			CString str;
			str.Format(_T("%s"), pWcsName);
			OutputDebugString(str); 
			
			CoTaskMemFree(pWcsName);
			pWcsName = NULL; 

			UINT cCount = 0;
			pPartNext->GetControlInterfaceCount(&cCount); 

			for(UINT index=0; index<cCount; index++)
			{
				IControlInterface* pCtrlItem = NULL;
				hr = pPartNext->GetControlInterface(index, &pCtrlItem);
				if(FAILED(hr))
					break;

				hr = pCtrlItem->GetName(&pWcsName);

				if(str == L"麦克风加强")
				{
					hr = pPartNext->Activate(CLSCTX_ALL,
                              __uuidof(IAudioLoudness),
                              (void**)&pLoudness);

					SAFE_RELEASE(pCtrlItem)

					if(FAILED(hr))
						break;

					CoTaskMemFree(pWcsName);
					pWcsName = NULL; 

					goto Exit;
				}
 

			}

			TRACE0("\n");  
			
			//  HRESULT GetControlInterface(
			//	[in]   UINT nIndex,
			//	[out]  IControlInterface **ppFunction
			//	);

			// Failure of the following call means only that
			// the part is not a MUX (input selector).
			hr = pPartNext->Activate(
				CLSCTX_ALL,
				IID_IAudioInputSelector,
				(void**)&pSelector);

			if (hr == S_OK)
			{
				// We found a MUX (input selector), so select
				// the input from our endpoint device.
				hr = pPartPrev->GetLocalId(&localId);
				EXIT_ON_ERROR(hr)

				hr = pSelector->SetSelection(localId, NULL);
				EXIT_ON_ERROR(hr)

				SAFE_RELEASE(pSelector)
			}

			SAFE_RELEASE(pPartPrev)
			pPartPrev = pPartNext;
			pPartNext = NULL;
		}
	}

Exit:
	SAFE_RELEASE(pConnFrom)
	SAFE_RELEASE(pConnTo)
	SAFE_RELEASE(pPartPrev)
	SAFE_RELEASE(pPartNext)
	SAFE_RELEASE(pSelector)
	return SUCCEEDED(hr);
}

BOOL volume_ctrl::getMicrophoneBoostVolumeLevel(IMMDevice *pEndptDev)
{
	HRESULT hr = S_OK;
	DataFlow flow;
	IDeviceTopology *pDeviceTopology = NULL;
	IConnector *pConnFrom = NULL;
	IConnector *pConnTo = NULL;
	IPart *pPartPrev = NULL;
	IPart *pPartNext = NULL;
	

	std::wstring microphoneBoostName = L"麦克风加强";//if your system language is English,the name is "microphone boost"

	if (pEndptDev == NULL)
	{
		EXIT_ON_ERROR(hr = E_POINTER)
	}
	// Get the endpoint device's IDeviceTopology interface.
	hr = pEndptDev->Activate(
		IID_IDeviceTopology, CLSCTX_ALL, NULL,
		(void**)&pDeviceTopology);
	EXIT_ON_ERROR(hr)
		// The device topology for an endpoint device always
		// contains just one connector (connector number 0).
		hr = pDeviceTopology->GetConnector(0, &pConnFrom);
	SAFE_RELEASE(pDeviceTopology)
		EXIT_ON_ERROR(hr)
		// Make sure that this is a capture device.
		hr = pConnFrom->GetDataFlow(&flow);
	EXIT_ON_ERROR(hr)
		if (flow != Out)
		{
			// Error -- this is a rendering device.
			EXIT_ON_ERROR(hr = AUDCLNT_E_WRONG_ENDPOINT_TYPE)
		}
		// Outer loop: Each iteration traverses the data path
		// through a device topology starting at the input
		// connector and ending at the output connector.
		while (TRUE)
		{
			BOOL bConnected;
			hr = pConnFrom->IsConnected(&bConnected);
			EXIT_ON_ERROR(hr)
            // Does this connector connect to another device?
            if (bConnected == FALSE)
            {
                // This is the end of the data path that
                // stretches from the endpoint device to the
                // system bus or external bus. Verify that
                // the connection type is Software_IO.
                ConnectorType  connType;
                hr = pConnFrom->GetType(&connType);
                EXIT_ON_ERROR(hr)
                    if (connType == Software_IO)
                    {
                        break;  // finished
                    }
                    EXIT_ON_ERROR(hr = E_FAIL)
            }
            // Get the connector in the next device topology,
            // which lies on the other side of the connection.
            hr = pConnFrom->GetConnectedTo(&pConnTo);
            EXIT_ON_ERROR(hr)
                SAFE_RELEASE(pConnFrom)
                // Get the connector's IPart interface.
                hr = pConnTo->QueryInterface(
                IID_IPart, (void**)&pPartPrev);
            EXIT_ON_ERROR(hr)
                SAFE_RELEASE(pConnTo)
                // Inner loop: Each iteration traverses one link in a
                // device topology and looks for input multiplexers.
                while (TRUE)
                {
                    PartType parttype;
                    IPartsList *pParts;
                    // Follow downstream link to next part.
                    hr = pPartPrev->EnumPartsOutgoing(&pParts);
                    EXIT_ON_ERROR(hr)
                        hr = pParts->GetPart(0, &pPartNext);
                    pParts->Release();
                    EXIT_ON_ERROR(hr)
                        hr = pPartNext->GetPartType(&parttype);
                    EXIT_ON_ERROR(hr)

                        LPWSTR pName;
                        if (SUCCEEDED(pPartNext->GetName(&pName)))
                        {
                            // Failure of the following call means only that
                            // the part is not a boost (micrphone boost).
                            if (microphoneBoostName.compare(pName) == 0)
                            {
                                //get IAudioVolumeLevel to control volume
                                hr = pPartNext->Activate(CLSCTX_ALL, __uuidof(IAudioVolumeLevel), (void**)&m_pVolumeLevel);
                                //获得麦克风加强取值范围
                                GetMicLevelRange();
                                goto Exit;
                            }
                            CoTaskMemFree(pName);
                        }
                        GUID subType;
                        pPartNext->GetSubType(&subType);
                        if (parttype == Connector)
                        {
                            // We've reached the output connector that
                            // lies at the end of this device topology.
                            hr = pPartNext->QueryInterface(
                                IID_IConnector,
                                (void**)&pConnFrom);
                            EXIT_ON_ERROR(hr)
                            SAFE_RELEASE(pPartPrev)
                            SAFE_RELEASE(pPartNext)
                                break;
                        }
                        SAFE_RELEASE(pPartPrev)
                        pPartPrev = pPartNext;
                        pPartNext = NULL;
                }
		}
Exit:
		SAFE_RELEASE(pConnFrom)
		SAFE_RELEASE(pConnTo)
		SAFE_RELEASE(pPartPrev)
		SAFE_RELEASE(pPartNext)
		return SUCCEEDED(hr);
}


BOOL volume_ctrl::GetMicLevelRange()
{
	if(m_pVolumeLevel)
	{
		m_pVolumeLevel->GetLevelRange(0,&VMin,&VMax,&VStep);
	}

	return TRUE;
}

BOOL volume_ctrl::SetMicLevel(float fValue)
{
	
	if(m_pVolumeLevel)
	{
		m_pVolumeLevel->SetLevel(0,fValue,NULL);
	}

	return TRUE;
}

WindowsType volume_ctrl::vGetWindowsType()
{
	OSVERSIONINFO info;
	ZeroMemory(&info, sizeof(OSVERSIONINFO));
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	WindowsType eType = UnknownOS;

	if(GetVersionEx(&info))
	{
        //TRACE("GetVersionEx: MajorVer = %d, MinVer = %d, BuildNo = %d", info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber);
		
		if(info.dwMajorVersion <= 5)//XP or early
		{
			eType = WindowsXP;
		}
		else
		{
			if(info.dwMinorVersion == 0)
			{
				eType = WindowsVista;
			}
			else
			{
				eType = Windows7;
			}
		}
	
	}
	return eType;
}





 
