
#include <EssentialsMethods.h>
#include <Muxing/Muxer.h>
#include <Codecs/VideoCodec.h>
#include <Frame Sinks/VideoEncoder.h>
#include <Frame Sinks/Filter.h>
#include <Sources/Demuxer.h>

void Test()
{
	// Create a muxer that will output the video as MP4.
	ffmpegcpp::Muxer* muxer = new ffmpegcpp::Muxer("filtered_video.mp4");

	// Create a MPEG2 codec that will encode the raw data.
	ffmpegcpp::VideoCodec* codec = new ffmpegcpp::VideoCodec(AV_CODEC_ID_MPEG2VIDEO);

	// Create an encoder that will encode the raw audio data using the codec specified above.
	// Tie it to the muxer so it will be written to file.
	ffmpegcpp::VideoEncoder* encoder = new ffmpegcpp::VideoEncoder(codec, muxer);

	// Create a video filter and do some funny stuff with the video data.
	ffmpegcpp::Filter* filter = new ffmpegcpp::Filter("scale=640:150,transpose=cclock,vignette", encoder);

	// Load a container. Pick the best video stream container in the container
	// And send it to the filter.
	ffmpegcpp::Demuxer* demuxer = new ffmpegcpp::Demuxer("big_buck_bunny.mp4");
	demuxer->DecodeBestVideoStream(filter);

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
}

int main(void)
{
	sal::LogMsg("Press enter to proceed to Test...");

	sal::LogWait();

	return 0;
}