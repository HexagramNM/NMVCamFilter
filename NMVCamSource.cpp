#include "stdafx.h"

NMVCamSource::NMVCamSource(IUnknown *pUnk, HRESULT *phr) :
	CSource(FILTER_NAME, pUnk, CLSID_NMVCamFilter)
{
	NMVCamPin *pPin = new NMVCamPin(phr, this);
	if(pPin == NULL) {
		*phr=E_OUTOFMEMORY;
	}
}

NMVCamSource::~NMVCamSource() {
}

CUnknown * WINAPI NMVCamSource::CreateInstance(IUnknown *pUnk, HRESULT *phr) {
	NMVCamSource *pNewFilter = new NMVCamSource(pUnk, phr);
	if(pNewFilter == NULL) {
		*phr=E_OUTOFMEMORY;
	}
	return pNewFilter;
}

STDMETHODIMP NMVCamSource::QueryInterface(REFIID riid, void **ppv) {
	if (riid == _uuidof(IAMStreamConfig) || riid == _uuidof(IKsPropertySet)) {
		return m_paStreams[0]->QueryInterface(riid, ppv);
	}
	else {
		return CSource::QueryInterface(riid, ppv);
	}
}

