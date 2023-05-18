//---------------------------------------------------------------------------

#ifndef AudioFileWavePcmH
#define AudioFileWavePcmH
//---------------------------------------------------------------------------

#include "AudioFile.h"
#include <stdio.h>
#include <stdint.h>

class AudioFileWavePcm: public AudioFile
{
public:
	AudioFileWavePcm(void);
	~AudioFileWavePcm(void);
	int Open(AnsiString fileName);
	int Close(void);
	/** \todo Check actual number of channels if splitting to L+R */
	int GetChannelsCount(void);
	int GetRealChannelsCount(void);
	int GetSampleRate(void);
	int GetSamples(int16_t *buf, unsigned int *count);
	int SeekToPcmPosition(int64_t pos);

	struct WaveFormat {
		uint16_t format;
		uint16_t channels;
		uint32_t srate;
		uint32_t byterate;
		uint16_t block_align;
		uint16_t bps;
		uint16_t extra;
		WaveFormat(void):
			format(0),
			channels(0),
			srate(0),
			byterate(0),
			block_align(0),
			bps(0),
			extra(0)
		{}
	};
private:
	struct WaveFormat waveFormat;
	uint32_t dataStartPosition;
	FILE *file;
};

#endif
