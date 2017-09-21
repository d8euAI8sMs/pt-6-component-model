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

class CList : public ComponentDll::ICollection, public ComponentDll::IEnumerator
{
    ULONG m_cRef;
    std::vector < ComponentDll::Object > mBackingCollection;
    SIZE_T cIteratorPosition;

    HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    ULONG   __stdcall AddRef();
    ULONG   __stdcall Release();

    HRESULT __stdcall Add(ComponentDll::Object obj);
    HRESULT __stdcall Remove(ComponentDll::Object obj);
    HRESULT __stdcall GetCount(unsigned *count);
    HRESULT __stdcall ToArray(ComponentDll::ObjectArray **arr);

    HRESULT __stdcall Reset();
    HRESULT __stdcall MoveNext(int *result);
    HRESULT __stdcall GetCurrent(ComponentDll::Object *obj);

public:

    CList() : m_cRef(0), cIteratorPosition(MAXSIZE_T) { }

    ~CList() { std::cout << __FUNCTION__ << std::endl; }
};

HRESULT __stdcall CList::QueryInterface(REFIID iid, void** ppv)
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
    return E_NOINTERFACE;
}

ULONG __stdcall CList::AddRef()
{
    return ++m_cRef;
}

ULONG __stdcall CList::Release()
{
    ULONG val = --m_cRef;
    if (!val)
    {
        delete this;
        return 0;
    }
    return val;
}

HRESULT __stdcall CList::Add(ComponentDll::Object obj)
{
    mBackingCollection.push_back(obj);
    return S_OK;
}

HRESULT __stdcall CList::Remove(ComponentDll::Object obj)
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

HRESULT __stdcall CList::GetCount(unsigned *count)
{
    if (!count) return E_POINTER;
    *count = mBackingCollection.size();
    return S_OK;
}

HRESULT __stdcall CList::ToArray(ComponentDll::ObjectArray **arr)
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

HRESULT __stdcall CList::Reset()
{
	cIteratorPosition = MAXSIZE_T;
    return S_OK;
}

HRESULT __stdcall CList::MoveNext(int *result)
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

HRESULT __stdcall CList::GetCurrent(ComponentDll::Object *obj)
{
    if (!obj) return E_POINTER;
    if (cIteratorPosition >= mBackingCollection.size()) return E_NOT_VALID_STATE;
    *obj = mBackingCollection[cIteratorPosition];
    return S_OK;
}

extern "C" COMPONENTDLL_API IUnknown* CreateInstance()
{
    IUnknown * com = reinterpret_cast<IUnknown *>(new CList());
    return com->AddRef(), com;
}
