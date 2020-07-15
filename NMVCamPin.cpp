#include "stdafx.h"

NMVCamPin::NMVCamPin(HRESULT *phr, NMVCamSource *pFilter) : CSourceStream(NAME("NMVCamPin"), phr, pFilter, OUTPUT_PIN_NAME)
	,m_pFilter(pFilter)
	,m_Count(0), m_rtFrameLength(666666)
	,m_BmpData(NULL), m_Hdc(NULL), m_Bitmap(NULL)
{
	GetMediaType(&m_mt);
}

NMVCamPin::~NMVCamPin() {
	if (m_Bitmap) {
		DeleteObject(m_Bitmap);
		m_Bitmap = NULL;
	}
	if (m_Hdc) {
		DeleteDC(m_Hdc);
		m_Hdc = NULL;
	}
	if (m_BmpData) {
		delete m_BmpData;
		m_BmpData = NULL;
	}
}

STDMETHODIMP NMVCamPin::Notify(IBaseFilter *pSelf, Quality q) {
	return E_NOTIMPL;
}

BOOL CALLBACK EnumWndProc(HWND hWnd, LPARAM lParam) {
	WCHAR buff[256] = L"";
	SendMessageTimeoutW(hWnd, WM_GETTEXT, sizeof(buff), (LPARAM)buff, SMTO_BLOCK, 100, NULL);
	if (wcscmp(buff, ((WindowCell *)lParam)->name) == 0) {
		((WindowCell *)lParam)->window = hWnd;
		return FALSE;
	}
	return TRUE;
}

HRESULT NMVCamPin::GetMediaType(CMediaType *pMediaType) {
	HRESULT hr=NOERROR;
	VIDEOINFO *pvi=(VIDEOINFO *)pMediaType->AllocFormatBuffer(sizeof(VIDEOINFO));
	ZeroMemory(pvi, sizeof(VIDEOINFO));

	pvi->AvgTimePerFrame=m_rtFrameLength;

	BITMAPINFOHEADER *pBmi=&(pvi->bmiHeader);
	pBmi->biSize=sizeof(BITMAPINFOHEADER);
	pBmi->biWidth = WINDOW_WIDTH;
	pBmi->biHeight = WINDOW_HEIGHT;
	pBmi->biPlanes=1;
	pBmi->biBitCount=PIXEL_BIT;
	pBmi->biCompression=BI_RGB;
	pvi->bmiHeader.biSizeImage=DIBSIZE(pvi->bmiHeader);

	SetRectEmpty(&(pvi->rcSource));
	SetRectEmpty(&(pvi->rcTarget));

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetFormatType(&FORMAT_VideoInfo);

	const GUID subtype=GetBitmapSubtype(&pvi->bmiHeader);
	pMediaType->SetSubtype(&subtype);

	pMediaType->SetTemporalCompression(FALSE);
	const int bmpsize=DIBSIZE(*pBmi);
	pMediaType->SetSampleSize(bmpsize);
	if(m_BmpData) delete m_BmpData;
	m_BmpData=new DWORD[pBmi->biWidth * pBmi->biHeight];
	memset(m_BmpData,0,pMediaType->GetSampleSize());
	
	HDC dwhdc=GetDC(GetDesktopWindow());
	m_Bitmap=
		CreateDIBitmap(dwhdc, pBmi, CBM_INIT, m_BmpData, (BITMAPINFO*)pBmi, DIB_RGB_COLORS);
	
	if (m_Hdc) {
		DeleteDC(m_Hdc);
		m_Hdc = NULL;
	}
	m_Hdc=CreateCompatibleDC(dwhdc);
	SelectObject(m_Hdc, m_Bitmap);
	ReleaseDC(GetDesktopWindow(), dwhdc);
	
	return hr;
}
HRESULT NMVCamPin::CheckMediaType(const CMediaType *pMediaType) {
	HRESULT hr=NOERROR;
	CheckPointer(pMediaType,E_POINTER);
	CMediaType mt;
	GetMediaType(&mt);
	if(mt==*pMediaType) {
		FreeMediaType(mt);
		return S_OK;
	}
	FreeMediaType(mt);
	return E_FAIL;
}

HRESULT NMVCamPin::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pRequest) {
	HRESULT hr=NOERROR;
	VIDEOINFO *pvi=reinterpret_cast<VIDEOINFO*>(m_mt.Format());
	ASSERT(pvi != NULL);
	pRequest->cBuffers=1;
	// バッファサイズはビットマップ1枚分以上である必要がある
	if(pvi->bmiHeader.biSizeImage > (DWORD)pRequest->cbBuffer) {
		pRequest->cbBuffer=pvi->bmiHeader.biSizeImage;
	}
	// アロケータプロパティを設定しなおす
	ALLOCATOR_PROPERTIES Actual;
	hr=pAlloc->SetProperties(pRequest, &Actual);
	if(FAILED(hr)) {
		return hr;
	}
	if(Actual.cbBuffer < pRequest->cbBuffer) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT NMVCamPin::FillBuffer(IMediaSample *pSample) {
	HRESULT hr=E_FAIL;
	CheckPointer(pSample,E_POINTER);
	// ダウンストリームフィルタが
	// フォーマットを動的に変えていないかチェック
	ASSERT(m_mt.formattype == FORMAT_VideoInfo);
	ASSERT(m_mt.cbFormat >= sizeof(VIDEOINFOHEADER));
	// フレームに書き込み
	LPBYTE pSampleData=NULL;
	const long size=pSample->GetSize();
	pSample->GetPointer(&pSampleData);

	TCHAR buffer[200];
	CRefTime ref;
	m_pFilter->StreamTime(ref);
	IReferenceClock *clock;
	m_pFilter->GetSyncSource(&clock);
	REFERENCE_TIME stime;
	clock->GetTime(&stime);

	WindowCell wc;
	wc.window = NULL;
	wcscpy(wc.name, TARGET_WINDOW_NAME);
	EnumWindows(EnumWndProc, (LPARAM)&wc);
	
	if (wc.window != NULL) {
		HDC windowDC = GetDC(wc.window);
		BitBlt(m_Hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			windowDC, 0, 0, SRCCOPY);
		ReleaseDC(wc.window, windowDC);
	}
	else {
		_snwprintf_s(buffer, _countof(buffer), _TRUNCATE, TEXT("NO SIGNAL"));
		TextOut(m_Hdc, 0, 0, buffer, lstrlen(buffer));
	}

	clock->Release();

	VIDEOINFO *pvi=(VIDEOINFO *)m_mt.Format();

	GetDIBits(m_Hdc, m_Bitmap, 0, WINDOW_HEIGHT,
		pSampleData, (BITMAPINFO*)&pvi->bmiHeader, DIB_RGB_COLORS);

	const REFERENCE_TIME delta=m_rtFrameLength;
	REFERENCE_TIME start_time=ref;
	FILTER_STATE state;
	m_pFilter->GetState(0, &state);
	if(state==State_Paused)
		start_time=0;
	REFERENCE_TIME end_time=(start_time+delta);
	pSample->SetTime(&start_time, &end_time);
	pSample->SetActualDataLength(size);
	pSample->SetSyncPoint(TRUE);
	m_Count++;

	return S_OK;
}


