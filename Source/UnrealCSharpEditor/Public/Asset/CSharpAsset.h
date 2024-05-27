// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSharpAsset.generated.h"

/**
 * 
 */
UCLASS()
class UNREALCSHARPEDITOR_API UCSharpAsset : public UObject
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, CallInEditor)
	static void NewCSharpAsset();
};
