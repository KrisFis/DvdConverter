
#pragma once

// AlternitySTD
#include <AltString.h>

// STD
#include <filesystem>

namespace FConverterUtilities
{
	FORCEINLINE sal::FString PathToString(const std::filesystem::path& Path) { return (char*)(Path.string().data()); }

	FORCEINLINE std::filesystem::path StringToPath(const sal::FString& String) { return std::filesystem::path((char*)(String)); }

	sal::FString ExtendFilename(const sal::FString& ParentDirectory, const std::filesystem::path& Path);

	sal::FString GetLastPart(const std::filesystem::path& Path);

	FORCEINLINE sal::FString GetLastPart(const sal::FString& String) { return GetLastPart(StringToPath(String)); }
}