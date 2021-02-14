#include <cassert>
#include <dshow.h>    // DirectShow (Windows SDK)
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <windows.h>

// https://programmersought.com/article/50006800825/
#pragma comment(lib, "strmiids")
#pragma comment(lib, "winmm.lib")
#ifdef _DEBUG
#pragma comment(lib, "strmbasd.lib")
#else
#pragma comment(lib, "STRMBASE.lib")
#endif

template<typename T>
void SafeRelease(T** ppT)
{
    if(*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}


HRESULT EnumerateDevices(REFGUID category, IEnumMoniker** ppEnum)
{
    // Create the System Device Enumerator.
    ICreateDevEnum* pDevEnum;
    HRESULT         hr =
      CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if(SUCCEEDED(hr))
    {
        // Create an enumerator for the category.
        hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
        if(hr == S_FALSE)
        {
            hr = VFW_E_NOT_FOUND;    // The category is empty. Treat as an error.
        }
        pDevEnum->Release();
    }
    return hr;
}

void DisplayDeviceInformation(IEnumMoniker* pEnum)
{
    IMoniker* pMoniker = NULL;

    while(pEnum->Next(1, &pMoniker, NULL) == S_OK)
    {
        IPropertyBag* pPropBag;
        HRESULT       hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
        if(FAILED(hr))
        {
            pMoniker->Release();
            continue;
        }

        VARIANT var;
        VariantInit(&var);

        // Get description or friendly name.
        hr = pPropBag->Read(L"Description", &var, 0);
        if(FAILED(hr))
        {
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
        }
        if(SUCCEEDED(hr))
        {
            printf("%S\n", var.bstrVal);
            VariantClear(&var);
        }

        hr = pPropBag->Write(L"FriendlyName", &var);

        // WaveInID applies only to audio capture devices.
        hr = pPropBag->Read(L"WaveInID", &var, 0);
        if(SUCCEEDED(hr))
        {
            printf("WaveIn ID: %d\n", var.lVal);
            VariantClear(&var);
        }

        hr = pPropBag->Read(L"DevicePath", &var, 0);
        if(SUCCEEDED(hr))
        {
            // The device path is not intended for display.
            printf("Device path: %S\n", var.bstrVal);
            VariantClear(&var);
        }
        pPropBag->Release();
        pMoniker->Release();
    }
}

// Query whether a pin is connected to another pin.
//
// Note: This function does not return a pointer to the connected pin.
HRESULT IsPinConnected(IPin* pPin, BOOL* pResult)
{
    IPin*   pTmp = NULL;
    HRESULT hr   = pPin->ConnectedTo(&pTmp);
    if(SUCCEEDED(hr))
    {
        *pResult = TRUE;
    }
    else if(hr == VFW_E_NOT_CONNECTED)
    {
        // The pin is not connected. This is not an error for our purposes.
        *pResult = FALSE;
        hr       = S_OK;
    }

    SafeRelease(&pTmp);
    return hr;
}

// Query whether a pin has a specified direction (input / output)
HRESULT IsPinDirection(IPin* pPin, PIN_DIRECTION dir, BOOL* pResult)
{
    PIN_DIRECTION pinDir;
    HRESULT       hr = pPin->QueryDirection(&pinDir);
    if(SUCCEEDED(hr))
    {
        *pResult = (pinDir == dir);
    }
    return hr;
}

// Match a pin by pin direction and connection state.
HRESULT MatchPin(IPin* pPin, PIN_DIRECTION direction, BOOL bShouldBeConnected, BOOL* pResult)
{
    assert(pResult != NULL);

    BOOL bMatch       = FALSE;
    BOOL bIsConnected = FALSE;

    HRESULT hr = IsPinConnected(pPin, &bIsConnected);
    if(SUCCEEDED(hr))
    {
        if(bIsConnected == bShouldBeConnected)
        {
            hr = IsPinDirection(pPin, direction, &bMatch);
        }
    }

    if(SUCCEEDED(hr))
    {
        *pResult = bMatch;
    }
    return hr;
}

// Return the first unconnected input pin or output pin.
HRESULT FindUnconnectedPin(IBaseFilter* pFilter, PIN_DIRECTION PinDir, IPin** ppPin)
{
    IEnumPins* pEnum  = NULL;
    IPin*      pPin   = NULL;
    BOOL       bFound = FALSE;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr))
    {
        goto done;
    }

    while(S_OK == pEnum->Next(1, &pPin, NULL))
    {
        hr = MatchPin(pPin, PinDir, FALSE, &bFound);
        if(FAILED(hr))
        {
            goto done;
        }
        if(bFound)
        {
            *ppPin = pPin;
            (*ppPin)->AddRef();
            break;
        }
        SafeRelease(&pPin);
    }

    if(!bFound)
    {
        hr = VFW_E_NOT_FOUND;
    }

