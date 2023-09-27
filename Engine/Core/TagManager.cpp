#include "TagManager.h"

using namespace D2DEngine;

/*
DEFINE_STATIC_ST_INSTANCE(TagManager)

TagManager::TagManager()
	: m_defaultTag(nullptr)
{
}

TagManager::~TagManager()
{
	Release();
}

bool TagManager::Initialize()
{
	char* p = new(std::nothrow) char[1];
	if (p == nullptr)
	{
		// BAD_ALLOC_LOGGING;
		return false;
	}

	*p = '\0';
	m_defaultTag = p;
	return true;
}

const char* TagManager::GetTag(const char* tag)
{
#ifdef _DEBUG
	if (tag == nullptr)
		return m_defaultTag;

	const std::set<const char*, TagSort>::iterator f = m_tagSet.find(tag);
	if (f == m_tagSet.end())
	{
		printf("Tag created: %s\n", tag);
		return CreateTag(tag);
	}
	else
	{
		printf("Tag(%s) already exists.\n", tag);
		return (*f);
	}
#else
	if (tag == nullptr)
		return m_defaultTag;

	const std::set<const char*, TagSort>::iterator f = m_tagSet.find(tag);
	if (f == m_tagSet.end())
		return CreateTag(tag);
	else
		return (*f);
#endif // !
}

void TagManager::Release()
{
	const auto end = m_tagSet.end();
	auto iter = m_tagSet.begin();
	while (iter != end)
	{
		delete[] * iter;
		iter = m_tagSet.erase(iter);
	}

	m_defaultTag = nullptr;
}

const char* TagManager::CreateTag(const char* tag)
{
	size_t strSize = strlen(tag) + 1;
	char* mem = new(std::nothrow) char[strSize];
	if (mem == nullptr)
		// m_log.WriteLog(MEM_ALLOC_FAIL, LogType::LT_SYSTEM);
		;
	else
		strcpy_s(mem, strSize, tag);

	m_tagSet.insert(mem);

	return mem;
}
*/