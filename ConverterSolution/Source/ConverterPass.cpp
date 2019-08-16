
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
		Muxer* muxer = new Muxer(OutputPath);

		VideoCodec* v_codec = new VideoCodec("libx264");
		v_codec->SetOption("crf", 21);

	//	AudioCodec* a_codec = new AudioCodec(muxer->GetDefaultAudioFormat()->id); // "libmp3lame"
	//	a_codec->SetOption("qscale:a", 2);

	// 	Codec* s_codec = new Codec(AV_CODEC_ID_DVD_SUBTITLE);
	// 	s_codec->SetOption("copy", 1);

		VideoEncoder* v_encoder = new VideoEncoder(v_codec, muxer);

		//AudioEncoder* a_encoder = new AudioEncoder(a_codec, muxer);

		Demuxer* demuxer = new Demuxer(InputPath);
		demuxer->DecodeBestVideoStream(v_encoder);
		//demuxer->DecodeBestAudioStream(a_encoder);

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
