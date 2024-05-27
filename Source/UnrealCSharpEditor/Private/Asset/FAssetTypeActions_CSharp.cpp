#include "Asset/FAssetTypeActions_CSharp.h"

#include "SourceCodeNavigation.h"
#include "Asset/CSharpAsset.h"

UClass* FAssetTypeActions_CSharp::GetSupportedClass() const
{
	return UCSharpAsset::StaticClass();
}

FText FAssetTypeActions_CSharp::GetName() const
{
	return INVTEXT("Normal Distribution");
}

FColor FAssetTypeActions_CSharp::GetTypeColor() const
{
	return FColor::Cyan;
}

uint32 FAssetTypeActions_CSharp::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

void FAssetTypeActions_CSharp::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	FSourceCodeNavigation::OpenSourceFile(TEXT("D:/file/UnrealProject/UnrealCSharpTest5.0/Script/Game/UnrealCSharpTest/UnitTest/UnitTestSubsystem.cs"));
}
