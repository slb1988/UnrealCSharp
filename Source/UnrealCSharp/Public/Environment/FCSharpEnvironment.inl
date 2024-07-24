#pragma once

#include "Registry/FContainerRegistry.h"
#include "Registry/FDelegateRegistry.h"
#include "Registry/FBindingRegistry.h"
#include "Registry/FCSharpBind.h"
#if UE_F_OPTIONAL_PROPERTY
#include "Registry/FOptionalRegistry.h"
#endif

template <bool bIsWeak>
auto FCSharpEnvironment::Bind(UObject* Object) const
{
	return FCSharpBind::Bind<bIsWeak>(Domain, Object);
}

template <bool bIsWeak>
auto FCSharpEnvironment::Bind(const UObject* Object) const
{
	return Bind<bIsWeak>(const_cast<UObject*>(Object));
}

template <bool bIsWeak>
auto FCSharpEnvironment::Bind(UClass* Class) const
{
	return FCSharpBind::Bind<bIsWeak>(Domain, Class);
}

template <bool bIsWeak, bool bNeedMonoClass>
auto FCSharpEnvironment::Bind(UObject* Object) const
{
	return FCSharpBind::Bind<bIsWeak, bNeedMonoClass>(Domain, Object);
}

template <bool bNeedMonoClass>
bool FCSharpEnvironment::Bind(UStruct* InStruct) const
{
	return FCSharpBind::Bind<bNeedMonoClass>(Domain, InStruct);
}

template <typename T>
T* FCSharpEnvironment::TGetAddress<UObject, T>::operator()(const FCSharpEnvironment* InEnvironment,
                                                           const FGarbageCollectionHandle& InGarbageCollectionHandle)
const
{
	if (InEnvironment != nullptr && InEnvironment->ObjectRegistry != nullptr)
	{
		if (const auto FoundObject = InEnvironment->ObjectRegistry->GetAddress(InGarbageCollectionHandle))
		{
			return static_cast<T*>(FoundObject);
		}
	}

	return nullptr;
}

template <typename T>
T* FCSharpEnvironment::TGetAddress<UScriptStruct, T>::operator()(const FCSharpEnvironment* InEnvironment,
                                                                 const FGarbageCollectionHandle&
                                                                 InGarbageCollectionHandle) const
{
	if (InEnvironment != nullptr && InEnvironment->StructRegistry != nullptr)
	{
		if (const auto FoundStruct = InEnvironment->StructRegistry->GetAddress(InGarbageCollectionHandle))
		{
			return static_cast<T*>(FoundStruct);
		}
	}

	return nullptr;
}

template <typename T, typename U>
auto FCSharpEnvironment::GetAddress(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return TGetAddress<T, U>()(this, InGarbageCollectionHandle);
}

template<bool bIsWeak>
auto FCSharpEnvironment::AddObjectReference(UObject* InObject, MonoObject* InMonoObject) const
{
	return ObjectRegistry != nullptr ? ObjectRegistry->AddReference<bIsWeak>(InObject, InMonoObject) : false;
}

template <>
inline void* FCSharpEnvironment::GetAddress<UObject>(const FGarbageCollectionHandle& InGarbageCollectionHandle,
                                                     UStruct*& InStruct) const
{
	if (ObjectRegistry != nullptr)
	{
		if (const auto FoundObject = ObjectRegistry->GetAddress(InGarbageCollectionHandle, InStruct))
		{
			return FoundObject;
		}
	}

	return nullptr;
}

template <>
inline void* FCSharpEnvironment::GetAddress<UScriptStruct>(const FGarbageCollectionHandle& InGarbageCollectionHandle,
                                                           UStruct*& InStruct) const
{
	if (StructRegistry != nullptr)
	{
		if (const auto FoundStruct = StructRegistry->GetAddress(InGarbageCollectionHandle, InStruct))
		{
			return FoundStruct;
		}
	}

	return nullptr;
}

template <typename T>
auto FCSharpEnvironment::GetObject(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return ObjectRegistry != nullptr ? Cast<T>(ObjectRegistry->GetObject(InGarbageCollectionHandle)) : nullptr;
}

template <typename T>
auto FCSharpEnvironment::GetContainer(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return ContainerRegistry != nullptr
		       ? FContainerRegistry::TContainerRegistry<T>::GetContainer(ContainerRegistry, InGarbageCollectionHandle)
		       : nullptr;
}

template <typename T>
auto FCSharpEnvironment::GetContainerObject(void* InAddress) const
{
	return ContainerRegistry != nullptr
		       ? FContainerRegistry::TContainerRegistry<T>::GetObject(ContainerRegistry, InAddress)
		       : nullptr;
}

