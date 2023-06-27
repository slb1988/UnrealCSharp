﻿#include "Domain/InternalCall/FSubclassOfImplementation.h"
#include "Binding/Class/FBindingClassBuilder.h"
#include "Environment/FCSharpEnvironment.h"
#include "Macro/NamespaceMacro.h"
#include "Async/Async.h"

struct FRegisterSubclassOf
{
	FRegisterSubclassOf()
	{
		FBindingClassBuilder(TEXT("SubclassOf"), NAMESPACE_LIBRARY)
			.Function("Register",
			          static_cast<void*>(FSubclassOfImplementation::SubclassOf_RegisterImplementation))
			.Function("UnRegister",
			          static_cast<void*>(FSubclassOfImplementation::SubclassOf_UnRegisterImplementation))
			.Function("Get",
			          static_cast<void*>(FSubclassOfImplementation::SubclassOf_GetImplementation))
			.Register();
	}
};

static FRegisterSubclassOf RegisterSubclassOf;

void FSubclassOfImplementation::SubclassOf_RegisterImplementation(MonoObject* InMonoObject, const MonoObject* InClass)
{
	const auto FoundClass = FCSharpEnvironment::GetEnvironment().GetObject<UClass>(InClass);

	const auto SubclassOf = new TSubclassOf<UObject>(FoundClass);

	FCSharpEnvironment::GetEnvironment().AddMultiReference<TSubclassOf<UObject>>(InMonoObject, SubclassOf);
}

void FSubclassOfImplementation::SubclassOf_UnRegisterImplementation(const MonoObject* InMonoObject)
{
	AsyncTask(ENamedThreads::GameThread, [InMonoObject]
	{
		(void)FCSharpEnvironment::GetEnvironment().RemoveMultiReference<TSubclassOf<UObject>>(InMonoObject);
	});
}

void FSubclassOfImplementation::SubclassOf_GetImplementation(const MonoObject* InMonoObject, MonoObject** OutValue)
{
	const auto Multi = FCSharpEnvironment::GetEnvironment().GetMulti<TSubclassOf<UObject>>(InMonoObject);

	*OutValue = FCSharpEnvironment::GetEnvironment().Bind(Multi->Get());
}