done:
    SafeRelease(&pPin);
    SafeRelease(&pEnum);
    return hr;
}

// Connect output pin to filter.
HRESULT ConnectFilters(IGraphBuilder* pGraph,    // Filter Graph Manager.
                       IPin*          pOut,      // Output pin on the upstream filter.
                       IBaseFilter*   pDest)       // Downstream filter.
{
    IPin* pIn = NULL;

    // Find an input pin on the downstream filter.
    HRESULT hr = FindUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
    if(SUCCEEDED(hr))
    {
        // Try to connect them.
        hr = pGraph->Connect(pOut, pIn);
        pIn->Release();
    }
    return hr;
}

// Create a filter by CLSID and add it to the graph.
HRESULT AddFilterByCLSID(IGraphBuilder* pGraph,    // Pointer to the Filter Graph Manager.
                         REFGUID        clsid,     // CLSID of the filter to create.
                         IBaseFilter**  ppF,       // Receives a pointer to the filter.
                         LPCWSTR        wszName    // A name for the filter (can be NULL).
)
{
    *ppF = 0;

    IBaseFilter* pFilter = NULL;

    HRESULT hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFilter));
    if(FAILED(hr))
    {
        goto done;
    }

    hr = pGraph->AddFilter(pFilter, wszName);
    if(FAILED(hr))
    {
        goto done;
    }

    *ppF = pFilter;
    (*ppF)->AddRef();

done:
    SafeRelease(&pFilter);
    return hr;
}
//
//// {3C78B8E2-6C4D-11d1-ADE2-0000F8754B99}
// static const GUID CLSID_WavDest = {0x3c78b8e2,
//                                   0x6c4d,
//                                   0x11d1,
//                                   {0xad, 0xe2, 0x0, 0x0, 0xf8, 0x75, 0x4b, 0x99}};
//
// int main()
//{
//    HRESULT       hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
//    IEnumMoniker* pEnum;
//    if(SUCCEEDED(hr))
//    {
//
//        hr = EnumerateDevices(CLSID_AudioInputDeviceCategory, &pEnum);
//        if(SUCCEEDED(hr))
//        {
//            DisplayDeviceInformation(pEnum);
//        }
//    }
//    {
//        IBaseFilter *    pSrc = NULL, *pWaveDest = NULL, *pWriter = NULL;
//        IFileSinkFilter* pSink = NULL;
//        IGraphBuilder*   pGraph;
//
//        // Create the Filter Graph Manager.
//        HRESULT hr = CoCreateInstance(
//          CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
//
//        // This example omits error handling.
//
//        // Not shown: Use the System Device Enumerator to create the
//        // audio capture filter.
//
//        // Add the audio capture filter to the filter graph.
//        hr = pGraph->AddFilter(pSrc, L"Capture");
//
//        // Add the WavDest and the File Writer.
//        hr = AddFilterByCLSID(pGraph, CLSID_WavDest, L"WavDest", &pWaveDest);
//        hr = AddFilterByCLSID(pGraph, CLSID_FileWriter, L"File Writer", &pWriter);
//
//        // Set the file name.
//        hr = pWriter->QueryInterface(IID_IFileSinkFilter, (void**)&pSink);
//        hr = pSink->SetFileName(L"C:\\MyWavFile.wav", NULL);
//
//        // Connect the filters.
//        hr = ConnectFilters(pGraph, pSrc, pWaveDest);
//        hr = ConnectFilters(pGraph, pWaveDest, pWriter);
//    }
//    // Not shown: Release interface pointers.
//    CoUninitialize();
//    pEnum->Release();
//
//    return 0;
//}