template <typename T>
auto FCSharpEnvironment::AddContainerReference(T* InValue, MonoObject* InMonoObject) const
{
	return ContainerRegistry != nullptr
		       ? FContainerRegistry::TContainerRegistry<T>::AddReference(
			       ContainerRegistry, InValue, InMonoObject)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::AddContainerReference(const FGarbageCollectionHandle& InOwner, void* InAddress, T* InValue,
                                               MonoObject* InMonoObject) const
{
	return ContainerRegistry != nullptr
		       ? FContainerRegistry::TContainerRegistry<T>::AddReference(
			       ContainerRegistry, InOwner, InAddress, InValue, InMonoObject)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::RemoveContainerReference(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return ContainerRegistry != nullptr
		       ? FContainerRegistry::TContainerRegistry<T>::RemoveReference(
			       ContainerRegistry, InGarbageCollectionHandle)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::GetDelegate(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return DelegateRegistry != nullptr
		       ? FDelegateRegistry::TDelegateRegistry<T>::GetDelegate(DelegateRegistry, InGarbageCollectionHandle)
		       : nullptr;
}

template <typename T>
auto FCSharpEnvironment::GetDelegateObject(void* InAddress) const
{
	return DelegateRegistry != nullptr
		       ? FDelegateRegistry::TDelegateRegistry<T>::GetObject(DelegateRegistry, InAddress)
		       : nullptr;
}

template <typename T>
auto FCSharpEnvironment::AddDelegateReference(T* InValue, MonoObject* InMonoObject) const
{
	return DelegateRegistry != nullptr
		       ? FDelegateRegistry::TDelegateRegistry<T>::AddReference(DelegateRegistry, InValue, InMonoObject)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::AddDelegateReference(const FGarbageCollectionHandle& InOwner, void* InAddress, T* InValue,
                                              MonoObject* InMonoObject) const
{
	return DelegateRegistry != nullptr
		       ? FDelegateRegistry::TDelegateRegistry<T>::AddReference(DelegateRegistry, InOwner, InAddress, InValue,
		                                                               InMonoObject)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::RemoveDelegateReference(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return DelegateRegistry != nullptr
		       ? FDelegateRegistry::TDelegateRegistry<T>::RemoveReference(DelegateRegistry, InGarbageCollectionHandle)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::GetMulti(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return MultiRegistry != nullptr
		       ? FMultiRegistry::TMultiRegistry<T, T>::GetMulti(MultiRegistry, InGarbageCollectionHandle)
		       : nullptr;
}

template <typename T>
auto FCSharpEnvironment::GetMultiObject(void* InAddress) const
{
	return MultiRegistry != nullptr
		       ? FMultiRegistry::TMultiRegistry<T, T>::GetObject(MultiRegistry, InAddress)
		       : nullptr;
}

template <typename T, bool bNeedFree>
auto FCSharpEnvironment::AddMultiReference(MonoObject* InMonoObject, void* InValue) const
{
	return MultiRegistry != nullptr
		       ? FMultiRegistry::TMultiRegistry<T, T>::template AddReference<bNeedFree>(MultiRegistry, InMonoObject, InValue)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::RemoveMultiReference(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return MultiRegistry != nullptr
		       ? FMultiRegistry::TMultiRegistry<T, T>::RemoveReference(MultiRegistry, InGarbageCollectionHandle)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::GetString(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return StringRegistry != nullptr
		       ? FStringRegistry::TStringRegistry<T>::GetString(StringRegistry, InGarbageCollectionHandle)
		       : nullptr;
}

template <typename T>
auto FCSharpEnvironment::GetStringObject(void* InAddress) const
{
	return StringRegistry != nullptr
		       ? FStringRegistry::TStringRegistry<T>::GetObject(StringRegistry, InAddress)
		       : nullptr;
}

template <typename T, bool bNeedFree>
auto FCSharpEnvironment::AddStringReference(MonoObject* InMonoObject, void* InValue) const
{
	return StringRegistry != nullptr
		       ? FStringRegistry::TStringRegistry<T>::template AddReference<bNeedFree>(StringRegistry, InMonoObject, InValue)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::RemoveStringReference(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return StringRegistry != nullptr
		       ? FStringRegistry::TStringRegistry<T>::RemoveReference(StringRegistry, InGarbageCollectionHandle)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::GetBinding(const FGarbageCollectionHandle& InGarbageCollectionHandle) const
{
	return BindingRegistry != nullptr
		       ? BindingRegistry->GetBinding<T>(InGarbageCollectionHandle)
		       : nullptr;
}

template <typename T, bool bNeedFree>
auto FCSharpEnvironment::AddBindingReference(MonoObject* InMonoObject, const T* InObject) const
{
	return BindingRegistry != nullptr ? BindingRegistry->AddReference<T, bNeedFree>(InObject, InMonoObject) : false;
}

template <typename T>
auto FCSharpEnvironment::AddBindingReference(const FGarbageCollectionHandle& InOwner, MonoObject* InMonoObject,
                                             const T* InObject) const
{
	return BindingRegistry != nullptr ? BindingRegistry->AddReference(InOwner, InObject, InMonoObject) : false;
}

#if UE_F_OPTIONAL_PROPERTY
template <typename T>
auto FCSharpEnvironment::GetOptionalObject(void* InAddress) const
{
	return OptionalRegistry != nullptr
		       ? OptionalRegistry->GetObject(InAddress)
		       : nullptr;
}

template <typename T>
auto FCSharpEnvironment::AddOptionalReference(T* InValue, MonoObject* InMonoObject) const
{
	return OptionalRegistry != nullptr
		       ? OptionalRegistry->AddReference(InValue, InMonoObject)
		       : false;
}

template <typename T>
auto FCSharpEnvironment::AddOptionalReference(void* InAddress, T* InValue, MonoObject* InMonoObject) const
{
	return OptionalRegistry != nullptr
		       ? OptionalRegistry->AddReference(InAddress, InValue, InMonoObject)
		       : false;
}
#endif
