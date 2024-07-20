﻿#pragma once

#include "Reflection/Property/TCompoundPropertyDescriptor.inl"

class FSoftObjectPropertyDescriptor final : public TCompoundPropertyDescriptor<FSoftObjectProperty>
{
public:
	using TCompoundPropertyDescriptor::TCompoundPropertyDescriptor;

public:
	virtual void Get(void* Src, void** Dest, bool bIsCopy) const override;

	virtual void Set(void* Src, void* Dest) const override;

public:
	virtual bool Identical(const void* A, const void* B, uint32 PortFlags = 0) const override;

private:
	MonoObject* NewWeakRef(void* InAddress) const;
};
