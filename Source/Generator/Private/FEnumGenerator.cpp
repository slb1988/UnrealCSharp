﻿#include "FEnumGenerator.h"
#include "FGeneratorCore.h"
#include "Engine/UserDefinedEnum.h"
#include "Common/FUnrealCSharpFunctionLibrary.h"
#include "Dynamic/CSharpEnum.h"

TMap<const UEnum*, EEnumUnderlyingType> FEnumGenerator::EnumUnderlyingType;

void FEnumGenerator::Generator()
{
	for (TObjectIterator<UEnum> EnumIterator; EnumIterator; ++EnumIterator)
	{
		Generator(*EnumIterator);
	}

	GeneratorCollisionChannel();
}

void FEnumGenerator::Generator(const UEnum* InEnum)
{
	if (InEnum == nullptr)
	{
		return;
	}

	if (Cast<UCSharpEnum>(InEnum))
	{
		return;
	}

	const auto UserDefinedEnum = Cast<UUserDefinedEnum>(InEnum);

	FString UsingNameSpaceContent;

	const auto NameSpaceContent = FUnrealCSharpFunctionLibrary::GetClassNameSpace(InEnum);

	if (!FGeneratorCore::IsSupportedModule(NameSpaceContent))
	{
		return;
	}

	const auto PathNameAttributeContent = FGeneratorCore::GetPathNameAttribute(InEnum);

	const auto FullClassContent = FUnrealCSharpFunctionLibrary::GetFullClass(InEnum);

	FString EnumeratorContent;

	auto ClassName = InEnum->GetName();

	TSet<FString> UsingNameSpaces{TEXT("System"), TEXT("Script.Common")};

	for (auto Index = 0; Index < InEnum->NumEnums(); ++Index)
	{
		const auto EnumeratorValue = InEnum->GetValueByIndex(Index);

		if (EnumeratorValue == InEnum->GetMaxEnumValue())
		{
			break;
		}

		auto EnumeratorString = UserDefinedEnum != nullptr
			                        ? UserDefinedEnum->GetDisplayNameTextByIndex(Index).ToString()
			                        : InEnum->GetNameStringByIndex(Index);

		EnumeratorContent += FString::Printf(TEXT(
			"\t\t%s = %lld%s\n"
		),
		                                     *FGeneratorCore::GetName(
			                                     FUnrealCSharpFunctionLibrary::Encode(EnumeratorString)),
		                                     EnumeratorValue, Index == InEnum->NumEnums() - 1 ? TEXT("") : TEXT(","));
	}

	for (auto UsingNameSpace : UsingNameSpaces)
	{
		UsingNameSpaceContent += FString::Printf(TEXT(
			"using %s;\n"
		),
		                                         *UsingNameSpace);
	}

	const auto Content = FString::Printf(TEXT(
		"%s\n"
		"namespace %s\n"
		"{\n"
		"\t[PathName(\"%s\")]\n"
		"\tpublic enum %s : %s\n"
		"\t{\n"
		"%s"
		"\t}\n"
		"}"
	),
	                                     *UsingNameSpaceContent,
	                                     *NameSpaceContent,
	                                     *PathNameAttributeContent,
	                                     *FullClassContent,
	                                     *GetEnumUnderlyingTypeName(InEnum),
	                                     *EnumeratorContent
	);

	auto ModuleName = FUnrealCSharpFunctionLibrary::GetModuleName(InEnum);

	auto DirectoryName = FPaths::Combine(FUnrealCSharpFunctionLibrary::GetGenerationPath(InEnum), ModuleName);

	const auto FileName = FPaths::Combine(DirectoryName, ClassName) + TEXT(".cs");

	FGeneratorCore::SaveStringToFile(FileName, Content);
}

void FEnumGenerator::AddEnumUnderlyingType(const UEnum* InEnum, const FNumericProperty* InNumericProperty)
{
	if (InEnum == nullptr || InNumericProperty == nullptr)
	{
		return;
	}

	if (EnumUnderlyingType.Contains(InEnum))
	{
		return;
	}

	auto UnderlyingType = EEnumUnderlyingType::None;

	if (CastField<FInt8Property>(InNumericProperty))
	{
		UnderlyingType = EEnumUnderlyingType::Int8;
	}
	else if (CastField<FByteProperty>(InNumericProperty))
	{
		UnderlyingType = EEnumUnderlyingType::UInt8;
	}
	else if (CastField<FInt16Property>(InNumericProperty))
	{
		UnderlyingType = EEnumUnderlyingType::Int16;
	}
	else if (CastField<FUInt16Property>(InNumericProperty))
	{
		UnderlyingType = EEnumUnderlyingType::UInt16;
	}
	else if (CastField<FIntProperty>(InNumericProperty))
	{
		UnderlyingType = EEnumUnderlyingType::Int;
	}
	else if (CastField<FUInt32Property>(InNumericProperty))
	{
		UnderlyingType = EEnumUnderlyingType::UInt32;
	}
	else if (CastField<FInt64Property>(InNumericProperty))
	{
		UnderlyingType = EEnumUnderlyingType::Int64;
	}
	else if (CastField<FUInt64Property>(InNumericProperty))
	{
		UnderlyingType = EEnumUnderlyingType::UInt64;
	}

	EnumUnderlyingType.Emplace(InEnum, UnderlyingType);
}

