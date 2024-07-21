#pragma once

#include "TPropertyClass.inl"
#include "Binding/TypeInfo/TTypeInfo.inl"
#include "Environment/FCSharpEnvironment.h"
#include "Reflection/Container/FArrayHelper.h"
#include "Reflection/Container/FSetHelper.h"
#include "Reflection/Container/FMapHelper.h"
#include "Template/TGetArrayLength.inl"
#include "Template/TTemplateTypeTraits.inl"
#include "Template/TIsTScriptInterface.inl"
#include "Template/TIsTLazyObjectPtr.inl"
#include "Template/TIsTSoftObjectPtr.inl"
#include "Template/TIsTSoftClassPtr.inl"
#include "Template/TIsUStruct.inl"
#include "Template/TIsNotUEnum.inl"
#include "Template/TIsTEnumAsByte.inl"
#include "UEVersion.h"
#if UE_F_OPTIONAL_PROPERTY
#include "Template/TIsTOptional.inl"
#include "UObject/PropertyOptional.h"
#endif

template <typename T, typename Enable = void>
struct TPropertyValue
{
};

template <typename T, typename Enable = void>
struct TBindingPropertyValue
{
};

template <typename T, typename Enable = void>
struct TScriptStructPropertyValue
{
};

template <typename T>
struct TPrimitivePropertyValue
{
	static MonoObject* Get(std::decay_t<T>* InMember,
		const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		return FCSharpEnvironment::GetEnvironment().GetDomain()->Value_Box(
			TPropertyClass<T, T>::Get(), InMember);
	}

	static MonoObject* Get(std::decay_t<T>* InMember, const bool bIsCopy)
	{
		return FCSharpEnvironment::GetEnvironment().GetDomain()->Value_Box(
			TPropertyClass<T, T>::Get(), InMember);
	}

	static T Set(MonoObject* InValue)
	{
		return *(std::decay_t<T>*)FCSharpEnvironment::GetEnvironment().GetDomain()->Object_Unbox(InValue);
	}
};

template <typename T>
struct TStringPropertyValue
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetStringObject<std::decay_t<T>>(InMember);

		if (SrcMonoObject == nullptr)
		{
			const auto FoundMonoClass = TPropertyClass<T, T>::Get();

			SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().AddStringReference<std::decay_t<T>>(SrcMonoObject, InMember, false);
		}

		return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember, const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		if(bIsCopy)
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().AddStringReference<std::decay_t<T>>(SrcMonoObject, new std::decay_t<T>(*InMember),
				true);

			return SrcMonoObject;
		}
		else
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetStringObject<std::decay_t<T>>(InMember);

			if (SrcMonoObject == nullptr)
			{
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				FCSharpEnvironment::GetEnvironment().AddStringReference<std::decay_t<T>>(SrcMonoObject, InMember, false);
			}

			return SrcMonoObject;
		}
	}

	static std::decay_t<T> Set(const FGarbageCollectionHandle InValue)
	{
		return std::decay_t<T>(*FCSharpEnvironment::GetEnvironment().GetString<std::decay_t<T>>(InValue));
	}
};

template <typename T>
struct TMultiPropertyValue
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetMultiObject<std::decay_t<T>>(InMember);

		if (SrcMonoObject == nullptr)
		{
			const auto FoundMonoClass = TPropertyClass<T, T>::Get();

			SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().AddMultiReference<std::decay_t<T>>(
				SrcMonoObject, InMember,false);
		}

		return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember,const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		if(bIsCopy)
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().AddMultiReference<std::decay_t<T>>(SrcMonoObject, new std::decay_t<T>(*InMember),
				true);

			return SrcMonoObject;
		}
		else
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetMultiObject<std::decay_t<T>>(InMember);

			if (SrcMonoObject == nullptr)
			{
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				FCSharpEnvironment::GetEnvironment().AddMultiReference<std::decay_t<T>>(
					SrcMonoObject, InMember,false);
			}

			return SrcMonoObject;
		}
	}

	static T Set(const FGarbageCollectionHandle InValue)
	{
		return *(std::decay_t<T>*)FCSharpEnvironment::GetEnvironment().GetMulti<std::decay_t<T>>(InValue);
	}
};

