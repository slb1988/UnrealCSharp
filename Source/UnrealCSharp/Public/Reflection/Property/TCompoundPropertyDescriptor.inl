#pragma once

#include "TPropertyDescriptor.inl"

template <typename T>
class TCompoundPropertyDescriptor : public TPropertyDescriptor<T, false>
{
public:
	using TPropertyDescriptor<T, false>::TPropertyDescriptor;

public:
	virtual void Get(void* Src, void** Dest, bool bIsCopy) const override
	{
	}
	
	virtual void Get(void* Src, void* Dest) const override
	{
		Get(Src, static_cast<void**>(Dest));
	}
};
