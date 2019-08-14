
// AlternitySTD
#include <EssentialsMethods.h>
#include <AltString.h>

// FFMPEG-cpp
#include <Muxing/Muxer.h>
#include <Codecs/VideoCodec.h>
#include <Frame Sinks/VideoEncoder.h>
#include <Frame Sinks/Filter.h>
#include <Sources/Demuxer.h>

// STD
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <filesystem>

using namespace std;
using namespace ffmpegcpp;
using namespace sal;
namespace fs = std::filesystem;

constexpr int32 OneGB = 1048576000;

typedef long long int64;
typedef unsigned long long uint64;

#define PARENT_DIRECTORY_NAME "VIDEO_TS"
#define VOB_EXTENSION ".VOB"
#define VOB_OUTPUT_NAME "outputVideo.vob"

uint16 NumberOfPackedVideos = 0;
uint16 NumberOfConvertedVideos = 0;

FString GetLastPart(const fs::path& Path)
{
	if (fs::is_directory(Path))
	{
		return (char*)(Path.filename().string().data());
	}

	string str = Path.string();
	string resultStr = "";

	for (char c : str)
	{
		if (c == '/')
		{
			break;
		}

		resultStr += c;
	}

	return (char*)(resultStr.data());
}

void CopyToFile(const FString& FromFile, ofstream& ToFile)
{
	ifstream file((char*)FromFile, ios::binary);
	if (file.is_open())
	{
		ToFile << file.rdbuf();

		file.close();
	}
}

void ConvertFileToMP4(const FString& Filename, const FString& OutputFile)
{
	Muxer* muxer = new Muxer(OutputFile);

	VideoCodec* codec = new VideoCodec(AV_CODEC_ID_H264);

	VideoEncoder* encoder = new VideoEncoder(codec, muxer);

	Demuxer* demuxer = new Demuxer(Filename);
	demuxer->DecodeBestVideoStream(encoder);

	demuxer->PreparePipeline();

	while (!demuxer->IsDone())
	{
		demuxer->Step();
	}

	muxer->Close();

	++NumberOfConvertedVideos;

	delete muxer;
	delete codec;
	delete encoder;
	delete demuxer;
}

FString CreateOutputVideo(const FString& DirectoryPath)
{
	FString resultPath = FString::Empty;

	fs::path path{ (char*)(DirectoryPath) };

	if(!fs::is_directory(path))
	{
		return resultPath;
	}

	path /= VOB_OUTPUT_NAME;
	fs::create_directories(path.parent_path());

	ofstream resultFile(path, ios::binary);

	bool forceUse = false;

	for (const auto& entry : fs::directory_iterator((char*)DirectoryPath))
	{
		if (entry.path().extension() == VOB_EXTENSION)
		{
			if (fs::file_size(entry.path()) >= OneGB)
			{
				FString filename = DirectoryPath;
				filename += "/";
				filename += (char*)(entry.path().filename().string().data());

				LogMsg(filename);

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
	resultPath += VOB_OUTPUT_NAME;
	return resultPath;
}

void RecursiveFindAndExecute(const FString& PWD)
{
	for (const auto& entry : fs::directory_iterator((char*)PWD))
	{
		if (fs::is_directory(entry.path()))
		{
			FString directoryName = (char*)(entry.path().string().data());

			if (directoryName != PARENT_DIRECTORY_NAME)
			{
				RecursiveFindAndExecute(directoryName);
			}
			else
			{
				cout << "Nalezeno nove DVD pro konverzi se jmenem: " << PWD << endl;
				cout << endl;
				cout << "Vytvareni jednotneho DVD balicku" << endl;

				FString finalFileName = CreateOutputVideo(directoryName);
				if (finalFileName.IsEmpty())
				{
					return;
				}

				cout << "Vytvareni MP4 z nove vytvoreneho balicku DVD" << endl;

				FString outputFilename = PWD;
				outputFilename += "/";
				outputFilename += GetLastPart({(char*)PWD});
				ConvertFileToMP4(finalFileName, outputFilename);

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
	cout << "Start programu.." << endl;
	cout << endl;
	cout << "Hledani slozek s DVD zaznamy.." << endl;
	cout << endl;

	RecursiveFindAndExecute("./");

	cout << "Veskere DVD formaty byly nalezeny" << endl;
	cout << "Konec hledani.." << endl;
	cout << "---------------------" << endl;

	cout << endl;
	cout << "Pocet sbalenych DVD zaznamu: " << NumberOfConvertedVideos << " !" << endl;
	cout << "Pocet vytvorenych MP4: " << NumberOfConvertedVideos << " !" << endl;
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
#undef VOB_OUTPUT_NAME