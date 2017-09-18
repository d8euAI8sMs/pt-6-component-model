#ifdef COMPONENTDLL_EXPORTS
#define COMPONENTDLL_API __declspec(dllexport)
#else
#define COMPONENTDLL_API __declspec(dllimport)
#endif

#include <objbase.h>
#include "ComponentDllAPI.h"

#include <iostream>
#include <vector>

extern "C" COMPONENTDLL_API IUnknown * CreateInstance();

namespace ComponentDll
{
    namespace Impl
    {

        extern ULONG g_ComponentRefs;

        class CList : public ComponentDll::ICollection, public ComponentDll::IEnumerator
        {
            ULONG m_cRef;
            std::vector < ComponentDll::Object > mBackingCollection;
            SIZE_T cIteratorPosition;

            // IUnknown
            HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
            ULONG   __stdcall AddRef();
            ULONG   __stdcall Release();

            // ICollection
            HRESULT __stdcall Add(ComponentDll::Object obj);
            HRESULT __stdcall Remove(ComponentDll::Object obj);
            HRESULT __stdcall GetCount(unsigned *count);
            HRESULT __stdcall ToArray(ComponentDll::ObjectArray **arr);

            // IEnumerator
            HRESULT __stdcall Reset();
            HRESULT __stdcall MoveNext(int *result);
            HRESULT __stdcall GetCurrent(ComponentDll::Object *obj);

        public:

            CList() : m_cRef(1) { InterlockedIncrement(&g_ComponentRefs); }

            ~CList() { std::cout << __FUNCTION__ << std::endl; InterlockedDecrement(&g_ComponentRefs); }
        };

        class CListFactory : public IClassFactory
        {
            ULONG m_cRef;

            // IUnknown
            HRESULT __stdcall QueryInterface(REFIID riid, void ** ppv);
            ULONG   __stdcall AddRef();
            ULONG   __stdcall Release();

            // IClassFactory
            HRESULT __stdcall CreateInstance(LPUNKNOWN pUnk, const IID & id, void ** ppv);
            HRESULT __stdcall LockServer (BOOL fLock);

        public:

            CListFactory() : m_cRef(1) { InterlockedIncrement(&g_ComponentRefs); }

            ~CListFactory() { std::cout << __FUNCTION__ << std::endl; InterlockedDecrement(&g_ComponentRefs); }
        };
    }
}
