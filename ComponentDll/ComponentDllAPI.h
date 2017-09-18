#pragma once

#include <objbase.h>
#include "Interfaces_h.h"

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
 *
 * change log:
 *
 *      For now, all the interfaces and constants were moved
 *      into global namespace (due to IDL and MILD compiler).
 *
 *      However, the namespace is still in use.
 */
namespace ComponentDll
{

    /**
     * CList class identifier
     */
    static const GUID CLSID_CLIST =
    { 0xc415618f, 0xc097, 0x4590, { 0x90, 0xdc, 0x86, 0x9c, 0xe7, 0x56, 0xff, 0x83} };

    typedef IUnknown * (*CREATEINSTANCEPROC)();
}
