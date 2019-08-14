
#include "ConverterUtilities.h"

#include <AltString.h>
#include <filesystem>
#include <string.h>

namespace FConverterUtilities
{
	sal::FString ExtendFilename(const sal::FString& ParentDirectory, const std::filesystem::path& Path)
	{
		sal::FString filename = ParentDirectory;
		filename += "/";
		filename += (char*)(Path.filename().string().data());

		return filename;
	}

	sal::FString GetLastPart(const std::filesystem::path& Path)
	{
		if (std::filesystem::is_directory(Path))
		{
			return (char*)(Path.filename().string().data());
		}

		std::string str = Path.string();

		std::size_t foundIdx = str.find_last_of('/');
		if (foundIdx != std::string::npos)
		{
			return (char*)(str.substr(++foundIdx).data());
		}

		return (char*)(Path.string().data());
	}

}