template <typename T>
struct TBindingPropertyValue<T, std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetBinding(InMember);

		if (SrcMonoObject == nullptr)
		{
			const auto FoundMonoClass = TPropertyClass<T, T>::Get();

			SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().AddBindingReference(
					InGarbageCollectionHandle, SrcMonoObject, InMember);
		}

		return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember,const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		if(bIsCopy)
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().AddBindingReference<std::decay_t<T>>(SrcMonoObject, new std::decay_t<T>(*InMember),
				true);

			return SrcMonoObject;
		}
		else
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetBinding(InMember);

			if (SrcMonoObject == nullptr)
			{
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				FCSharpEnvironment::GetEnvironment().AddBindingReference(
					SrcMonoObject, InMember,false);
			}

			return SrcMonoObject;
		}
	}

	static T Set(const FGarbageCollectionHandle InValue)
	{
		return *FCSharpEnvironment::GetEnvironment().GetBinding<std::decay_t<T>>(InValue);
	}
};

template <typename T>
struct TBindingPropertyValue<T, std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetBinding(*InMember);

		if (SrcMonoObject == nullptr)
		{
			const auto FoundMonoClass = TPropertyClass<T, T>::Get();

			SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().AddBindingReference(
					InGarbageCollectionHandle, SrcMonoObject, *InMember);
		}

		return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember,const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		if(bIsCopy)
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().AddBindingReference<std::decay_t<T>>(SrcMonoObject, new std::decay_t<T>(*InMember),
				true);

			return SrcMonoObject;
		}
		else
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetBinding(*InMember);

			if (SrcMonoObject == nullptr)
			{
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				FCSharpEnvironment::GetEnvironment().AddBindingReference(
						SrcMonoObject, *InMember, false);
			}

			return SrcMonoObject;
		}
	}

	static std::decay_t<T> Set(const FGarbageCollectionHandle InValue)
	{
		return FCSharpEnvironment::GetEnvironment().GetBinding<
			std::remove_pointer_t<std::remove_reference_t<T>>>(InValue);
	}
};

template <typename T>
struct TScriptStructPropertyValue<T, std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetObject(
				TBaseStructure<std::decay_t<T>>::Get(), InMember);

		if (SrcMonoObject == nullptr)
		{
			const auto FoundMonoClass = TPropertyClass<T, T>::Get();

			SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().Bind(TBaseStructure<std::decay_t<T>>::Get(), false);

			FCSharpEnvironment::GetEnvironment().AddStructReference(InGarbageCollectionHandle,
																		TBaseStructure<std::decay_t<T>>::Get(),
																		InMember,
																		SrcMonoObject);
		}

		return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember,const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		if(bIsCopy)
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().Bind(TBaseStructure<std::decay_t<T>>::Get(), false);

			FCSharpEnvironment::GetEnvironment().AddStructReference(TBaseStructure<std::decay_t<T>>::Get(),
				new std::decay_t<T>(*InMember),SrcMonoObject,
				true);

			return SrcMonoObject;
		}
		else
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetObject(
				TBaseStructure<std::decay_t<T>>::Get(), InMember);

			if (SrcMonoObject == nullptr)
			{
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				FCSharpEnvironment::GetEnvironment().Bind(TBaseStructure<std::decay_t<T>>::Get(), false);
				
FCSharpEnvironment::GetEnvironment().AddStructReference(TBaseStructure<std::decay_t<T>>::Get(),
																			InMember,
																			SrcMonoObject,
																			false);
			}

			return SrcMonoObject;
		}
	}

	static T Set(const FGarbageCollectionHandle InValue)
	{
		return *(std::decay_t<T>*)FCSharpEnvironment::GetEnvironment().GetStruct(InValue);
	}
};

