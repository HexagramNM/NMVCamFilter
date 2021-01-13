#include "stdafx.h"

//IUnknown
HRESULT NMVCamPin::QueryInterface(REFIID   riid,
	LPVOID * ppvObj)
{

	if (riid == _uuidof(IAMStreamConfig)) {
		*ppvObj = (IAMStreamConfig *)this;
	}
	else if (riid == _uuidof(IKsPropertySet)) {
		*ppvObj = (IKsPropertySet *)this;
	}
	else {
		return CSourceStream::QueryInterface(riid, ppvObj);
	}
	AddRef();
	return S_OK;
}

ULONG NMVCamPin::AddRef()
{
	return GetOwner()->AddRef();
}
ULONG NMVCamPin::Release()
{
	return GetOwner()->Release();
}
//IUnknown‚±‚±‚Ü‚Å

//IKsPropertySet
HRESULT NMVCamPin::Get(
	REFGUID PropSet,
	ULONG   Id,
	LPVOID  InstanceData,
	ULONG   InstanceLength,
	LPVOID  PropertyData,
	ULONG   DataLength,
	ULONG   *BytesReturned
)
{
	if (PropSet != AMPROPSETID_Pin) return E_PROP_SET_UNSUPPORTED;
	if (Id != AMPROPERTY_PIN_CATEGORY) return E_PROP_ID_UNSUPPORTED;
	if (PropertyData == NULL && BytesReturned == NULL) return E_POINTER;

	if (BytesReturned) *BytesReturned = sizeof(GUID);
	if (PropertyData == NULL) return S_OK; // Caller just wants to know the size. 
	if (DataLength < sizeof(GUID)) return E_UNEXPECTED; // The buffer is too small.

	*(GUID *)PropertyData = PIN_CATEGORY_CAPTURE;
	return S_OK;
}

HRESULT NMVCamPin::Set(
	REFGUID PropSet,
	ULONG   Id,
	LPVOID  InstanceData,
	ULONG   InstanceLength,
	LPVOID  PropertyData,
	ULONG   DataLength
)
{
	return E_NOTIMPL;
}

HRESULT NMVCamPin::QuerySupported(
	REFGUID PropSet,
	ULONG   Id,
	ULONG   *TypeSupport
)
{
	if (PropSet != AMPROPSETID_Pin) return E_PROP_SET_UNSUPPORTED;
	if (Id != AMPROPERTY_PIN_CATEGORY) return E_PROP_ID_UNSUPPORTED;
	//We support getting this property, but not setting it.
	if (TypeSupport) *TypeSupport = KSPROPERTY_SUPPORT_GET;

	return S_OK;
}
//IKsPropertySet‚±‚±‚Ü‚Å

//IAMStreamConfig
HRESULT NMVCamPin::GetFormat(
	AM_MEDIA_TYPE **ppmt
)
{
	*ppmt = CreateMediaType(&m_mt);
	return S_OK;
}

HRESULT NMVCamPin::GetNumberOfCapabilities(
	int *piCount,
	int *piSize
)
{
	*piCount = 1;
	*piSize = sizeof(VIDEO_STREAM_CONFIG_CAPS);

	return S_OK;
}

HRESULT NMVCamPin::GetStreamCaps(
	int           iIndex,
	AM_MEDIA_TYPE **ppmt,
	BYTE          *pSCC
)
{
	*ppmt = CreateMediaType(&m_mt);
	VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)(*ppmt)->pbFormat;

	pvi->bmiHeader.biCompression = BI_RGB;
	pvi->bmiHeader.biBitCount = PIXEL_BIT;
	pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pvi->bmiHeader.biWidth = WINDOW_WIDTH;
	pvi->bmiHeader.biHeight = WINDOW_HEIGHT;
	pvi->bmiHeader.biPlanes = 1;
	pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
	pvi->bmiHeader.biClrImportant = 0;

	SetRectEmpty(&(pvi->rcSource));
	SetRectEmpty(&(pvi->rcTarget));

	(*ppmt)->majortype = (const GUID)(*sudPinTypes.clsMajorType);
	(*ppmt)->subtype = (const GUID)(*sudPinTypes.clsMinorType);
	(*ppmt)->formattype = FORMAT_VideoInfo;
	(*ppmt)->bTemporalCompression = FALSE;
	(*ppmt)->bFixedSizeSamples = TRUE;
	(*ppmt)->lSampleSize = pvi->bmiHeader.biSizeImage;
	(*ppmt)->cbFormat = sizeof(VIDEOINFOHEADER);

	VIDEO_STREAM_CONFIG_CAPS *pvscc = (VIDEO_STREAM_CONFIG_CAPS *)pSCC;
	pvscc->guid = FORMAT_VideoInfo;
	pvscc->VideoStandard = AnalogVideo_None;
	pvscc->InputSize.cx = WINDOW_WIDTH;
	pvscc->InputSize.cy = WINDOW_HEIGHT;
	pvscc->MinCroppingSize.cx = WINDOW_WIDTH;
	pvscc->MinCroppingSize.cy = WINDOW_HEIGHT;
	pvscc->MaxCroppingSize.cx = WINDOW_WIDTH;
	pvscc->MaxCroppingSize.cy = WINDOW_HEIGHT;
	pvscc->CropGranularityX = 80;
	pvscc->CropGranularityY = 60;
	pvscc->CropAlignX = 0;
	pvscc->CropAlignY = 0;

	pvscc->MinOutputSize.cx = WINDOW_WIDTH;
	pvscc->MinOutputSize.cy = WINDOW_HEIGHT;
	pvscc->MaxOutputSize.cx = WINDOW_WIDTH;
	pvscc->MaxOutputSize.cy = WINDOW_HEIGHT;
	pvscc->OutputGranularityX = 0;
	pvscc->OutputGranularityY = 0;
	pvscc->StretchTapsX = 0;
	pvscc->StretchTapsY = 0;
	pvscc->ShrinkTapsX = 0;
	pvscc->ShrinkTapsY = 0;
	pvscc->MinFrameInterval = 200000; //50fps
	pvscc->MaxFrameInterval = 50000000; //0.2fps
	pvscc->MinBitsPerSecond = (WINDOW_WIDTH * WINDOW_HEIGHT * PIXEL_BIT) / 5;
	pvscc->MaxBitsPerSecond = (WINDOW_WIDTH * WINDOW_HEIGHT * PIXEL_BIT) * 50;

	return S_OK;
}

HRESULT NMVCamPin::SetFormat(
	AM_MEDIA_TYPE *pmt
)
{
	VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)m_mt.pbFormat;
	m_mt = *pmt;
	IPin *pin;
	ConnectedTo(&pin);
	if (pin) {
		IFilterGraph *pGraph = m_pFilter->GetGraph();
		pGraph->Reconnect(this);
	}
	return S_OK;
}
//IAMStreamConfig‚±‚±‚Ü‚Å

//IAMFilterMiscFlags
ULONG STDMETHODCALLTYPE NMVCamPin::GetMiscFlags() {
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}
//IAMFilterMiscFlags‚±‚±‚Ü‚Å

