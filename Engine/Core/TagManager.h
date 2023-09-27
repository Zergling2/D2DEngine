#pragma once

#include "Headers\Essential.h"

namespace D2DEngine
{
	// enum class로 임시 사용
	enum class ObjectTag
	{
		Default,

	};

	// 나중에 태그 관리자로 교체 예정
	/*
	class TagManager
	{
		DECLARE_STATIC_ST(TagManager)
	private:
		struct TagSort
		{
			inline bool operator()(const char* t1, const char* t2) const
			{
				return strcmp(t1, t2) > 0;
			}
		};
	public:
		bool Initialize();
		const char* GetTag(const char* tag);
		void Release();
	private:
		const char* CreateTag(const char* tag);
		std::set<const char*, TagSort> m_tagSet;
		const char* m_defaultTag;
	};
	*/
}
