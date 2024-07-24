#pragma once

template <bool bIsWeak>
auto FObjectRegistry::AddReference(UObject* InObject, MonoObject* InMonoObject)
{
	if constexpr (bIsWeak)
	{
		const auto GarbageCollectionHandle = FGarbageCollectionHandle::NewWeakRef(InMonoObject, true);

		GarbageCollectionHandle2Object.Add(GarbageCollectionHandle, &*InObject);
	}
	else
	{
		const auto GarbageCollectionHandle = FGarbageCollectionHandle::NewRef(InMonoObject, true);

		Object2GarbageCollectionHandleMap.Add(InObject, GarbageCollectionHandle);

		GarbageCollectionHandle2Object.Add(GarbageCollectionHandle, &*InObject);
	}

	return true;
}