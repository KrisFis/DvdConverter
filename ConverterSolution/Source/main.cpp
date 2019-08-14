
// AlternitySTD
#include <EssentialsMethods.h>
#include <AltString.h>

// FFMPEG-cpp
#include <Muxing/Muxer.h>
#include <Codecs/VideoCodec.h>
#include <Frame Sinks/VideoEncoder.h>
#include <Frame Sinks/Filter.h>
#include <Sources/Demuxer.h>
#include <Codecs/AudioCodec.h>
#include <Frame Sinks/AudioEncoder.h>

// STD
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <filesystem>
#include <time.h>

using namespace std;
using namespace ffmpegcpp;
using namespace sal;
namespace fs = std::filesystem;

constexpr int32 OneGB = 1048576000;

typedef long long int64;
typedef unsigned long long uint64;

#define PARENT_DIRECTORY_NAME "VIDEO_TS"
#define VOB_EXTENSION ".VOB"
#define MKV_EXTENSION ".MKV"
#define PACKAGE_NAME "VTS_PACKAGE.VOB"

#ifdef _DEBUG
	#define DEBUG_COMMAND(command) command
#else
	#define DEBUG_COMMAND(command)
#endif

uint16 NumberOfPackedVideos = 0;
uint16 NumberOfConvertedVideos = 0;

FString AddFilename(const FString& ParentDirectory, const fs::path& Path)
{
	FString filename = ParentDirectory;
	filename += "/";
	filename += (char*)(Path.filename().string().data());

	return filename;
}

FString GetLastPart(const fs::path& Path)
{
	if (fs::is_directory(Path))
	{
		return (char*)(Path.filename().string().data());
	}

	string str = Path.string();

	size_t foundIdx = str.find_last_of('/');
	if (foundIdx != string::npos)
	{
		return (char*)(str.substr(++foundIdx).data());
	}

	return (char*)(Path.string().data());
}

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

void ConvertFileToMKV(const FString& Filename, const FString& OutputFile)
{
	cout << endl;

	Muxer* muxer = new Muxer(OutputFile);

	VideoCodec* v_codec = new VideoCodec(AVCodecID::AV_CODEC_ID_H264);
	v_codec->SetOption("crf", 21);

	AudioCodec* a_codec = new AudioCodec(AVCodecID::AV_CODEC_ID_MP3);
	a_codec->SetOption("qscale:a", 2);

// 	Codec* s_codec = new Codec(AV_CODEC_ID_DVD_SUBTITLE);
// 	s_codec->SetOption("copy", 1);

	VideoEncoder* v_encoder = new VideoEncoder(v_codec, muxer);

	AudioEncoder* a_encoder = new AudioEncoder(a_codec, muxer);

	Demuxer* demuxer = new Demuxer(Filename);
	demuxer->DecodeBestVideoStream(v_encoder);
	demuxer->DecodeBestAudioStream(a_encoder);

	demuxer->PreparePipeline();

	while (!demuxer->IsDone())
	{
		demuxer->Step();
	}

	muxer->Close();

	++NumberOfConvertedVideos;

	delete muxer;
	delete v_codec;
	delete v_encoder;
	delete demuxer;

	cout << endl;
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
		if (entry.path().extension() == VOB_EXTENSION)
		{
			fs::path debug_path = {(char*)AddFilename(DirectoryPath, entry.path())};
			FString last_part = GetLastPart(debug_path);
			size_t fileSize = fs::file_size(entry.path());

			if (last_part == PACKAGE_NAME &&
				fileSize >= OneGB)
			{
				cout << "Byl nalezen predchozi balicek!" << endl;
				cout << "Preskakuji vytvareni balicku.." << endl;
				partsForPackage = INDEX_NONE;
				break;
			}
			else
			{
				if (forceUse || fs::file_size(entry.path()) >= OneGB)
				{
					partsForPackage++;
					forceUse = true;
				}
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
		cout << "Vytvarení balicku: " << ((float)tmpPackagePartsNumber / (float)partsForPackage) * 100 << endl;

		if (entry.path().extension() == VOB_EXTENSION)
		{
			if (forceUse || fs::file_size(entry.path()) >= OneGB)
			{
				FString filename = DirectoryPath;
				filename += "/";
				filename += (char*)(entry.path().filename().string().data());

				CopyToFile(filename, resultFile);
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
		DEBUG_COMMAND(LogMsg((char*)(entry.path().string().data())));

		if (fs::is_directory(entry.path()))
		{
			FString directoryName = PWD;
			if(directoryName != "./") { directoryName += "/"; }
			directoryName += GetLastPart(entry.path());

			if (GetLastPart(entry.path()) != PARENT_DIRECTORY_NAME)
			{
				RecursiveFindAndExecute(directoryName);
			}
			else
			{
				cout << "Nalezeno nove DVD pro konverzi se jmenem: " << (char*)(GetLastPart({(char*)PWD})) << endl;
				cout << endl;
				cout << "Vytvareni jednotneho DVD balicku" << endl;

				FString finalFileName = CreateOutputVideo(directoryName);
				if (finalFileName.IsEmpty())
				{
					return;
				}

				cout << "Vytvareni MKV z nove vytvoreneho balicku DVD" << endl;

				FString outputFilename = PWD;
				outputFilename += "/";
				outputFilename += GetLastPart({(char*)PWD});
				outputFilename += MKV_EXTENSION;
				ConvertFileToMKV(finalFileName, outputFilename);

				cout << "Mazani vytvoreneho balicku DVD" << endl;
				cout << endl;
#ifdef _RELEASE
				fs::remove({ (char*)finalFileName });
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

#undef PARENT_DIRECTORY_NAME
#undef VOB_EXTENSION
#undef MKV_EXTENSION
#undef PACKAGE_NAME