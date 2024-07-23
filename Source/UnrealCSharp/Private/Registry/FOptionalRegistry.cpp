#include "Registry/FOptionalRegistry.h"

#if UE_F_OPTIONAL_PROPERTY
FOptionalRegistry::FOptionalRegistry()
{
	Initialize();
}

FOptionalRegistry::~FOptionalRegistry()
{
	Deinitialize();
}

void FOptionalRegistry::Initialize()
{
}

void FOptionalRegistry::Deinitialize()
{
	for (auto& Pair : OptionalGarbageCollectionHandle2Helper.Get())
	{
		if (Pair.Value != nullptr)
		{
			delete Pair.Value;

			Pair.Value = nullptr;
		}

		FGarbageCollectionHandle::Free(Pair.Key);
	}

	OptionalGarbageCollectionHandle2Helper.Empty();

	OptionalAddress2GarbageCollectionHandle.Empty();
}

FOptionalHelper* FOptionalRegistry::GetOptional(const FGarbageCollectionHandle& InGarbageCollectionHandle)
{
	const auto FoundValue = OptionalGarbageCollectionHandle2Helper.Find(InGarbageCollectionHandle);

	return FoundValue != nullptr ? *FoundValue : nullptr;
}

MonoObject* FOptionalRegistry::GetObject(const FOptionalHelperValueMapping::FAddressType& InAddress)
{
	const auto FoundGarbageCollectionHandle = OptionalAddress2GarbageCollectionHandle.Find(InAddress);

	return FoundGarbageCollectionHandle != nullptr
		       ? static_cast<MonoObject*>(*FoundGarbageCollectionHandle)
		       : nullptr;
}

bool FOptionalRegistry::AddReference(const FOptionalHelperValueMapping::ValueType& InValue, MonoObject* InMonoObject)
{
	const auto GarbageCollectionHandle = FGarbageCollectionHandle::NewWeakRef(InMonoObject, true);

	OptionalGarbageCollectionHandle2Helper.Add(GarbageCollectionHandle, InValue);

	return true;
}

bool FOptionalRegistry::AddReference(const FOptionalHelperValueMapping::FAddressType& InAddress,
                                     const FOptionalHelperValueMapping::ValueType& InValue,
                                     MonoObject* InMonoObject)
{
	const auto GarbageCollectionHandle = FGarbageCollectionHandle::NewWeakRef(InMonoObject, true);

	OptionalAddress2GarbageCollectionHandle.Add(InAddress, GarbageCollectionHandle);

	OptionalGarbageCollectionHandle2Helper.Add(GarbageCollectionHandle, InValue);

	return true;
}

bool FOptionalRegistry::RemoveReference(const FGarbageCollectionHandle& InGarbageCollectionHandle)
{
	if (const auto FoundValue = OptionalGarbageCollectionHandle2Helper.Find(InGarbageCollectionHandle))
	{
		if (const auto FoundGarbageCollectionHandle = OptionalAddress2GarbageCollectionHandle.Find(*FoundValue))
		{
			if(*FoundGarbageCollectionHandle == InGarbageCollectionHandle)
			{
				OptionalAddress2GarbageCollectionHandle.Remove(*FoundValue);
			}
		}

		delete *FoundValue;

		OptionalGarbageCollectionHandle2Helper.Remove(InGarbageCollectionHandle);

		return true;
	}

	return false;
}
#endif
