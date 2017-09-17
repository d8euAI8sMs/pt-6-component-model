#include "stdafx.h"

#include "ComponentDll.h"
#include "ComponentDllAPI.h"

#include <iostream>
#include <vector>

bool ComponentDll:: operator == (const ComponentDll::Object & o1, const ComponentDll::Object & o2)
{
    if (!(o1.Type == o2.Type)) return false;
    switch (o1.Type)
    {
    case ComponentDll::ObjectType::otInt:
        return o1.Value.Int == o2.Value.Int;
    case ComponentDll::ObjectType::otDouble:
        return o1.Value.Double == o2.Value.Double;
    case ComponentDll::ObjectType::otArray:
        return o1.Value.Array == o2.Value.Array;
    default:
        break;
    }
    return false;
}

HRESULT __stdcall ComponentDll::Impl::CList::QueryInterface(REFIID iid, void** ppv)
{
    if (ppv == NULL) return E_POINTER;
    if ((iid == IID_IUnknown) || (iid == ComponentDll::IID_ICollection))
    {
        *ppv = (ICollection*) this;
        return AddRef(), S_OK;
    }
    else if ((iid == ComponentDll::IID_IEnumerator))
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

HRESULT __stdcall ComponentDll::Impl::CList::Add(ComponentDll::Object obj)
{
    mBackingCollection.push_back(obj);
    return S_OK;
}

HRESULT __stdcall ComponentDll::Impl::CList::Remove(ComponentDll::Object obj)
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

HRESULT __stdcall ComponentDll::Impl::CList::ToArray(ComponentDll::ObjectArray **arr)
{
    if (!arr) return E_POINTER;
    *arr = new ComponentDll::ObjectArray();
    (*arr)->Count = mBackingCollection.size();
    (*arr)->Data = new ComponentDll::Object[mBackingCollection.size()];
    SIZE_T idx = 0;
    for each (const ComponentDll::Object &obj in mBackingCollection)
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
    if ((cIteratorPosition + 1) == mBackingCollection.size()) return E_NOT_VALID_STATE;
    ++cIteratorPosition;
    if (result)
    {
        *result = cIteratorPosition;
    }
    return S_OK;
}

HRESULT __stdcall ComponentDll::Impl::CList::GetCurrent(ComponentDll::Object *obj)
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

ULONG ComponentDll::Impl::g_ServerLocks = 0;

HRESULT __stdcall ComponentDll::Impl::CListFactory::LockServer(BOOL fLock)
{
	if (fLock)
    {
        CoLockObjectExternal((IUnknown *) this, TRUE, TRUE);
        InterlockedIncrement(&g_ServerLocks);
    }
    else
    {
        InterlockedDecrement(&g_ServerLocks);
        CoLockObjectExternal((IUnknown *) this, FALSE, TRUE);
    }
    return S_OK;
}

extern "C" COMPONENTDLL_API IUnknown* CreateInstance()
{
    IUnknown * com = reinterpret_cast<IUnknown *>(new ComponentDll::Impl::CList());
    return com->AddRef(), com;
}
