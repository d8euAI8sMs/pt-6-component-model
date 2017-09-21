#pragma once

#include <objbase.h>


/**
 * summary:
 * 
 *      Contains a number of interfaces and plain object
 *      definitions.
 *      
 * remarks:
 * 
 *      Thread safety notice. No explicit threading model is
 *      implemented. Be sure the component is accessed from
 *      a single thread (or stored in a (primary) STA).
 */
namespace ComponentDll
{

    enum ObjectType
    {
        otInt,
        otDouble,
        otArray
    };

    struct Object;

    struct ObjectArray
    {
        Object *Data;
        unsigned Count;
    };

    struct Object
    {
        ObjectType Type;
        union
        {
            int Int;
            double Double;
            ObjectArray *Array;
        } Value;
    };

    bool operator == (const Object &, const Object &);

    static const GUID IID_ICollection =
    { 0x37f7727a, 0x174, 0x4f2b, { 0xad, 0xe9, 0x27, 0x1b, 0x94, 0x9a, 0xa2, 0x29 } };

    interface ICollection : public IUnknown
    {
        virtual HRESULT __stdcall Add(Object obj) = 0;

        virtual HRESULT __stdcall Remove(Object obj) = 0;

        virtual HRESULT __stdcall GetCount(unsigned *count) = 0;

        /**
         * summary:
         * 
         *      Allocates the new ObjectArray and copies
         *      to it whole content of this collection.
         *      
         * params:
         * 
         *      arr - The pointer to (ObjectArray *) variable
         *      
         * returns:
         * 
         *      S_OK      - On success
         *      E_POINTER - If NULL passed
         *      
         * remarks:
         * 
         *      The function allocates an array of Objects.
         *      The array itself and its Data field must be freed
         *      manually by call to the operator delete (or delete[]).
         */
        virtual HRESULT __stdcall ToArray(ObjectArray **arr) = 0;
    };

    static const GUID IID_IEnumerator =
    { 0x7baab13d, 0xac13, 0x484c, { 0xa1, 0xa0, 0x13, 0x2b, 0x28, 0x13, 0x64, 0x25 } };

    interface IEnumerator : public IUnknown
    {
        virtual HRESULT __stdcall Reset() = 0;
        
        /**
         * summary:
         * 
         *      Moves this iterator object a one position next,
         *      if possible.
         *      
         * params:
         * 
         *      result - The result of the operation
         *      
         * returns:
         * 
         *      S_OK      - On success
         *      S_FALSE   - If no more elements present
         *      E_POINTER - If NULL passed
         */
        virtual HRESULT __stdcall MoveNext(int *result) = 0;
        
        /**
         * summary:
         * 
         *      Retrieves the object this iterator object
         *      points to.
         *      
         * params:
         * 
         *      obj - Receives the copy of the object
         *            pointed by this iterator object
         *      
         * returns:
         * 
         *      S_OK              - On success
         *      E_NOT_VALID_STATE - If the iterator position
         *                          is invalid
         *      E_POINTER         - If NULL passed
         *      
         * remarks:
         * 
         *      The iterator may become invalid in case if
         *      the collection size is reduced. To ensure
         *      the iterator is always in valid state, don't
         *      mix collection modification operations and
         *      collection iteration operations.
         *      
         *      Since the iterator in this model is supposed
         *      to be a singleton, reset function should
         *      always be called right after the iterator is obtained.
         */
        virtual HRESULT __stdcall GetCurrent(Object *obj) = 0;
    };

    typedef IUnknown * (*CREATEINSTANCEPROC)();
}
