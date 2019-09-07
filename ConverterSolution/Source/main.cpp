
// AlternitySTD
#include <EssentialsMethods.h>
#include <AltString.h>
using namespace sal;

// STD
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <filesystem>
#include <time.h>
using namespace std;
namespace fs = std::filesystem;

// ConverterProject
#include "ConverterUtilities.h"
#include "ConverterMacros.h"
#include "ConverterPass.h"
using namespace FConverterUtilities;
using namespace FConvertPass;

uint16 NumberOfPackedVideos = 0;
uint16 NumberOfConvertedVideos = 0;

EOperationMode ExecMode = EOperationMode::OnlyPackage;

void CopyToFile(const FString& FromFile, ofstream& ToFile)
{
	DEBUG_COMMAND(LogMsg("Copying file: "));
	DEBUG_COMMAND(LogMsg(FromFile));

	ifstream file((char*)FromFile, ios::binary);
	if (file.is_open())
	{
		ToFile << file.rdbuf();

		file.close();
	}
}

FString CreateOutputVideo(const FString& DirectoryPath)
{
	FString resultPath = FString::Empty;

	fs::path path{ (char*)(DirectoryPath) };

	if(!fs::is_directory(path))
	{
		return resultPath;
	}

	int16 partsForPackage = 0;
	bool forceUse = false;

	for (const auto& entry : fs::directory_iterator((char*)DirectoryPath))
	{
		FString newPath = (char*)ExtendFilename(DirectoryPath, entry.path());

		if (entry.path().extension() == VOB_EXTENSION)
		{
			if (GetLastPart(newPath) == PACKAGE_NAME &&
				fs::file_size(StringToPath(newPath)) >= ONE_GB)
			{
				cout << "Byl nalezen predchozi balicek!" << endl;
				cout << "Preskakuji vytvareni balicku.." << endl;
				partsForPackage = INDEX_NONE;
				break;
			}
			else if(forceUse || fs::file_size(entry.path()) >= ONE_GB)
			{
				++partsForPackage;
				forceUse = true;
			}
		}
		else
		{
			forceUse = false;
		}
	}

	if (partsForPackage == INDEX_NONE)
	{
		++NumberOfPackedVideos;

		resultPath = DirectoryPath;
		resultPath += "/";
		resultPath += PACKAGE_NAME;
		return resultPath;
	}
	else if (partsForPackage == 0)
	{
		ENSURE_NO_ENTRY(FString::Empty);
	}

	path /= PACKAGE_NAME;
	fs::create_directories(path.parent_path());

	ofstream resultFile(path, ios::binary);

	int16 tmpPackagePartsNumber = 0;

	for (const auto& entry : fs::directory_iterator((char*)DirectoryPath))
	{
		FString newPath = (char*)ExtendFilename(DirectoryPath, entry.path());

		if (entry.path().extension() == VOB_EXTENSION &&
			GetLastPart(newPath) != PACKAGE_NAME)
		{
			if (forceUse || fs::file_size(StringToPath(newPath)) >= ONE_GB)
			{
				cout << "Vytvareni balicku: " << ((float)tmpPackagePartsNumber / (float)partsForPackage) * 100 << " %" << endl;

				CopyToFile(newPath, resultFile);
				++tmpPackagePartsNumber;
				forceUse = true;
			}
		}
		else
		{
			forceUse = false;
		}
	}

	resultFile.close();
	++NumberOfPackedVideos;

	resultPath = DirectoryPath;
	resultPath += "/";
	resultPath += PACKAGE_NAME;
	return resultPath;
}

void RecursiveFindAndExecute(const FString& PWD)
{
	for (const auto& entry : fs::directory_iterator((char*)PWD))
	{
		FString newPath = (char*)ExtendFilename(PWD, entry.path());

		DEBUG_COMMAND(LogMsg(newPath));

		if (fs::is_directory(StringToPath(newPath)))
		{
			FString directoryName = PWD;
			if(directoryName != "./") { directoryName += "/"; }
			directoryName += GetLastPart(newPath);

			if (GetLastPart(newPath) != PARENT_DIRECTORY_NAME)
			{
				RecursiveFindAndExecute(directoryName);
			}
			else
			{
				cout << "Nalezeno nove DVD pro konverzi se jmenem: " << (char*)(GetLastPart(PWD)) << endl;
				cout << endl;
				cout << "Vytvareni jednotneho DVD balicku" << endl;

				FString inputFilePath = CreateOutputVideo(directoryName);
				if (inputFilePath.IsEmpty())
				{
					return;
				}

				if (ExecMode != EOperationMode::Convert) return;

				cout << "Vytvareni MKV z balicku DVD" << endl;

				FString outputFilePath = PWD;
				outputFilePath += "/";
				outputFilePath += GetLastPart(PWD);
				outputFilePath += MKV_EXTENSION;

				cout << endl;
				ConvertToMKV(inputFilePath, outputFilePath);
				++NumberOfConvertedVideos;
				cout << endl;

				cout << "Mazani vytvoreneho balicku DVD" << endl;
				cout << endl;
#ifdef _RELEASE
				fs::remove({ (char*)inputFilePath });
#endif
			}
		}
	}
}

int main(void)
{
	time_t programTimer = time(NULL);

	cout << "Start programu.." << endl;
	cout << endl;
	cout << "Hledani slozek s DVD zaznamy.." << endl;
	cout << endl;


	DEBUG_COMMAND(LogWait());

	RecursiveFindAndExecute("./");

	cout << "Veskere DVD formaty byly nalezeny" << endl;
	cout << "Konec hledani.." << endl;
	cout << "---------------------" << endl;

	cout << endl;
	cout << "Pocet sbalenych DVD zaznamu: " << NumberOfConvertedVideos << " !" << endl;
	cout << "Pocet vytvorenych MKV: " << NumberOfConvertedVideos << " !" << endl;
	cout << endl;
	cout << "Celkovy cas behu programu: " << difftime(programTimer, time(NULL)) << endl;
	cout << endl;
	cout << endl;

	cout << "V pripade chyb nebo problemu urcite napis nebo zavolej" << endl;
	cout << "Jan Kristian Fisera" << endl;
	cout << "Email: krisfis@email.cz" << endl;
	cout << endl;

	LogWait();

	return 0;
}