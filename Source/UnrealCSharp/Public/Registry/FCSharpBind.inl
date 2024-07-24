#pragma once

#include "Environment/FCSharpEnvironment.h"
#include "Bridge/FTypeBridge.h"

template<bool bIsWeak>
auto FCSharpBind::Bind(FDomain* InDomain, UObject* InObject)
{
	if constexpr (!bIsWeak)
	{
		if (const auto FoundMonoObject = FCSharpEnvironment::GetEnvironment().GetObject(InObject))
		{
			return FoundMonoObject;
		}
	}
	
	return Bind<bIsWeak>(InDomain, InObject, false);
}

template<bool bIsWeak>
auto FCSharpBind::Bind(FDomain* InDomain, UClass* InClass)
{
	Bind(InDomain, static_cast<UStruct*>(InClass), false);

	return Bind<bIsWeak>(InDomain, static_cast<UObject*>(InClass));
}

template <bool bIsWeak>
auto FCSharpBind::Bind(FDomain* InDomain, UObject* InObject, const bool bNeedMonoClass)
{
	return BindImplementation<bIsWeak>(InDomain, InObject, bNeedMonoClass);
}

template <typename T>
auto FCSharpBind::Bind(MonoObject* InMonoObject, MonoReflectionType* InReflectionType)
{
	return BindImplementation<T>(InMonoObject, InReflectionType);
}

template <typename T>
auto FCSharpBind::Bind(MonoObject* InMonoObject, MonoReflectionType* InKeyReflectionType,
                       MonoReflectionType* InValueReflectionType)
{
	return BindImplementation<T>(InMonoObject, InKeyReflectionType, InValueReflectionType);
}

template <typename T>
auto FCSharpBind::Bind(MonoObject* InMonoObject)
{
	return BindImplementation<T>(InMonoObject);
}

template <bool bIsWeak>
MonoObject* FCSharpBind::BindImplementation(FDomain* InDomain, UObject* InObject, const bool bNeedMonoClass)
{
	if (InDomain == nullptr || InObject == nullptr)
	{
		return nullptr;
	}

	const auto InClass = InObject->GetClass();

	if (InClass == nullptr)
	{
		return nullptr;
	}

	if (!Bind(InDomain, static_cast<UStruct*>(InClass), bNeedMonoClass))
	{
		return nullptr;
	}

	const auto FoundClassDescriptor = FCSharpEnvironment::GetEnvironment().GetClassDescriptor(InClass);

	if (FoundClassDescriptor == nullptr)
	{
		return nullptr;
	}

	const auto FoundMonoClass = FoundClassDescriptor->GetMonoClass();

	if (FoundMonoClass == nullptr)
	{
		return nullptr;
	}

	const auto NewMonoObject = InDomain->Object_New(FoundMonoClass);

	FCSharpEnvironment::GetEnvironment().AddObjectReference<bIsWeak>(InObject, NewMonoObject);

	return NewMonoObject;
}

template <typename T>
auto FCSharpBind::BindImplementation(MonoObject* InMonoObject, MonoReflectionType* InReflectionType)
{
	const auto Property = FTypeBridge::Factory(InReflectionType, nullptr, "", EObjectFlags::RF_Transient);

	Property->SetPropertyFlags(CPF_HasGetValueTypeHash);

	const auto ContainerHelper = new T(Property, nullptr, true, true);

	FCSharpEnvironment::GetEnvironment().AddContainerReference(ContainerHelper, InMonoObject);

	return true;
}

template <typename T>
auto FCSharpBind::BindImplementation(MonoObject* InMonoObject, MonoReflectionType* InKeyReflectionType,
                                     MonoReflectionType* InValueReflectionType)
{
	const auto KeyProperty = FTypeBridge::Factory(InKeyReflectionType, nullptr, "", EObjectFlags::RF_Transient);

	KeyProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);

	const auto ValueProperty =
		FTypeBridge::Factory(InValueReflectionType, nullptr, "", EObjectFlags::RF_Transient);

	ValueProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);

	const auto ContainerHelper = new T(KeyProperty, ValueProperty, nullptr, true, true);

	FCSharpEnvironment::GetEnvironment().AddContainerReference(ContainerHelper, InMonoObject);

	return true;
}

template <typename T>
auto FCSharpBind::BindImplementation(MonoObject* InMonoObject)
{
	const auto Helper = new T();

	FCSharpEnvironment::GetEnvironment().AddDelegateReference(Helper, InMonoObject);

	return true;
}
