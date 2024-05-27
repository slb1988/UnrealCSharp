// Fill out your copyright notice in the Description page of Project Settings.


#include "Asset/CSharpAsset.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Asset/CSharpFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Common/FUnrealCSharpFunctionLibrary.h"
#include "CoreMacro/Macro.h"

void UCSharpAsset::NewCSharpAsset()
{
#if 1
	FString Name = "789";

	// 创建一个包
	const FString PackagePath = FString("/Game/SampleTexture/") + Name;
	UPackage* Package = CreatePackage(*PackagePath);
	Package->FullyLoad();
	
	// 创建贴图
	UCSharpAsset* NewTexture = NewObject<UCSharpAsset>(Package, *Name, RF_Public|RF_Standalone|RF_MarkAsRootSet);
	
	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewTexture);
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
	bool bSaved = UPackage::SavePackage(Package, NewTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone | EObjectFlags::RF_Transient, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);

	auto x111 = 19;
	
#else
		FString Name = "123";

	FVector2D  Size = FVector2D (1024,1024);
	// 准备颜色信息数组
	TArray<FColor> Colors;
	for(int32 y = 0; y < Size.Y; y++)
	{
		for(int32 x = 0; x < Size.X; x++)
		{
			// 先加个纯蓝试试
			Colors.Add(FColor::Blue);
		}
	}

	// 创建一个包
	const FString PackagePath = FString("/Game/SampleTexture/") + Name;
	UPackage* Package = CreatePackage(*PackagePath);
	Package->FullyLoad();
	
	// 创建贴图
	UTexture2D* NewTexture = NewObject<UTexture2D>(Package, *Name, RF_Public|RF_Standalone|RF_MarkAsRootSet);
	NewTexture->PlatformData = new FTexturePlatformData();
	// 设置贴图大小以及格式
	NewTexture->PlatformData->SizeX = Size.X;
	NewTexture->PlatformData->SizeY = Size.Y;
	NewTexture->PlatformData->SetNumSlices(1);
	NewTexture->PlatformData->PixelFormat = PF_B8G8R8A8;
	// 防止被垃圾回收删除
	NewTexture->AddToRoot();
	
	// 写入颜色信息
	// 储存像素数据的数组，因为有RGBA四个数据，所以数组大小要乘以4
	uint8* Pixels = new uint8[Size.X * Size.Y * 4];
	for (int y = 0; y < Size.Y; y++)
	{
		for (int x = 0; x < Size.X; x++)
		{
			const int32 CurrentPixelIndex = y * Size.X + x;
			Pixels[CurrentPixelIndex * 4 + 0] = Colors[CurrentPixelIndex].B;
			Pixels[CurrentPixelIndex * 4 + 1] = Colors[CurrentPixelIndex].G;
			Pixels[CurrentPixelIndex * 4 + 2] = Colors[CurrentPixelIndex].R;
			Pixels[CurrentPixelIndex * 4 + 3] = Colors[CurrentPixelIndex].A;
		}
	}
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	NewTexture->PlatformData->Mips.Add(Mip);
	Mip->SizeX = Size.X;
	Mip->SizeY = Size.Y;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = static_cast<uint8*>(Mip->BulkData.Realloc(Size.X * Size.Y * 4));
	FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * Size.X * Size.Y * 4);
	Mip->BulkData.Unlock();
	
	// 更细贴图并创建Asset
	NewTexture->Source.Init(Size.X, Size.Y, 1, 1, TSF_BGRA8, Pixels);
	NewTexture->UpdateResource();
	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewTexture);
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
	bool bSaved = UPackage::SavePackage(Package, NewTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);
    
	// 删除数组
	delete [] Pixels;
#endif

	// auto x = 10;
	//
	// FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	//
	// UCSharpFactory* FlipbookFactory = NewObject<UCSharpFactory>();
	//
	// // ContentBrowserModule.Get().CreateNewAsset("123", "/Game/123", UCSharpAsset::StaticClass(),FlipbookFactory);
	//
	// TMap<FString, FString> CodeAnalysisOverrideFilesMap;
	//
	// CodeAnalysisOverrideFilesMap = FUnrealCSharpFunctionLibrary::LoadFileToString(FString::Printf(TEXT(
	// 	"%s/%s.json"
	// ),
	// 	*FUnrealCSharpFunctionLibrary::GetCodeAnalysisPath(),
	// 	*OVERRIDE_FILE
	// ));
	//
	// for(auto File : CodeAnalysisOverrideFilesMap)
	// {
	// 	auto Filename= FPaths::GetBaseFilename(File.Value);
	//
	// 	auto x2 = 10;
	//
	// 	// ContentBrowserModule.Get().CreateNewAsset(Filename, FString::Printf(TEXT(
	// 	// 	"/Game/%s"),
	// 	// 	*Filename), UCSharpAsset::StaticClass(),FlipbookFactory);
	//
	// 	// ContentBrowserModule.Get().CreateNewAsset(Filename, "/Game",
	//  // UCSharpAsset::StaticClass(),FlipbookFactory);
	// 	//创建名为 PackageName 值的包
	// 	//PackageName 为 FString 类型
	// 	FString AssetPath = TEXT("/Game/")+ Filename+ TEXT("/");
	// 	AssetPath += Filename;
	// 	UPackage* Package = CreatePackage(NULL, *AssetPath);
	// 	Package->FullyLoad();
	// 	UCSharpAsset* NewTexture = NewObject<UCSharpAsset>(Package, *Filename, RF_Public | RF_Standalone | RF_MarkAsRootSet);
	//
	// 	Package->MarkPackageDirty();
	// 	FAssetRegistryModule::AssetCreated(NewTexture);
	// 	//通过 asset 路径获取包中文件名
	// 	FString PackageFileName = FPackageName::LongPackageNameToFilename(AssetPath, FPackageName::GetAssetPackageExtension());
	// 	//进行保存
	// 	bool bSaved = UPackage::SavePackage(Package, NewTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);
	// }
}
