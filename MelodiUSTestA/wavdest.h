#pragma once
//------------------------------------------------------------------------------
// File: WavDest.h
//
// Desc: DirectShow sample code - header file for WAV audio file writer filter.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------
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

#pragma warning(disable : 4097 4511 4512 4514 4705)



class CWavDestOutputPin : public CTransformOutputPin
{
public:
    CWavDestOutputPin(CTransformFilter* pFilter, HRESULT* phr);

    STDMETHODIMP EnumMediaTypes(IEnumMediaTypes** ppEnum);
    HRESULT      CheckMediaType(const CMediaType* pmt);
};

class CWavDestFilter : public CTransformFilter
{

public:
    DECLARE_IUNKNOWN;

    CWavDestFilter(LPUNKNOWN pUnk, HRESULT* pHr);
    ~CWavDestFilter();
    static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk, HRESULT* pHr);

    HRESULT Transform(IMediaSample* pIn, IMediaSample* pOut);
    HRESULT Receive(IMediaSample* pSample);

    HRESULT CheckInputType(const CMediaType* mtIn);
    HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut);
    HRESULT GetMediaType(int iPosition, CMediaType* pMediaType);

    HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* pProperties);

    HRESULT StartStreaming();
    HRESULT StopStreaming();

    HRESULT CompleteConnect(PIN_DIRECTION direction, IPin* pReceivePin)
    {
        return S_OK;
    }

private:
    HRESULT Copy(IMediaSample* pSource, IMediaSample* pDest) const;
    HRESULT Transform(IMediaSample* pMediaSample);
    HRESULT Transform(AM_MEDIA_TYPE* pType, const signed char ContrastLevel) const;

    ULONG m_cbWavData;
    ULONG m_cbHeader;
};