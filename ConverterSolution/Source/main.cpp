
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

template<typename ArrayType>
struct FTempArray
{
	FTempArray() : Pointer(nullptr), Num(0) { }
	FTempArray(const uint32& NewSize) : Num(NewSize) { Pointer = new ArrayType[NewSize]; }
	//~FTempArray() { if (IsValid(Pointer)) { delete[] Pointer; } }

	void Reset() { if (IsValid(Pointer)) { delete[] Pointer; } Num = 0; }
	bool IsEmpty() const { return (Num <= 0); }

	bool IsValidIndex(const uint32& i) const { return (i >= 0) && (i < Num); }
	ArrayType& operator[](const uint32& i) { return Pointer[i]; }
	const ArrayType& operator[](const uint32& i) const { return Pointer[i]; }

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
		}

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
	uint32 Num;
};

FTempArray<char> CopyToMemory(const FString& Filename)
{
	streampos size;
	FTempArray<char> memArray;

	ifstream file((char*)Filename, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memArray.Pointer = new char[size];
		file.seekg(0, ios::beg);
		file.read(memArray.Pointer, size);
		file.close();
	}

	return memArray;
}

FString CreateOutputVideo(const FString& DirectoryPath)
{
	FString resultPath = FString::Empty;

	fs::path path{ (char*)(DirectoryPath) };

	if(!fs::is_directory(path))
	{
		return resultPath;
	}

	path /= "outputVideo.vob";
	fs::create_directories(path.parent_path());

	ofstream resultFile(path);

	for (const auto& entry : fs::directory_iterator((char*)DirectoryPath))
	{
		if (entry.path().extension() == ".VOB" && fs::file_size(entry.path()) >= OneGB)
		{
			LogMsg((char*)(entry.path().string().data()));

			FTempArray<char> memArray = CopyToMemory((char*)(entry.path().string().data()));
			if (!memArray.IsEmpty())
			{
				resultFile << (char*)(memArray.Pointer);
			}

			memArray.Reset();
		}
	}

	resultFile.close();

	resultPath = (char*)(path.string().data());
	return resultPath;

// 	FString command = "forfiles / P";
// 	command += FilePath;
// 	command += "/ M * .vob / C \"CMD /C if @fsize gtr 1048576000 echo @PATH";
// 	system(command);
}

void ConvertFile(const FString& Filename)
{
	// Create a muxer that will output the video as MP4.
	Muxer* muxer = new Muxer(Filename);

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
	Demuxer* demuxer = new Demuxer("finalFilm.mp4");
	demuxer->DecodeBestVideoStream(encoder);

	// Prepare the output pipeline.
	// This will decode a small amount of frames so the pipeline can configure itself.
	demuxer->PreparePipeline();

	// Push all the remaining frames through.
	while (!demuxer->IsDone())
	{
		LogMsg("Doing step..");
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

int main(void)
{
	LogMsg("Press enter to proceed to Test...");

	LogWait();

	FString filename = CreateOutputVideo("TestData/VIDEO_TS");
	if (filename.IsEmpty())
	{
		LogMsg("Nothing was found");

		LogWait();

		return 1;
	}

	ConvertFile(filename);

	LogMsg("Converting has been done..");

	LogWait();

	return 0;
}