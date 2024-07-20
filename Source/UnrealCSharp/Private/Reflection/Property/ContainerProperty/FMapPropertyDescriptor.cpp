﻿#include "Reflection/Property/ContainerProperty/FMapPropertyDescriptor.h"
#include "Environment/FCSharpEnvironment.h"
#include "Reflection/Container/FMapHelper.h"

void FMapPropertyDescriptor::Get(void* Src, void** Dest, bool bIsCopy) const
{
	if (Property != nullptr)
	{
		*Dest = NewWeakRef(Src);
	}
}

void FMapPropertyDescriptor::Get(void* Src, void* Dest) const
{
	if (Property != nullptr)
	{
		*static_cast<void**>(Dest) = NewRef(Src);
	}
}

void FMapPropertyDescriptor::Set(void* Src, void* Dest) const
{
	if (Property != nullptr)
	{
		const auto SrcGarbageCollectionHandle = static_cast<FGarbageCollectionHandle>(Src);

		const auto SrcContainer = FCSharpEnvironment::GetEnvironment().GetContainer<FMapHelper>(
			SrcGarbageCollectionHandle);

		Property->InitializeValue(Dest);

		Property->CopyCompleteValue(Dest, SrcContainer->GetScriptMap());
	}
}

MonoObject* FMapPropertyDescriptor::NewRef(void* InAddress) const
{
	auto Object = FCSharpEnvironment::GetEnvironment().GetContainerObject<FMapHelper>(InAddress);

	if (Object == nullptr)
	{
		Object = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(Class);

		const auto MapHelper = new FMapHelper(Property->KeyProp, Property->ValueProp, InAddress);

		const auto OwnerGarbageCollectionHandle = FCSharpEnvironment::GetEnvironment().GetGarbageCollectionHandle(
			InAddress, Property);

		FCSharpEnvironment::GetEnvironment().AddContainerReference(OwnerGarbageCollectionHandle, InAddress, MapHelper,
		                                                           Object);
	}

	return Object;
}

MonoObject* FMapPropertyDescriptor::NewWeakRef(void* InAddress) const
{
	const auto Object = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(Class);

	const auto MapHelper = new FMapHelper(Property->KeyProp, Property->ValueProp, InAddress);

	FCSharpEnvironment::GetEnvironment().AddContainerReference(MapHelper, Object);

	return Object;
}