template <typename T>
struct TScriptStructPropertyValue<T, std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetObject(
				TBaseStructure<std::decay_t<std::remove_pointer_t<T>>>::Get(), *InMember);

		if (SrcMonoObject == nullptr)
		{
			SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().Bind(TBaseStructure<
														  std::decay_t<std::remove_pointer_t<T>>>::Get(), false);

			FCSharpEnvironment::GetEnvironment().AddStructReference(InGarbageCollectionHandle,
																		TBaseStructure<
																			std::decay_t<std::remove_pointer_t<
																				T>>>::Get(),
																		*InMember,
																		SrcMonoObject);
		}

		return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember, const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		if(bIsCopy)
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().Bind(TBaseStructure<
															  std::decay_t<std::remove_pointer_t<T>>>::Get(), false);

			FCSharpEnvironment::GetEnvironment().AddStructReference(TBaseStructure<
																				std::decay_t<std::remove_pointer_t<
																					T>>>::Get(),
																			new std::decay_t<T>(*InMember),
																			SrcMonoObject,
																			true);

			return SrcMonoObject;
		}
		else
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetObject(
				TBaseStructure<std::decay_t<std::remove_pointer_t<T>>>::Get(), *InMember);

			if (SrcMonoObject == nullptr)
			{
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				FCSharpEnvironment::GetEnvironment().Bind(TBaseStructure<
															  std::decay_t<std::remove_pointer_t<T>>>::Get(), false);

				FCSharpEnvironment::GetEnvironment().AddStructReference(TBaseStructure<
																				std::decay_t<std::remove_pointer_t<
																					T>>>::Get(),
																			*InMember,
																			SrcMonoObject,
																			false);
			}

			return SrcMonoObject;
		}
	}

	static std::decay_t<T> Set(const FGarbageCollectionHandle InValue)
	{
		return (std::decay_t<T>)FCSharpEnvironment::GetEnvironment().GetStruct(InValue);
	}
};

