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

	void* CopyValue(const void* InAddress) const
	{
		const auto Value = static_cast<void*>(static_cast<uint8*>(
			FMemory::Malloc(TPropertyDescriptor<T, false>::Property->ElementSize)));

		TPropertyDescriptor<T, false>::Property->InitializeValue(Value);
		
		TPropertyDescriptor<T, false>::Property->CopySingleValue(Value, InAddress);

		return Value;
	}
};