void FEnumGenerator::EmptyEnumUnderlyingType()
{
	EnumUnderlyingType.Empty();
}

void FEnumGenerator::GeneratorCollisionChannel()
{
	const auto InEnum = LoadObject<UEnum>(UCollisionProfile::StaticClass()->GetPackage(), TEXT("ECollisionChannel"));

	if (InEnum == nullptr)
	{
		return;
	}

	FString UsingNameSpaceContent;

	const auto NameSpaceContent = FUnrealCSharpFunctionLibrary::GetClassNameSpace(InEnum);

	const auto PathNameAttributeContent = FGeneratorCore::GetPathNameAttribute(InEnum);

	const auto FullClassContent = FUnrealCSharpFunctionLibrary::GetFullClass(InEnum);

	FString EnumeratorContent;

	auto ClassName = InEnum->GetName();

	TSet<FString> UsingNameSpaces{TEXT("System"), TEXT("Script.Common")};

	const auto CollisionProfile = UCollisionProfile::Get();

	for (auto Index = 0; Index < InEnum->NumEnums(); ++Index)
	{
		const auto EnumeratorValue = InEnum->GetValueByIndex(Index);

		if (EnumeratorValue == InEnum->GetMaxEnumValue())
		{
			break;
		}

		EnumeratorContent += FString::Printf(TEXT(
			"\t\t%s = %lld%s\n"
		),
		                                     *CollisionProfile->ReturnChannelNameFromContainerIndex(Index).ToString(),
		                                     EnumeratorValue, Index == InEnum->NumEnums() - 1 ? TEXT("") : TEXT(","));
	}

	for (auto UsingNameSpace : UsingNameSpaces)
	{
		UsingNameSpaceContent += FString::Printf(TEXT(
			"using %s;\n"
		),
		                                         *UsingNameSpace);
	}

	const auto Content = FString::Printf(TEXT(
		"%s\n"
		"namespace %s\n"
		"{\n"
		"\t[PathName(\"%s\")]\n"
		"\tpublic enum %s : %s\n"
		"\t{\n"
		"%s"
		"\t}\n"
		"}"
	),
	                                     *UsingNameSpaceContent,
	                                     *NameSpaceContent,
	                                     *PathNameAttributeContent,
	                                     *FullClassContent,
	                                     *GetEnumUnderlyingTypeName(InEnum),
	                                     *EnumeratorContent
	);

	auto ModuleName = FUnrealCSharpFunctionLibrary::GetModuleName(InEnum);

	auto DirectoryName = FPaths::Combine(FUnrealCSharpFunctionLibrary::GetGenerationPath(InEnum), ModuleName);

	const auto FileName = FPaths::Combine(DirectoryName, ClassName) + TEXT(".cs");

	FGeneratorCore::SaveStringToFile(FileName, Content);
}

FString FEnumGenerator::GetEnumUnderlyingTypeName(const UEnum* InEnum)
{
	static TMap<EEnumUnderlyingType, FString> EnumUnderlyingTypeName
	{
		{EEnumUnderlyingType::None, TEXT("UInt64")},
		{EEnumUnderlyingType::Int8, TEXT("SByte")},
		{EEnumUnderlyingType::UInt8, TEXT("Byte")},
		{EEnumUnderlyingType::Int16, TEXT("Int16")},
		{EEnumUnderlyingType::UInt16, TEXT("UInt16")},
		{EEnumUnderlyingType::Int, TEXT("Int32")},
		{EEnumUnderlyingType::UInt32, TEXT("UInt32")},
		{EEnumUnderlyingType::Int64, TEXT("Int64")},
		{EEnumUnderlyingType::UInt64, TEXT("UInt64")}
	};

	if (const auto FoundEnumUnderlyingType = EnumUnderlyingType.Find(InEnum))
	{
		return EnumUnderlyingTypeName[*FoundEnumUnderlyingType];
	}

	return TEXT("Int64");
}
