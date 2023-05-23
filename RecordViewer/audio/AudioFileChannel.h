//---------------------------------------------------------------------------

#ifndef AudioFileChannelH
#define AudioFileChannelH
//---------------------------------------------------------------------------

enum AudioFileChannel {
	AUDIO_CHANNEL_MONO = 0,
	AUDIO_CHANNEL_L,
	AUDIO_CHANNEL_R
};

static const char* GetAudioFileChannelName(enum AudioFileChannel channel)
{
	switch (channel)
	{
	case AUDIO_CHANNEL_MONO:
		return "MONO";
	case AUDIO_CHANNEL_L:
		return "L";
	case AUDIO_CHANNEL_R:
		return "R";
	default:
		return "???";
	}
}

#endif
