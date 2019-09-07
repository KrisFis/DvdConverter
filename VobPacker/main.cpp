
#pragma region Includes

#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <cstdio>

#pragma endregion

#pragma region Namespaces (using)

using namespace std;
using namespace filesystem;
using namespace chrono;

#pragma endregion

#pragma region Typedefs && Usings

typedef path FPath;
typedef string FString;

template<typename TType>
using TArray = vector<TType>;

#pragma endregion

#pragma region Macros

#define VOB_EXTENSION ".VOB"
#define ONE_GB 1048576000

#define ECHO(var) cout << var;
#define ECHOL(var) cout << var << endl;

#define ECHO_V(var) if(Verbose) { cout << var; }
#define ECHOL_V(var) if(Verbose) { cout << var << endl; }

#define PAUSE() system("PAUSE")

#pragma endregion

#pragma region Global Variables

bool Verbose = false;
bool RemoveFiles = true;

#pragma endregion

#pragma region Essential methods

FString ToUpper(const FString& InString)
{
	FString resultString;

	for (int i = 0; i < InString.size(); ++i)
	{
		resultString.push_back(toupper(InString[i]));
	}

	return resultString;
}

#pragma endregion

#pragma region Internal methods

FString Internal_CreateFile(const FPath& ParentDir, const FString& InExtension)
{
	FString newFile = ParentDir.string();
	newFile.append("\\");
	newFile.append(ParentDir.stem().string());
	newFile.append(InExtension);

	return newFile;
}

void Internal_RecursiveFindFirst(const FPath& CurrentDir, const FString& InExtension, TArray<FPath>& OutFoundDirs)
{
	for (const FPath& foundDir : directory_iterator(CurrentDir))
	{
		if (is_directory(foundDir))
		{
			Internal_RecursiveFindFirst(foundDir, InExtension, OutFoundDirs);
		}
		else // is file
		{
			if(!ToUpper(foundDir.extension().string()).compare(ToUpper(InExtension)))
			{
				ECHO_V("Nalezen film: ");
				ECHOL_V(foundDir);

				if (find(OutFoundDirs.begin(), OutFoundDirs.end(), CurrentDir) == OutFoundDirs.end()) // is not contains
				{
					OutFoundDirs.push_back(CurrentDir);
				}

				return;
			}
		}
	}
}

void Internal_CopyToFile(const FPath& FromFile, ofstream& ToFile)
{
	ifstream file(FromFile.string(), ios::binary);
	if (file.is_open())
	{
		ToFile << file.rdbuf();

		file.close();
	}
}

int Internal_GetStepNumber(const FPath& ParentDir, const FString& InExtension, const FString& OutputFile)
{
	int number = 0;

	bool isPacking = false;
	for (const FPath& foundFile : directory_iterator(ParentDir))
	{
		if (is_directory(foundFile)) continue; // only files

		if(ToUpper(foundFile.extension().string()).compare(ToUpper(InExtension))) continue;

		if (!foundFile.string().compare(OutputFile)) continue;

		bool packThis = false;
		if (file_size(foundFile) >= ONE_GB)
		{
			if (!foundFile.string().compare(OutputFile))
			{
				return 0;
			}

			isPacking = true;
			packThis = true;
		}
		else if (isPacking)
		{
			isPacking = false;
			packThis = true;
		}

		if (packThis)
		{
			++number;
		}
	}

	return number;
}

void Internal_Pack(const FPath& ParentDir, const FString& InExtension, const int& MaxSteps, const FString& OutputFile, ofstream& toFile)
{
	int numberOfSteps = 0;
	bool isPacking = false;
	for (const FPath& foundFile : directory_iterator(ParentDir))
	{
		if (is_directory(foundFile)) continue; // only files

		if(!foundFile.string().compare(OutputFile)) continue;

		if (!ToUpper(foundFile.extension().string()).compare(ToUpper(InExtension)))
		{
			bool packThis = false;
			if (file_size(foundFile) >= ONE_GB)
			{
				isPacking = true;
				packThis = true;
			}
			else if (isPacking)
			{
				isPacking = false;
				packThis = true;
			}

			if (packThis)
			{
				ECHO("Baleni filmu: ");
				const int percentage = (int)(((float)numberOfSteps / (float)MaxSteps) * 100);
				ECHO(percentage);
				ECHOL("%");

				ECHO_V("Pridavani souboru do balicku: ");
				ECHOL_V(foundFile);

				Internal_CopyToFile(foundFile, toFile);

				++numberOfSteps;
			}
		}

		if (RemoveFiles)
		{
			ECHO_V("Odstranovani souboru: ");
			ECHOL_V(foundFile.string());

			if (!remove(foundFile.string()))
			{
				ECHOL_V("Odstranovani souboru selhalo");
			}
		}
	}

	ECHOL("Baleni filmu: 100%");
}

