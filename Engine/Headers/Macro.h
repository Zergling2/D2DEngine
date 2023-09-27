#pragma once

enum class DeleteType
{
	SCALAR,
	VECTOR
};

template<class Interface>
__forceinline void SafeRelease(Interface*& pInterfaceToRelease)
{
    if (pInterfaceToRelease != nullptr)
    {
        (pInterfaceToRelease)->Release();
        (pInterfaceToRelease) = nullptr;
    }
}

#define SafeDeleteScalar(p)\
{\
	if (p != nullptr)\
	{\
		delete p;\
		p = nullptr;\
	}\
}

#define SafeDeleteVector(p)\
{\
	if (p != nullptr)\
	{\
		delete[] p;\
		p = nullptr;\
	}\
}


// =========================================================================
#ifndef Assert
#if defined(DEBUG) || defined(_DEBUG)
#define Assert(b)\
do\
{\
	if (!(b))\
	{\
		OutputDebugStringA("Assert: " #b "\n");\
	}\
} while (false)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif
// =========================================================================

// ============================ SINGLETON MACRO ============================
// [1] DYNAMIC SINGLETON
#define DECLARE_DYNAMIC_ST(Type)	\
private:\
	Type();\
	~Type();\
	static Type* s_pInstance;\
public:\
	static __forceinline Type& GetInstance()\
	{\
		if (Type::s_pInstance == nullptr)\
		{\
			Type::s_pInstance = new Type;\
			atexit(Type::Destroy);\
		}\
		return *Type::s_pInstance;\
	}\
	static void Destroy()\
	{\
		SafeDelete(Type::s_pInstance);\
	}

#define DEFINE_DYNAMIC_ST_INSTANCE_PTR(Type) Type* Type::s_pInstance = nullptr;
// =========================================================================


// [2] STATIC SINGLETON
#define DECLARE_STATIC_ST(Type)	\
private:\
	Type();\
	~Type();\
	static Type s_instance;\
public:\
	static __forceinline Type& GetInstance()\
	{\
		return Type::s_instance;\
	}\

#define DEFINE_STATIC_ST_INSTANCE(Type) Type Type::s_instance;
// =========================================================================


// =========================================================================
template<class Interface>
void SafeDeletePointerVector(std::vector<Interface>& ref)
{
	auto end = ref.end();
	for (auto i = ref.begin(); i != end; ++i)
		delete (*i);

	ref.clear();
	ref.shrink_to_fit();
}

template<class Interface>
void SafeDeletePointerList(std::list<Interface>& ref)
{
	auto end = ref.end();
	auto i = ref.begin();
	while (i != end)
	{
		delete* i;
		i = ref.erase(i);
	}
}
// =========================================================================


#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif