#include "GameObject.h"
#include "Components\Derived\Transform.h"

using namespace D2DEngine;

GameObject::GameObject()
	: Object(ActiveFlag(true), L"", ObjectTag::Default)
	, m_transform(this)
{
}

GameObject::GameObject(ActiveFlag flag, const wchar_t* name, ObjectTag tag)
	: Object(flag, name, tag)
	, m_transform(this)
{
}

GameObject::GameObject(ActiveFlag flag, const std::wstring& name, ObjectTag tag)
	: Object(flag, name, tag)
	, m_transform(this)
{
}

GameObject::~GameObject()
{
}
