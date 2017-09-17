#include "stdafx.h"

#include <iostream>

#include "../ComponentDll/ComponentDllAPI.h"
#include "../ComponentDll/ComponentDll.h"

int _tmain(int argc, _TCHAR* argv[])
{
    HMODULE lib = LoadLibrary(_T("ComponentDll"));
    if (!lib)
    {
        std::cout << "Cannot load library, die" << std::endl;
        system("pause");
        return 1;
    }
    FARPROC proc = GetProcAddress(lib, "CreateInstance");
    if (!proc)
    {
        std::cout << "Cannot find CreateInstance(), die" << std::endl;
        system("pause");
        return 2;
    }
    IUnknown * component = ((ComponentDll::CREATEINSTANCEPROC)proc)();

    std::cout << "IUnknown (created) - 0x" << component << std::endl;

    // Assert transitivity, repeatability and so on
    {
        IUnknown * another;
        HRESULT r = component->QueryInterface(IID_IUnknown, (void**)&another);
        std::cout << "IUnknown (queried from IUnknown) - 0x" << another << " - HRESULT " << r << std::endl;
        another->Release();

        ComponentDll::ICollection * collection;
        r = component->QueryInterface(ComponentDll::IID_ICollection, (void**)&collection);
        std::cout << "ICollection (queried from IUnknown) - 0x" << collection << " - HRESULT " << r << std::endl;

        ComponentDll::ICollection * collection2;
        r = collection->QueryInterface(ComponentDll::IID_ICollection, (void**)&collection2);
        std::cout << "ICollection (queried from ICollection) - 0x" << collection2 << " - HRESULT " << r << std::endl;
        collection2->Release();

        r = collection->QueryInterface(IID_IUnknown, (void**)&another);
        std::cout << "IUnknown (queried from ICollection) - 0x" << another << " - HRESULT " << r << std::endl;
        another->Release();

        ComponentDll::IEnumerator * enumerator;
        r = component->QueryInterface(ComponentDll::IID_IEnumerator, (void**)&enumerator);
        std::cout << "IEnumerator (queried from IUnknown) - 0x" << enumerator << " - HRESULT " << r << std::endl;

        ComponentDll::IEnumerator * enumerator2;
        r = enumerator->QueryInterface(ComponentDll::IID_IEnumerator, (void**)&enumerator2);
        std::cout << "IEnumerator (queried from IEnumerator) - 0x" << enumerator2 << " - HRESULT " << r << std::endl;
        enumerator2->Release();

        r = collection->QueryInterface(ComponentDll::IID_IEnumerator, (void**)&enumerator2);
        std::cout << "IEnumerator (queried from ICollection) - 0x" << enumerator2 << " - HRESULT " << r << std::endl;
        enumerator2->Release();

        r = enumerator->QueryInterface(IID_IUnknown, (void**)&another);
        std::cout << "IUnknown (queried from IEnumerator) - 0x" << another << " - HRESULT " << r << std::endl;
        another->Release();

        enumerator->Release();
        collection->Release();
    }

    ComponentDll::ICollection * collection;
    component->QueryInterface(ComponentDll::IID_ICollection, (void**)&collection);

    ComponentDll::Object obj1; obj1.Type = ComponentDll::ObjectType::otInt; obj1.Value.Int = 123456789;
    ComponentDll::Object obj2; obj2.Type = ComponentDll::ObjectType::otDouble; obj1.Value.Double = 0.123456789;

    std::cout << "Add 1st object to collection: HRESULT - " << collection->Add(obj1) << std::endl;
    std::cout << "Add 2nd object to collection: HRESULT - " << collection->Add(obj2) << std::endl;

    // Enumerate the collection
    {
        ComponentDll::IEnumerator * enumerator;
        component->QueryInterface(ComponentDll::IID_IEnumerator, (void**)&enumerator);
        std::cout << "Reset enumerator: HRESULT - " << enumerator->Reset() << std::endl;

        HRESULT r;
        int next;
        do
        {
            ComponentDll::Object o;
            r = enumerator->GetCurrent(&o);
            std::cout << "\tObject of type " << o.Type << " : HRESULT - " << r << std::endl;
            r = enumerator->MoveNext(&next);
            if (SUCCEEDED(r))
            {
                std::cout << "\tMove next to " << next << " position : HRESULT - " << r << std::endl;
            }
        } while (SUCCEEDED(r));

        enumerator->Release();
    }

    std::cout << "Remove 1st object from collection: HRESULT - " << collection->Remove(obj1) << std::endl;
    std::cout << "Remove 2nd object from collection: HRESULT - " << collection->Remove(obj2) << std::endl;

    collection->Release();
    component->Release();

    system("pause");

	return 0;
}