/// AddFilterToGraph
HRESULT AddFilterToGraph(IGraphBuilder* pGraph,
                         const GUID&    clsId,
                         LPCWSTR        wstrName,
                         IBaseFilter**  pFilter)
{
    if(!pGraph || !pFilter)
    {
        return E_POINTER;
    }

    *pFilter = NULL;

    // CreateFilter
    IBaseFilter* pBaseFilter = NULL;
    HRESULT      hr =
      CoCreateInstance(clsId, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pBaseFilter);
    if(SUCCEEDED(hr))
    {
        // AddFilterToGraph
        hr = pGraph->AddFilter(pBaseFilter, wstrName);
        if(SUCCEEDED(hr))
        {
            // SaveFilter
            *pFilter = pBaseFilter;
        }
        else
        {
            // FreeFilter
            pBaseFilter->Release();
        }
    }
    return hr;
}

/// GetFilterDirectionPin
HRESULT GetUnconnectedPin(IBaseFilter* pFilter, PIN_DIRECTION pinDirection, IPin** ppPin)
{
    if(!pFilter || !ppPin)
        return E_POINTER;
    // Get pin enumerator
    *ppPin              = NULL;
    IEnumPins* pEnumPin = NULL;
    HRESULT    hr       = pFilter->EnumPins(&pEnumPin);
    if(FAILED(hr))
    {
        return hr;
    }
    // Traverse the pin enumerator
    IPin* pPin = NULL;
    while(pEnumPin->Next(1, &pPin, NULL) == S_OK)
    {
        // Check pin direction
        PIN_DIRECTION curPinDirection;
        pPin->QueryDirection(&curPinDirection);
        if(curPinDirection == pinDirection)
        {
            IPin* tmpPin = NULL;
            // Try to connect
            hr = pPin->ConnectedTo(&tmpPin);
            if(SUCCEEDED(hr))
            {
                // Can be connected
                tmpPin->Release();
            }
            else
            {
                pEnumPin->Release();
                *ppPin = pPin;
                return S_OK;
            }
        }
    }
    // Release the pin enumerator
    pEnumPin->Release();
    return E_FAIL;
}

/// FilterConnectedToFilter
// PinToFilterPinConnected
HRESULT ConnectedFilters(IGraphBuilder* pGraph, IPin* pOutPin, IBaseFilter* pDstFilter)
{
    if(!pGraph || !pOutPin || !pDstFilter)
        return E_POINTER;
    // Get input pin
    IPin*   pInPin = NULL;
    HRESULT hr     = GetUnconnectedPin(pDstFilter, PINDIR_INPUT, &pInPin);
    if(FAILED(hr))
        return hr;
    // Connect input pin
    hr = pGraph->Connect(pOutPin, pInPin);
    pInPin->Release();
    return hr;
}

// FilterToFilterConnected
HRESULT ConnectedFilters(IGraphBuilder* pGraph, IBaseFilter* pSrcFilter, IBaseFilter* pDstFilter)
{
    if(!pGraph || !pSrcFilter || !pDstFilter)
        return E_POINTER;
    // Get input pin
    IPin*   pOutPin = NULL;
    HRESULT hr      = GetUnconnectedPin(pSrcFilter, PINDIR_OUTPUT, &pOutPin);
    if(FAILED(hr))
        return hr;
    // Connect input pin
    hr = ConnectedFilters(pGraph, pOutPin, pDstFilter);
    pOutPin->Release();
    return hr;
}

HRESULT EnumDeviceInfo(std::vector<std::wstring>&         devicesNames,
                       std::map<std::wstring, IMoniker*>& devicesInfo,
                       const IID                          InputDeviceCategory)
{
    // Create system device enumerator
    ICreateDevEnum* pSystemDevEnum = NULL;
    HRESULT         hr             = CoCreateInstance(CLSID_SystemDeviceEnum,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_ICreateDevEnum,
                                  (void**)&pSystemDevEnum);
    if(FAILED(hr))
    {
        return hr;
    }
    // Create a device enumerator
    IEnumMoniker* pEnumMoniker = NULL;
    hr = pSystemDevEnum->CreateClassEnumerator(InputDeviceCategory, &pEnumMoniker, 0);
    if(hr == S_OK)
    {
        IMoniker* pMoniker = NULL;
        ULONG     cFetched;
        while(pEnumMoniker->Next(1, &pMoniker, &cFetched) == S_OK)
        {
            IPropertyBag* pPropertyBag = NULL;
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropertyBag);
            if(SUCCEEDED(hr))
            {
                // Get device information
                VARIANT varName;
                VariantInit(&varName);
                hr = pPropertyBag->Read(L"FriendlyName", &varName, 0);    // Get friendly name
                if(SUCCEEDED(hr))
                {
                    // Save device information
                    devicesNames.push_back(varName.bstrVal);
                    devicesInfo.insert(
                      std::pair<std::wstring, IMoniker*>(varName.bstrVal, pMoniker));
                }
                VariantClear(&varName);
            }
            else
            {
                return hr;
            }
            pPropertyBag->Release();
        }
    }
    pEnumMoniker->Release();
    pSystemDevEnum->Release();
    return hr;
}