template <typename T>
struct TBindingEnumPropertyValue :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, uint8>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, uint16>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, uint32>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, uint64>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, int8>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, int16>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, int32>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, int64>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, bool>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, float>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T,
                      std::enable_if_t<std::is_base_of_v<UObject, std::remove_pointer_t<std::decay_t<T>>> &&
                                       !std::is_same_v<std::remove_pointer_t<std::decay_t<T>>, UClass>, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember, const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		return FCSharpEnvironment::GetEnvironment().Bind(
			std::add_pointer_t<std::remove_const_t<std::remove_pointer_t<std::decay_t<T>>>>(*InMember), false);
	}

	static MonoObject* Get(std::decay_t<T>* InMember, const bool bIsCopy)
	{
		return FCSharpEnvironment::GetEnvironment().Bind(
			std::add_pointer_t<std::remove_const_t<std::remove_pointer_t<std::decay_t<T>>>>(*InMember), bIsCopy);
	}

	static std::decay_t<T> Set(const FGarbageCollectionHandle InValue)
	{
		return FCSharpEnvironment::GetEnvironment().GetObject<std::remove_pointer_t<std::decay_t<T>>>(InValue);
	}
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTObjectPtr<T>::Value, T>>
{
	static MonoObject* Get(T* InMember, const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		return FCSharpEnvironment::GetEnvironment().Bind(*InMember, false);
	}
	
	static MonoObject* Get(T* InMember, const bool bIsCopy)
	{
		return FCSharpEnvironment::GetEnvironment().Bind(*InMember, bIsCopy);
	}

	static T Set(const FGarbageCollectionHandle InValue)
	{
		return FCSharpEnvironment::GetEnvironment().GetObject<typename T::ElementType>(InValue);
	}
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, FName>, T>> :
	TStringPropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTScriptInterface<std::decay_t<T>>::Value, T>> :
	TMultiPropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsUStruct<std::decay_t<T>>::Value, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	                       const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetObject(std::decay_t<T>::StaticStruct(), InMember);

		if (SrcMonoObject == nullptr)
		{
			const auto FoundMonoClass = TPropertyClass<T, T>::Get();

			SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().Bind(std::decay_t<T>::StaticStruct(), false);

			FCSharpEnvironment::GetEnvironment().AddStructReference(
					InGarbageCollectionHandle, std::decay_t<T>::StaticStruct(), InMember, SrcMonoObject);
		}

		return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember,const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		if(bIsCopy)
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			FCSharpEnvironment::GetEnvironment().Bind(std::decay_t<T>::StaticStruct(), false);

			FCSharpEnvironment::GetEnvironment().AddStructReference(std::decay_t<T>::StaticStruct(),
																			new std::decay_t<T>(*InMember),
																			SrcMonoObject,
																			true);

			return SrcMonoObject;
		}
		else
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetObject(std::decay_t<T>::StaticStruct(), InMember);

			if (SrcMonoObject == nullptr)
			{
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				FCSharpEnvironment::GetEnvironment().Bind(std::decay_t<T>::StaticStruct(), false);

				FCSharpEnvironment::GetEnvironment().AddStructReference(
						std::decay_t<T>::StaticStruct(), InMember, SrcMonoObject,
						false);
			}

			return SrcMonoObject;
		}
	}

	static T Set(const FGarbageCollectionHandle InValue)
	{
		return *(std::decay_t<T>*)FCSharpEnvironment::GetEnvironment().GetStruct(InValue);
	}
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, FString>, T>> :
	TStringPropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, FText>, T>> :
	TStringPropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTWeakObjectPtr<std::decay_t<T>>::Value, T>> :
	TMultiPropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTLazyObjectPtr<std::decay_t<T>>::Value, T>> :
	TMultiPropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTSoftObjectPtr<std::decay_t<T>>::Value, T>> :
	TMultiPropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::decay_t<T>, double>, T>> :
	TPrimitivePropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTMap<std::decay_t<T>>::Value, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	                       const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetContainerObject<FMapHelper>(InMember);

		if (SrcMonoObject == nullptr)
		{
			const auto FoundMonoClass = TPropertyClass<T, T>::Get();

			const auto FoundKeyPropertyMonoClass = TPropertyClass<
							typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>,
							typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>>
						::Get();

			const auto FoundKeyPropertyMonoType = FCSharpEnvironment::GetEnvironment().GetDomain()->Class_Get_Type(
				FoundKeyPropertyMonoClass);

			const auto FoundKeyPropertyReflectionType = FCSharpEnvironment::GetEnvironment().GetDomain()->
				Type_Get_Object(FoundKeyPropertyMonoType);

			const auto FoundValuePropertyMonoClass = TPropertyClass<
					typename TTemplateTypeTraits<std::decay_t<T>>::template Type<1>,
					typename TTemplateTypeTraits<std::decay_t<T>>::template Type<1>>
				::Get();

			const auto FoundValuePropertyMonoType = FCSharpEnvironment::GetEnvironment().GetDomain()->
				Class_Get_Type(FoundValuePropertyMonoClass);

			const auto FoundValuePropertyReflectionType = FCSharpEnvironment::GetEnvironment().GetDomain()->
				Type_Get_Object(FoundValuePropertyMonoType);

			const auto KeyProperty = FTypeBridge::Factory(FoundKeyPropertyReflectionType, nullptr, "",
																  EObjectFlags::RF_Transient);

			KeyProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);

			const auto ValueProperty = FTypeBridge::Factory(FoundValuePropertyReflectionType, nullptr, "",
															EObjectFlags::RF_Transient);

			ValueProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);
			
			SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			const auto MapHelper = new FMapHelper(KeyProperty, ValueProperty, InMember,
												  false, true);

			FCSharpEnvironment::GetEnvironment().AddContainerReference(
					InGarbageCollectionHandle, InMember, MapHelper, SrcMonoObject);
		}

		return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember,const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		const auto FoundKeyPropertyMonoClass = TPropertyClass<
						typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>,
						typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>>
					::Get();

		const auto FoundKeyPropertyMonoType = FCSharpEnvironment::GetEnvironment().GetDomain()->Class_Get_Type(
			FoundKeyPropertyMonoClass);

		const auto FoundKeyPropertyReflectionType = FCSharpEnvironment::GetEnvironment().GetDomain()->
			Type_Get_Object(FoundKeyPropertyMonoType);

		const auto FoundValuePropertyMonoClass = TPropertyClass<
				typename TTemplateTypeTraits<std::decay_t<T>>::template Type<1>,
				typename TTemplateTypeTraits<std::decay_t<T>>::template Type<1>>
			::Get();

		const auto FoundValuePropertyMonoType = FCSharpEnvironment::GetEnvironment().GetDomain()->
			Class_Get_Type(FoundValuePropertyMonoClass);

		const auto FoundValuePropertyReflectionType = FCSharpEnvironment::GetEnvironment().GetDomain()->
			Type_Get_Object(FoundValuePropertyMonoType);

		const auto KeyProperty = FTypeBridge::Factory(FoundKeyPropertyReflectionType, nullptr, "",
															  EObjectFlags::RF_Transient);

		KeyProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);

		const auto ValueProperty = FTypeBridge::Factory(FoundValuePropertyReflectionType, nullptr, "",
														EObjectFlags::RF_Transient);

		ValueProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);
		
		if(bIsCopy)
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			const auto MapHelper = new FMapHelper(KeyProperty, ValueProperty, new std::decay_t<T>(*InMember),
													  true, true);

			FCSharpEnvironment::GetEnvironment().AddContainerReference(MapHelper, SrcMonoObject);

			return SrcMonoObject;
		}
		else
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetContainerObject<FMapHelper>(InMember);

			if (SrcMonoObject == nullptr)
			{
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				const auto MapHelper = new FMapHelper(KeyProperty, ValueProperty, InMember,
													  false, true);

				FCSharpEnvironment::GetEnvironment().AddContainerReference(MapHelper, SrcMonoObject);
			}

			return SrcMonoObject;
		}
	}

	static std::decay_t<T> Set(const FGarbageCollectionHandle InValue)
	{
		std::decay_t<T> Value;

		const auto SrcContainer = FCSharpEnvironment::GetEnvironment().GetContainer<FMapHelper>(InValue);

		for (auto Index = 0; Index < SrcContainer->GetMaxIndex(); ++Index)
		{
			if (SrcContainer->IsValidIndex(Index))
			{
				Value.Add(*static_cast<typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>*>
				          (SrcContainer->GetEnumeratorKey(Index)),
				          *static_cast<typename TTemplateTypeTraits<std::decay_t<T>>::template Type<1>*>
				          (SrcContainer->GetEnumeratorValue(Index)));
			}
		}

		return Value;
	}
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTSet<std::decay_t<T>>::Value, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	                       const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetContainerObject<FSetHelper>(InMember);

			if (SrcMonoObject == nullptr)
			{
				const auto FoundMonoClass = TPropertyClass<T, T>::Get();

				const auto FoundPropertyMonoClass = TPropertyClass<
						typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>,
						typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>>
					::Get();

				const auto FoundPropertyMonoType = FCSharpEnvironment::GetEnvironment().GetDomain()->Class_Get_Type(
					FoundPropertyMonoClass);

				const auto FoundPropertyReflectionType = FCSharpEnvironment::GetEnvironment().GetDomain()->
					Type_Get_Object(FoundPropertyMonoType);

				const auto Property = FTypeBridge::Factory(FoundPropertyReflectionType, nullptr, "",
																   EObjectFlags::RF_Transient);

				Property->SetPropertyFlags(CPF_HasGetValueTypeHash);
				
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				const auto SetHelper = new FSetHelper(Property, InMember, false, true);

				FCSharpEnvironment::GetEnvironment().AddContainerReference(
						InGarbageCollectionHandle, InMember, SetHelper, SrcMonoObject);
			}

			return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember,const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		const auto FoundPropertyMonoClass = TPropertyClass<
				typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>,
				typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>>
			::Get();

		const auto FoundPropertyMonoType = FCSharpEnvironment::GetEnvironment().GetDomain()->Class_Get_Type(
			FoundPropertyMonoClass);

		const auto FoundPropertyReflectionType = FCSharpEnvironment::GetEnvironment().GetDomain()->
			Type_Get_Object(FoundPropertyMonoType);

		const auto Property = FTypeBridge::Factory(FoundPropertyReflectionType, nullptr, "",
														   EObjectFlags::RF_Transient);

		Property->SetPropertyFlags(CPF_HasGetValueTypeHash);
		
		if(bIsCopy)
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			const auto SetHelper = new FSetHelper(Property, new std::decay_t<T>(*InMember), true, true);

			FCSharpEnvironment::GetEnvironment().AddContainerReference(SetHelper, SrcMonoObject);

			return SrcMonoObject;
		}
		else
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetContainerObject<FSetHelper>(InMember);

			if (SrcMonoObject == nullptr)
			{
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				const auto SetHelper = new FSetHelper(Property, InMember, false, true);

				FCSharpEnvironment::GetEnvironment().AddContainerReference(SetHelper, SrcMonoObject);
			}

			return SrcMonoObject;
		}
	}

	static std::decay_t<T> Set(const FGarbageCollectionHandle InValue)
	{
		std::decay_t<T> Value;

		const auto SrcContainer = FCSharpEnvironment::GetEnvironment().GetContainer<FSetHelper>(InValue);

		for (auto Index = 0; Index < SrcContainer->GetMaxIndex(); ++Index)
		{
			if (SrcContainer->IsValidIndex(Index))
			{
				Value.Add(
					*static_cast<typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>*>(SrcContainer->
						GetEnumerator(Index)));
			}
		}

		return Value;
	}
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTSubclassOf<std::decay_t<T>>::Value, T>> :
	TMultiPropertyValue<T>
{
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<std::is_same_v<std::remove_pointer_t<std::decay_t<T>>, UClass>, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember, const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

		auto SubclassOf = new TSubclassOf<UObject>(*InMember);

		FCSharpEnvironment::GetEnvironment().AddMultiReference<TSubclassOf<UObject>>(
			SrcMonoObject, SubclassOf, true);

		return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember, const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

		auto SubclassOf = new TSubclassOf<UObject>(*InMember);

		FCSharpEnvironment::GetEnvironment().AddMultiReference<TSubclassOf<UObject>>(
			SrcMonoObject, SubclassOf, true);

		return SrcMonoObject;
	}

	static std::decay_t<T> Set(const FGarbageCollectionHandle InValue)
	{
		return FCSharpEnvironment::GetEnvironment().GetMulti<TSubclassOf<UObject>>(InValue)->Get();
	}
};


