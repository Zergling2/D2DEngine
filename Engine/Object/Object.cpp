#include "Object.h"

using namespace D2DEngine;

Object::Object(ActiveFlag flag, const wchar_t* name, ObjectTag tag)
	: m_name(name)
	, m_tag(tag)
	, m_isActive(flag)
{
}

Object::Object(ActiveFlag flag, const std::wstring& name, ObjectTag tag)
	: m_name(name)
	, m_tag(tag)
	, m_isActive(flag)
{
}

Object::~Object()
{
}
