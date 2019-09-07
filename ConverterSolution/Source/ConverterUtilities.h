
#pragma once

// AlternitySTD
#include <AltString.h>
#include <SafeInt.h>

// STD
#include <filesystem>

namespace FConverterUtilities
{
	enum class EOperationMode : unsigned char
	{
		Convert = 0,
		OnlyPackage = 1
	};

	FORCEINLINE sal::FString PathToString(const std::filesystem::path& Path) { return (char*)(Path.string().data()); }

	FORCEINLINE std::filesystem::path StringToPath(const sal::FString& String) { return std::filesystem::path((char*)(String)); }

	sal::FString ExtendFilename(const sal::FString& ParentDirectory, const std::filesystem::path& Path);

	sal::FString GetLastPart(const std::filesystem::path& Path);

	FORCEINLINE sal::FString GetLastPart(const sal::FString& String) { return GetLastPart(StringToPath(String)); }
}