template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTArray<std::decay_t<T>>::Value, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	                       const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetContainerObject<FArrayHelper>(InMember);

		if (SrcMonoObject == nullptr)
		{
			const auto FoundMonoClass = TPropertyClass<T, T>::Get();

			const auto FoundPropertyMonoClass = TPropertyClass<
					typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>,
					typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>>
				::Get();

			const auto FoundPropertyMonoType = FCSharpEnvironment::GetEnvironment().GetDomain()->Class_Get_Type(
				FoundPropertyMonoClass);

			const auto FoundPropertyReflectionType = FCSharpEnvironment::GetEnvironment().GetDomain()->
				Type_Get_Object(FoundPropertyMonoType);

			const auto Property = FTypeBridge::Factory(FoundPropertyReflectionType, nullptr, "",
															   EObjectFlags::RF_Transient);

			Property->SetPropertyFlags(CPF_HasGetValueTypeHash);
			
			SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			const auto ArrayHelper = new FArrayHelper(Property, InMember, false, true);

			FCSharpEnvironment::GetEnvironment().AddContainerReference(
					InGarbageCollectionHandle, InMember, ArrayHelper, SrcMonoObject);
		}

		return SrcMonoObject;
	}

	static MonoObject* Get(std::decay_t<T>* InMember,const bool bIsCopy)
	{
		const auto FoundMonoClass = TPropertyClass<T, T>::Get();

		const auto FoundPropertyMonoClass = TPropertyClass<
				typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>,
				typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>>
			::Get();

		const auto FoundPropertyMonoType = FCSharpEnvironment::GetEnvironment().GetDomain()->Class_Get_Type(
			FoundPropertyMonoClass);

		const auto FoundPropertyReflectionType = FCSharpEnvironment::GetEnvironment().GetDomain()->
			Type_Get_Object(FoundPropertyMonoType);

		const auto Property = FTypeBridge::Factory(FoundPropertyReflectionType, nullptr, "",
														   EObjectFlags::RF_Transient);

		Property->SetPropertyFlags(CPF_HasGetValueTypeHash);
		
		if(bIsCopy)
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			const auto ArrayHelper = new FArrayHelper(Property, new std::decay_t<T>(*InMember), true, true);

			FCSharpEnvironment::GetEnvironment().AddContainerReference(ArrayHelper, SrcMonoObject);

			return SrcMonoObject;
		}
		else
		{
			auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetContainerObject<FArrayHelper>(InMember);

			if (SrcMonoObject == nullptr)
			{
				SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

				const auto ArrayHelper = new FArrayHelper(Property, InMember, false, true);

				FCSharpEnvironment::GetEnvironment().AddContainerReference(ArrayHelper, SrcMonoObject);
			}

			return SrcMonoObject;
		}
	}

	static std::decay_t<T> Set(const FGarbageCollectionHandle InValue)
	{
		const auto SrcContainer = FCSharpEnvironment::GetEnvironment().GetContainer<FArrayHelper>(InValue);

		return std::decay_t<T>(
			static_cast<typename TTemplateTypeTraits<std::decay_t<T>>::template Type<0>*>
			(SrcContainer->GetScriptArray()->GetData()),
			SrcContainer->Num());
	}
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsEnum<std::decay_t<T>>::Value && !TIsNotUEnum<std::decay_t<T>>::Value, T>> :
	TPrimitivePropertyValue<T>
{
	static MonoObject* Get(std::decay_t<T>* InMember, const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		return FCSharpEnvironment::GetEnvironment().GetDomain()->Value_Box(
			TPropertyClass<T, T>::Get(), InMember);
	}

	static MonoObject* Get(std::decay_t<T>* InMember,const bool bIsCopy)
	{
		return FCSharpEnvironment::GetEnvironment().GetDomain()->Value_Box(
			TPropertyClass<T, T>::Get(), InMember);
	}
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTEnumAsByte<std::decay_t<T>>::Value, T>> :
	TPrimitivePropertyValue<T>
{
	static MonoObject* Get(std::decay_t<T>* InMember, const FGarbageCollectionHandle& InGarbageCollectionHandle)
	{
		return FCSharpEnvironment::GetEnvironment().GetDomain()->Value_Box(
			TPropertyClass<T, T>::Get(), InMember);
	}

	static MonoObject* Get(std::decay_t<T>* InMember, const bool bIsCopy)
	{
		return FCSharpEnvironment::GetEnvironment().GetDomain()->Value_Box(
			TPropertyClass<T, T>::Get(), InMember);
	}
};

template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTSoftClassPtr<std::decay_t<T>>::Value, T>> :
	TMultiPropertyValue<T>
{
};

#if UE_F_OPTIONAL_PROPERTY
template <typename T>
struct TPropertyValue<T, std::enable_if_t<TIsTOptional<std::decay_t<T>>::Value, T>>
{
	static MonoObject* Get(std::decay_t<T>* InMember,
	                       const FGarbageCollectionHandle& InGarbageCollectionHandle = FGarbageCollectionHandle::Zero(),
	                       bool bNeedFree = true)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetOptionalObject<FOptionalHelper>(InMember);

		if (SrcMonoObject == nullptr)
		{
			const auto FoundMonoClass = TPropertyClass<T, T>::Get();

			const auto FoundPropertyMonoClass = TPropertyClass<
					typename TTemplateTypeTraits<std::decay_t<T>>::Type,
					typename TTemplateTypeTraits<std::decay_t<T>>::Type>
				::Get();

			const auto FoundPropertyMonoType = FCSharpEnvironment::GetEnvironment().GetDomain()->Class_Get_Type(
				FoundPropertyMonoClass);

			const auto FoundPropertyReflectionType = FCSharpEnvironment::GetEnvironment().GetDomain()->
				Type_Get_Object(FoundPropertyMonoType);

			SrcMonoObject = FCSharpEnvironment::GetEnvironment().GetDomain()->Object_New(FoundMonoClass);

			const auto OptionalProperty = new FOptionalProperty(nullptr, "", EObjectFlags::RF_Transient);

			const auto Property = FTypeBridge::Factory(FoundPropertyReflectionType, nullptr, "",
			                                           EObjectFlags::RF_Transient);

			Property->SetPropertyFlags(CPF_HasGetValueTypeHash);

			OptionalProperty->SetValueProperty(Property);

			const auto OptionalHelper = new FOptionalHelper(OptionalProperty, InMember,
			                                                !InGarbageCollectionHandle.IsValid(), true);

			if (InGarbageCollectionHandle.IsValid())
			{
				FCSharpEnvironment::GetEnvironment().AddOptionalReference(
					InMember, OptionalHelper, SrcMonoObject);
			}
			else
			{
				FCSharpEnvironment::GetEnvironment().AddOptionalReference(OptionalHelper, SrcMonoObject);
			}
		}

		return SrcMonoObject;
	}

	static std::decay_t<T> Set(const FGarbageCollectionHandle InValue)
	{
		const auto SrcOptional = FCSharpEnvironment::GetEnvironment().GetOptional(InValue);

		return *static_cast<typename TTemplateTypeTraits<std::decay_t<T>>::Type*>(SrcOptional->GetData());
	}
};
#endif
