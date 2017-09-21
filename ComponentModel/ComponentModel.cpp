#include "stdafx.h"

#import "ComponentDll.tlb"

#include <iostream>
#include "componentdll.tlh"

namespace ComponentDll
{
    static const UUID CLSID_CLIST     = __uuidof(CList);
    static const UUID IID_ICollection = __uuidof(ICollection);
    static const UUID IID_IEnumerator = __uuidof(IEnumerator);
}

int _tmain(int argc, _TCHAR* argv[])
{
    CoInitialize(NULL);

    IClassFactory * fct;
    HRESULT rc = CoGetClassObject(ComponentDll::CLSID_CLIST, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void**)&fct);
    if (!SUCCEEDED(rc))
    {
        std::cout << "Cannot get component factory, die" << std::endl; system("pause"); exit(0);
    }

    IUnknown * component;
    rc = fct->CreateInstance(NULL, IID_IUnknown, (void**)&component);
    if (SUCCEEDED(rc))
    {
        std::cout << "IUnknown (created) - 0x" << component << std::endl;
    }
    else
    {
        std::cout << "Cannot create component, die - HRESULT - " << rc << std::endl; system("pause"); exit(0);
    }

    fct->Release();

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

    ComponentDll::Object obj1; obj1.Type = ComponentDll::otInt; obj1.Value.Int = 123456789;
    ComponentDll::Object obj2; obj2.Type = ComponentDll::otDouble; obj1.Value.Double = 0.123456789;

    std::cout << "Add 1st object to collection: HRESULT - " << collection->Add(obj1) << std::endl;
    std::cout << "Add 2nd object to collection: HRESULT - " << collection->Add(obj2) << std::endl;

    // Enumerate the collection
    {
        ComponentDll::IEnumerator * enumerator;
        component->QueryInterface(ComponentDll::IID_IEnumerator, (void**)&enumerator);
        std::cout << "Reset enumerator: HRESULT - " << enumerator->Reset() << std::endl;

        HRESULT r;
        int hasNext;
        do
        {
            ComponentDll::Object o;
            r = enumerator->MoveNext(&hasNext);
            std::cout << "\tMove next : HRESULT - " << r << std::endl;
            if (hasNext)
            {
                r = enumerator->GetCurrent(&o);
                std::cout << "\tObject of type " << o.Type << " : HRESULT - " << r << std::endl;
            }
            else
            {
                break;
            }
        } while (r == S_OK);

        enumerator->Release();
    }

    std::cout << "Remove 1st object from collection: HRESULT - " << collection->Remove(obj1) << std::endl;
    std::cout << "Remove 2nd object from collection: HRESULT - " << collection->Remove(obj2) << std::endl;

    collection->Release();
    component->Release();

    std::cout << std::endl << std::endl;
    std::cout << "Using of some interfaces through smart pointers" << std::endl;
    std::cout << std::endl << std::endl;

    {
        ComponentDll::ICollectionPtr collectionSPtr(ComponentDll::CLSID_CLIST);

        std::cout << "Add 1st object to collection: HRESULT - " << collectionSPtr->Add(obj1) << std::endl;
        std::cout << "Add 2nd object to collection: HRESULT - " << collectionSPtr->Add(obj2) << std::endl;

        // Enumerate the collection
        {
            ComponentDll::IEnumerator * enumerator;
            collectionSPtr->QueryInterface(ComponentDll::IID_IEnumerator, (void**)&enumerator);
            ComponentDll::IEnumeratorPtr enumeratorSPtr;
            enumeratorSPtr.Attach(enumerator, true);
            enumerator->Release();

            std::cout << "Reset enumerator: HRESULT - " << enumeratorSPtr->Reset() << std::endl;

            HRESULT r;
            int next;
            do
            {
                ComponentDll::Object o;
                r = enumeratorSPtr->GetCurrent(&o);
                std::cout << "\tObject of type " << o.Type << " : HRESULT - " << r << std::endl;
                r = enumeratorSPtr->MoveNext(&next);
                if (r == S_OK)
                {
                    std::cout << "\tMove next to " << next << " position : HRESULT - " << r << std::endl;
                }
            } while (r == S_OK);
        }

        std::cout << "Remove 1st object from collection: HRESULT - " << collectionSPtr->Remove(obj1) << std::endl;
        std::cout << "Remove 2nd object from collection: HRESULT - " << collectionSPtr->Remove(obj2) << std::endl;
    }

    system("pause");

	return 0;
}
