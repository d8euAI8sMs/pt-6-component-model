#ifdef COMPONENTDLL_EXPORTS
#define COMPONENTDLL_API __declspec(dllexport)
#else
#define COMPONENTDLL_API __declspec(dllimport)
#endif

#include <objbase.h>

extern "C" COMPONENTDLL_API IUnknown * CreateInstance();
