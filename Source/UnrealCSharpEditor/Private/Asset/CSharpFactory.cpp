// Fill out your copyright notice in the Description page of Project Settings.


#include "Asset/CSharpFactory.h"
#include "Asset/CSharpAsset.h"
#include "AssetRegistry/AssetRegistryModule.h"

UCSharpFactory::UCSharpFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UCSharpAsset::StaticClass();
}

UObject* UCSharpFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	auto x=  NewObject<UCSharpAsset>(InParent, InClass, InName, Flags | RF_Transactional);

	FAssetRegistryModule::AssetCreated(x);

	x->MarkPackageDirty();

	FAssetRegistryModule::AssetSaved(*x);

	return x;
}
