
#include "ConverterPass.h"

// AlternitySTD
#include <AltString.h>

// FFMPEG-cpp
#include <Muxing/Muxer.h>
#include <Codecs/VideoCodec.h>
#include <Frame Sinks/VideoEncoder.h>
#include <Frame Sinks/Filter.h>
#include <Sources/Demuxer.h>
#include <Codecs/AudioCodec.h>
#include <Frame Sinks/AudioEncoder.h>

using namespace sal;
using namespace ffmpegcpp;

namespace FConvertPass
{
	void ConvertToMKV(const sal::FString& InputPath, const sal::FString& OutputPath)
	{
		// Muxer
		Muxer* muxer = new Muxer(OutputPath);

		// Codecs
		VideoCodec* v_codec = new VideoCodec("libx264");
		AudioCodec* a_codec = new AudioCodec("libmp3lame");

		// Codecs options
		v_codec->SetOption("copy", 1);
		a_codec->SetOption("copy", 1);

		// Encoders
		VideoEncoder* v_encoder = new VideoEncoder(v_codec, muxer);
		AudioEncoder* a_encoder = new AudioEncoder(a_codec, muxer);

		// Demuxer
		Demuxer* demuxer = new Demuxer(InputPath);
		demuxer->DecodeBestVideoStream(v_encoder);
		demuxer->DecodeBestAudioStream(a_encoder);

		demuxer->PreparePipeline();

		while (!demuxer->IsDone())
		{
			demuxer->Step();
		}

		muxer->Close();

		delete muxer;
		delete v_codec;
		delete v_encoder;
		delete demuxer;
	}
}
