#include "stdafx.h"

// {BFA298B8-F936-477D-83F1-339F872D02FC}
DEFINE_GUID(CLSID_NMVCamFilter,
	0xbfa298b8, 0xf936, 0x477d, 0x83, 0xf1, 0x33, 0x9f, 0x87, 0x2d, 0x2, 0xfc);

// ピンタイプの定義
const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
	&MEDIATYPE_Video,
	&MEDIASUBTYPE_RGB24
};
// 入力用、出力用ピンの情報
const AMOVIESETUP_PIN sudPins =
{
	OUTPUT_PIN_NAME,
	FALSE,
	TRUE,
	FALSE,
	FALSE,
	&CLSID_NULL,
	NULL,
	1,
	&sudPinTypes
};

// フィルタ情報
const AMOVIESETUP_FILTER afFilterInfo=
{
	&CLSID_NMVCamFilter,
	FILTER_NAME,
	MERIT_NORMAL,
	1,
	&sudPins
};

class NMVCamPin;

//ソースフィルタクラス
class NMVCamSource : public CSource{
public:
	NMVCamSource(LPUNKNOWN pUnk, HRESULT *phr);
	virtual ~NMVCamSource();
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
	
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	IFilterGraph *GetGraph() { return m_pGraph; }
	
protected:
private:
};

struct WindowCell {
	HWND window;
	WCHAR name[256];
};

// プッシュピンクラス
class NMVCamPin : public CSourceStream, public IAMStreamConfig , public IKsPropertySet, public IAMFilterMiscFlags{
public:
	NMVCamPin(HRESULT *phr, NMVCamSource *pFilter);
	virtual			~NMVCamPin();
	STDMETHODIMP	Notify(IBaseFilter *pSelf, Quality q);	

	// CSourceStreamの実装
	HRESULT			GetMediaType(CMediaType *pMediaType);
	HRESULT			CheckMediaType(const CMediaType *pMediaType);
	HRESULT			DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest);
	HRESULT			FillBuffer(IMediaSample *pSample);

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID   riid, LPVOID * ppvObj);

	STDMETHODIMP_(ULONG) AddRef();

	STDMETHODIMP_(ULONG) Release();

	//IKsPropertySet
	HRESULT STDMETHODCALLTYPE Get(
		REFGUID PropSet,
		ULONG   Id,
		LPVOID  InstanceData,
		ULONG   InstanceLength,
		LPVOID  PropertyData,
		ULONG   DataLength,
		ULONG   *BytesReturned
	);

	HRESULT STDMETHODCALLTYPE Set(
		REFGUID PropSet,
		ULONG   Id,
		LPVOID  InstanceData,
		ULONG   InstanceLength,
		LPVOID  PropertyData,
		ULONG   DataLength
	);

	HRESULT STDMETHODCALLTYPE QuerySupported(
		REFGUID PropSet,
		ULONG   Id,
		ULONG   *TypeSupport
	);

	//IAMStreamConfig
	HRESULT STDMETHODCALLTYPE GetFormat(
		AM_MEDIA_TYPE **ppmt
	);

	HRESULT STDMETHODCALLTYPE GetNumberOfCapabilities(
		int *piCount,
		int *piSize
	);

	HRESULT STDMETHODCALLTYPE GetStreamCaps(
		int           iIndex,
		AM_MEDIA_TYPE **ppmt,
		BYTE          *pSCC
	);

	HRESULT STDMETHODCALLTYPE SetFormat(
		AM_MEDIA_TYPE *pmt
	);

	//IAMFilterMiscFlags
	ULONG STDMETHODCALLTYPE GetMiscFlags();

protected:
private:
	VIDEOINFOHEADER videoInfo {
		RECT{0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
		RECT{0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
		30 * WINDOW_WIDTH * WINDOW_HEIGHT * PIXEL_BIT,
		0,
		160000,
		BITMAPINFOHEADER{
			sizeof(BITMAPINFOHEADER),
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			1,
			PIXEL_BIT,
			BI_RGB,
			0,
			2500,
			2500,
			0,
			0
		}
	};

	NMVCamSource*		m_pFilter;			//このピンが所属しているフィルタへのポインタ
	unsigned __int64 m_Count;			//フレームカウンタ

	REFERENCE_TIME	m_rtFrameLength;	//1フレームあたりの時間

	HBITMAP m_Bitmap;
	LPDWORD m_BmpData;
	HDC     m_Hdc;
	
};
