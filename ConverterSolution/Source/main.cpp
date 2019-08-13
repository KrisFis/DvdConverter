
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
#define MP4_OUPUT_NAME "finalVideo.mp4"

template<typename ArrayType>
struct FTempArray
{
	FTempArray() : Pointer(nullptr), Num(0) { }
	FTempArray(const int64& NewSize) : Num(NewSize) { Pointer = new ArrayType[NewSize]; }
	//~FTempArray() { if (IsValid(Pointer)) { delete[] Pointer; } }

	void Reset() { if (IsValid(Pointer)) { delete[] Pointer; } Num = 0; }
	bool IsEmpty() const { return (Num <= 0); }

	bool IsValidIndex(const int64& i) const { return (i >= 0) && (i < Num); }
	ArrayType& operator[](const int64& i) { return Pointer[i]; }
	const ArrayType& operator[](const int64& i) const { return Pointer[i]; }

	void Add(const ArrayType& newElement)
	{
		ArrayType* resultPtr = new ArrayType[Num + 1];

		for (int32 i = 0; i < Num + 1; ++i)
		{
			resultPtr[i] = Pointer[i];
		}

		resultPtr[++Num] = newElement;

		if (IsValid(Pointer))
		{
			delete[] Pointer;
		};

		Pointer = resultPtr;
	}

	void Append(const FTempArray& OtherArray)
	{
		if (OtherArray.Num <= 0) return;

		ENSURE_VALID(OtherArray.Pointer);

		ArrayType* resultPtr = new ArrayType[Num + OtherArray.Num];

		for (int32 i = 0; i < Num; ++i)
		{
			resultPtr[i] = Pointer[i];
		}

		if (IsValid(Pointer))
		{
			delete[] Pointer;
		}

		for (int32 i = Num; i < (Num + OtherArray.Num); ++i)
		{
			resultPtr[i] = OtherArray[i];
		}

		Pointer = resultPtr;
		Num += OtherArray.Num;
	}

	ArrayType* Pointer;
	int64 Num;
};

void CopyToFile(const FString& FromFile, ofstream& ToFile)
{
	ifstream file((char*)FromFile, ios::binary);
	if (file.is_open())
	{
		ToFile << file.rdbuf();

		file.close();
	}

	// 	ifstream input("C:\\Final.gif", ios::binary);
	// 	ofstream output("C:\\myfile.gif", ios::binary);
	// 
	// 	copy(
	// 		istreambuf_iterator<char>(input),
	// 		istreambuf_iterator<char>(),
	// 		ostreambuf_iterator<char>(output));
}

// FTempArray<char> CopyToMemory(const FString& Filename)
// {
// 	FTempArray<char> memArray;
// 
// 	ifstream file((char*)Filename, ios::binary);
// 	if (file.is_open())
// 	{
// 		file.seekg(0, file.end);
// 		int64 size = file.tellg();
// 		memArray = FTempArray<char>(size);
// 		file.seekg(0);
// 		file.read(memArray.Pointer, size);
// 		file.close();
// 	}
// 
// 	return memArray;
// }

void ConvertFileToMP4(const FString& Filename)
{
	// Create a muxer that will output the video as MP4.
	Muxer* muxer = new Muxer(MP4_OUPUT_NAME);

	// Create a MPEG2 codec that will encode the raw data.
	VideoCodec* codec = new VideoCodec(AV_CODEC_ID_H264);

	// Create an encoder that will encode the raw audio data using the codec specified above.
	// Tie it to the muxer so it will be written to file.
	VideoEncoder* encoder = new VideoEncoder(codec, muxer);

	// Create a video filter and do some funny stuff with the video data.
	// "scale=640:150,transpose=cclock,vignette"
	// Filter* filter = new Filter("scale=1:1", encoder);

	// Load a container. Pick the best video stream container in the container
	// And send it to the filter.
	Demuxer* demuxer = new Demuxer(Filename);
	demuxer->DecodeBestVideoStream(encoder);

	// Prepare the output pipeline.
	// This will decode a small amount of frames so the pipeline can configure itself.
	demuxer->PreparePipeline();

	// Push all the remaining frames through.
	while (!demuxer->IsDone())
	{
		demuxer->Step();
	}

	// Save everything to disk by closing the muxer.
	muxer->Close();

	delete muxer;
	delete codec;
	delete encoder;
	//delete filter;
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

	resultPath = DirectoryPath;
	resultPath += "/";
	resultPath += VOB_OUTPUT_NAME;
	return resultPath;

// 	FString command = "forfiles / P";
// 	command += FilePath;
// 	command += "/ M * .vob / C \"CMD /C if @fsize gtr 1048576000 echo @PATH";
// 	system(command);
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
				FString finalFileName = CreateOutputVideo(directoryName);
				if (finalFileName.IsEmpty())
				{

					return;
				}

				ConvertFileToMP4(finalFileName);

#ifdef _RELEASE
				fs::remove({ (char*)finalFileName });
#endif
			}
		}
	}
}

int main(void)
{
	LogMsg("Press enter to proceed to Test...");

	LogWait();

	RecursiveFindAndExecute("./");

	LogMsg("Converting has been done..");

	LogWait();

	return 0;
}

#undef DIRECTORY_NAME
#undef VOB_EXTENSION
#undef VOB_OUTPUT_NAME
#undef MP4_OUPUT_NAME