#pragma endregion

void ParseArguments(int argc, char** argv)
{
	Verbose = false;
	RemoveFiles = true;

	for (int i = 0; i < argc; ++i)
	{
		FString arg = argv[i];

		if (!arg.compare("-Verbose") ||
			!arg.compare("-verbose") ||
			!arg.compare("-V") ||
			!arg.compare("-v"))
		{
			Verbose = true;
		}
		else if (!arg.compare("-noremove") ||
			!arg.compare("-noRemove") ||
			!arg.compare("-NoRemove") ||
			!arg.compare("-Noremove") ||
			!arg.compare("-nr") ||
			!arg.compare("-nR") ||
			!arg.compare("-Nr") ||
			!arg.compare("-NR"))
		{
			RemoveFiles = false;
		}
	}
}

void PackFile(const FPath& ParentDir, const FString& InExtension, const FString& OutputFile)
{
	ECHOL("");
	ECHO("Baleni filmu: ");
	ECHOL(ParentDir.stem());

	auto startTime = high_resolution_clock::now();

	create_directories(ParentDir);

	// We name output file like directory name
	ECHO_V("Vytvareni souboru: ");
	ECHOL_V(OutputFile);

	const int maxSteps = Internal_GetStepNumber(ParentDir, InExtension, OutputFile);
	if (maxSteps > 1)
	{
		ofstream resultFile(OutputFile, ios::binary);
		Internal_Pack(ParentDir, InExtension, maxSteps, OutputFile, resultFile);
		resultFile.close();
	}
	else
	{
		ECHOL("Nalezen jiz vytvoreny balicek nebo film nenalezen vubec");
		ECHOL("Preskakovani filmu...");
		return;
	}

	auto diffTime = (high_resolution_clock::now() - startTime);

	ECHOL("");
	auto durationInMin = duration_cast<minutes>(diffTime);
	ECHO("Celkovy cas baleni filmu v minutach: ");
	ECHOL(durationInMin.count());
}

// Finds directories which includes specific files with extension
TArray<FPath> FindDirectoriesIncludingFilesExt(const FPath& StartDirectory, const FString& InExtension)
{
	TArray<FPath> resultArray;
	Internal_RecursiveFindFirst(StartDirectory, InExtension, resultArray);

	return resultArray;
}

int main(int argc, char** argv)
{
	ParseArguments(argc, argv);

	auto startTime = high_resolution_clock::now();

	ECHOL("");

	if (!Verbose)
	{
		ECHOL("Start programu...");
	}
	else
	{
		ECHOL("Start programu ve verbose modu...");
	}

	if (RemoveFiles)
	{
		ECHOL("Odstranovani prebytecnich souboru: Zapnuto");
	}
	else
	{
		ECHOL("Odstranovani prebytecnich souboru: Vypnuto");
	}

	ECHOL("");

	TArray<FPath> foundDirs = FindDirectoriesIncludingFilesExt(".\\", VOB_EXTENSION);

	if (!foundDirs.empty()) // is not empty
	{
		if (foundDirs.size() > 1)
		{
			ECHO("Nalezeny ");
			ECHO(foundDirs.size());
			ECHOL(" filmy.")
		}
		else
		{
			ECHOL("Nalezen 1 film");
		}

		for (FPath newDir : foundDirs)
		{
			PackFile(newDir, VOB_EXTENSION, Internal_CreateFile(newDir, VOB_EXTENSION));
		}
	}
	else
	{
		ECHOL("Zadny film nebyl nalezen");
	}

	ECHOL("");
	ECHOL("Konec behu programu..");
	ECHOL("");
	auto diffTime = (high_resolution_clock::now() - startTime);

	auto durationInMin = duration_cast<minutes>(diffTime);
	ECHO("Celkovy cas behu programu v minutach: ");
	ECHOL(durationInMin.count());

	auto durationInHour = duration_cast<hours>(diffTime);
	ECHO("Celkovy cas behu programu v hodinach: ");
	ECHOL(durationInHour.count());

	ECHOL("");
	PAUSE();

	return EXIT_SUCCESS;
}