HRESULT FindAudioCaptureFilter(IMoniker* pMoniker, IBaseFilter** pFilter)
{
    if(!pFilter || !pMoniker)
    {
        return E_POINTER;
    }

    *pFilter   = NULL;
    HRESULT hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)pFilter);
    if(FAILED(hr))
    {
        return E_FAIL;
    }
    return S_OK;
}

// Global data
CFactoryTemplate g_Templates[] = {
  {L"WAV Dest", &CLSID_WavDest, CWavDestFilter::CreateInstance, NULL, &sudWavDest},
};

int main()
{
    // https://www.codeproject.com/Articles/31022/A-simple-console-audio-input-device-reader
    HRESULT         hr;                               // COM result
    ICreateDevEnum* pDeviceEnum         = nullptr;    // Create the System Device Enumerator.
    IEnumMoniker*   pEnumCat            = nullptr;    // Device enumeration moniker
    IGraphBuilder*  pGraph              = nullptr;
    IBaseFilter*    pAudioCaptureFilter = nullptr;
    IBaseFilter*    pDestWavFilter      = nullptr;
    IBaseFilter*    pWriteFilter        = nullptr;

    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    hr = CoCreateInstance(CLSID_SystemDeviceEnum,
                          nullptr,
                          CLSCTX_INPROC_SERVER,
                          IID_ICreateDevEnum,
                          (void**)&pDeviceEnum);
    hr = CoCreateInstance(
      CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
    if(FAILED(hr))
    {
        std::cout << "IFilterGraph failed!" << std::endl;
        return 0;
    }

    hr = pDeviceEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEnumCat, 0);


    std::vector<std::wstring>         audioDeviceNamesVector;
    std::map<std::wstring, IMoniker*> audioDeviceFriendlyMap;
    EnumDeviceInfo(audioDeviceNamesVector, audioDeviceFriendlyMap, CLSID_AudioInputDeviceCategory);

    hr = FindAudioCaptureFilter(audioDeviceFriendlyMap[audioDeviceNamesVector[0]],
                                &pAudioCaptureFilter);
    if(FAILED(hr))
    {
        std::cout << "FindAudioCaptureFilter  failed!" << std::endl;
        return 0;
    }

    pGraph->AddFilter(pAudioCaptureFilter, L"AudioCaptureFilter");
    /// WavDestFilter
    pDestWavFilter = new CWavDestFilter(nullptr, &hr);
    if(FAILED(hr))
    {
        MessageBox(nullptr, L"WavDestFilter failed!", L"ErrWnd", nullptr);
        return 0;
    }
    pGraph->AddFilter(pDestWavFilter, L"WavDestFilter");
    /// WriteFilter
    hr = AddFilterToGraph(pGraph, CLSID_FileWriter, L"WriteFilter", &pWriteFilter);
    if(FAILED(hr))
    {
        MessageBox(nullptr, L"WriteFilter failed!", L"ErrWnd", nullptr);
        return 0;
    }
    IFileSinkFilter* pFileSinkFilter;
    hr = pWriteFilter->QueryInterface(IID_IFileSinkFilter, (void**)&pFileSinkFilter);
    if(FAILED(hr))
    {
        MessageBox(nullptr, L"IFileSinkFilter failed!", L"ErrWnd", nullptr);
        return 0;
    }
    pFileSinkFilter->SetFileName(L"E:\\TestFile\\AudioCapture.wav", nullptr);
    /// ConnectedFilters
    ConnectedFilters(pGraph, pAudioCaptureFilter, pDestWavFilter);
    ConnectedFilters(pGraph, pDestWavFilter, pWriteFilter);
    /// MediaControl
    IMediaControl* pControl = nullptr;
    hr                      = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
    if(FAILED(hr))
    {
        MessageBox(nullptr, L"QueryInterface IMediaControl Failed!", L"ErrWnd", nullptr);
        return 0;
    }
    //
    hr = pControl->Run();
    if(SUCCEEDED(hr))
    {
        Sleep(10000);
        pControl->Stop();
    }

    /// COM_EndÅF
    ::CoUninitialize();

    return 1;
}
