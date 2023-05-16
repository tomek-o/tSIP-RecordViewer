//---------------------------------------------------------------------------

#ifndef AudioFileWavePcmH
#define AudioFileWavePcmH
//---------------------------------------------------------------------------

#include "AudioFile.h"
#include <stdio.h>

class AudioFileWavePcm: public AudioFile
{
public:
	AudioFileWavePcm(void);
	~AudioFileWavePcm(void);
	int Open(AnsiString fileName);
	int Close(void);
	/** \todo Check actual number of channels if splitting to L+R */
	int GetChannels(void);
	int GetSampleRate(void);
	int GetSamples(int16_t *buf, int *count);
	int SeekToPcmPosition(int64_t pos);
private:
	FILE *file;
};

#endif
