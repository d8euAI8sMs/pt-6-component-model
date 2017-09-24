#include "stdafx.h"

#include "ComponentDll.h"
#include "ComponentDllAPI.h"

#include <iostream>
#include <vector>

bool operator == (const Object & o1, const Object & o2)
{
    if (!(o1.Type == o2.Type)) return false;
    switch (o1.Type)
    {
    case ObjectType::otInt:
        return o1.Value.Int == o2.Value.Int;
    case ObjectType::otDouble:
        return o1.Value.Double == o2.Value.Double;
    case ObjectType::otArray:
        return o1.Value.Array == o2.Value.Array;
    default:
        break;
    }
    return false;
}

HRESULT __stdcall ComponentDll::Impl::CList::QueryInterface(REFIID iid, void** ppv)
{
    if (ppv == NULL) return E_POINTER;
    if ((iid == IID_IUnknown) || (iid == IID_ICollection))
    {
        *ppv = (ICollection*) this;
        return AddRef(), S_OK;
    }
    else if ((iid == IID_IEnumerator))
    {
        *ppv = (IEnumerator*) this;
        return AddRef(), S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}

ULONG __stdcall ComponentDll::Impl::CList::AddRef()
{
    return ++m_cRef;
}

ULONG __stdcall ComponentDll::Impl::CList::Release()
{
    ULONG val = --m_cRef;
    if (!val)
    {
        delete this;
        return 0;
    }
    return val;
}

HRESULT __stdcall ComponentDll::Impl::CList::Add(Object obj)
{
    mBackingCollection.push_back(obj);
    return S_OK;
}

HRESULT __stdcall ComponentDll::Impl::CList::Remove(Object obj)
{
    for (size_t i = 0; i < mBackingCollection.size(); i++)
    {
        if (obj == mBackingCollection[i])
        {
            mBackingCollection.erase(mBackingCollection.begin() + i);
            return S_OK;
        }
    }
    return S_FALSE;
}

HRESULT __stdcall ComponentDll::Impl::CList::GetCount(unsigned *count)
{
    if (!count) return E_POINTER;
    *count = mBackingCollection.size();
    return S_OK;
}

HRESULT __stdcall ComponentDll::Impl::CList::ToArray(ObjectArray **arr)
{
    if (!arr) return E_POINTER;
    *arr = (ObjectArray *)CoTaskMemAlloc(sizeof(ObjectArray));
    if (!*arr) return E_OUTOFMEMORY;
    (*arr)->Count = mBackingCollection.size();
    (*arr)->Data = (Object *)CoTaskMemAlloc(mBackingCollection.size() * sizeof(Object));
    if (!(*arr)->Data) { CoTaskMemFree(*arr); return E_OUTOFMEMORY; }
    SIZE_T idx = 0;
    for each (const Object &obj in mBackingCollection)
    {
        (*arr)->Data[idx] = obj;
    }
    return S_OK;
}

HRESULT __stdcall ComponentDll::Impl::CList::Reset()
{
    cIteratorPosition = 0;
    return S_OK;
}

HRESULT __stdcall ComponentDll::Impl::CList::MoveNext(int *result)
{
	if (!result) return E_POINTER;
	if ((cIteratorPosition + 1) >= mBackingCollection.size())
	{
		*result = FALSE;
		return S_FALSE;
	}
	++cIteratorPosition;
	*result = TRUE;
	return S_OK;
}

HRESULT __stdcall ComponentDll::Impl::CList::GetCurrent(Object *obj)
{
	if (!obj) return E_POINTER;
	if (cIteratorPosition >= mBackingCollection.size()) return E_NOT_VALID_STATE;
	*obj = mBackingCollection[cIteratorPosition];
	return S_OK;
}

HRESULT __stdcall ComponentDll::Impl::CListFactory::QueryInterface(REFIID iid, void** ppv)
{
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown)
    {
        *ppv = (IUnknown*) this;
        return AddRef(), S_OK;
    }
    else if (iid == IID_IClassFactory)
    {
        *ppv = (IClassFactory*) this;
        return AddRef(), S_OK;
    }
    return E_NOINTERFACE;
}

ULONG __stdcall ComponentDll::Impl::CListFactory::AddRef()
{
    return ++m_cRef;
}

ULONG __stdcall ComponentDll::Impl::CListFactory::Release()
{
    ULONG val = --m_cRef;
    if (!val)
    {
        delete this;
        return 0;
    }
    return val;
}

HRESULT __stdcall ComponentDll::Impl::CListFactory::CreateInstance(LPUNKNOWN pUnk, const IID & id, void ** ppv)
{
	if (pUnk != NULL) return CLASS_E_NOAGGREGATION;
    if (ppv == NULL) return E_POINTER;
    IUnknown * component = ::CreateInstance();
    HRESULT rc = component->QueryInterface(id, ppv);
    component->Release();
    return rc;
}

ULONG ComponentDll::Impl::g_ComponentRefs = 0;

HRESULT __stdcall ComponentDll::Impl::CListFactory::LockServer(BOOL fLock)
{
	if (fLock)
    {
        CoLockObjectExternal((IUnknown *) this, TRUE, TRUE);
    }
    else
    {
        CoLockObjectExternal((IUnknown *) this, FALSE, TRUE);
    }
    return S_OK;
}

extern "C" COMPONENTDLL_API IUnknown* CreateInstance()
{
    IUnknown * com = reinterpret_cast<IUnknown *>(new ComponentDll::Impl::CList());
    return com;
}

STDAPI DllCanUnloadNow() {
	HRESULT rc = E_UNEXPECTED;
	if (ComponentDll::Impl::g_ComponentRefs == 0) rc = S_OK;
	else rc = S_FALSE;
	return rc;
};

STDAPI DllGetClassObject(const CLSID & clsid, const IID & iid, void ** ppv) {
	HRESULT rc = E_UNEXPECTED;

	if (clsid == ComponentDll::CLSID_CLIST) {
		IUnknown * cf = new ComponentDll::Impl::CListFactory();
		rc = cf->QueryInterface(iid, ppv);
		cf->Release();
	}
	else
    {
		rc = CLASS_E_CLASSNOTAVAILABLE;
        *ppv = NULL;
    }

	return rc;
};