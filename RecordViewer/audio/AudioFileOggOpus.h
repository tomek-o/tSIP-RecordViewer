//---------------------------------------------------------------------------

#ifndef AudioFileOggOpusH
#define AudioFileOggOpusH
//---------------------------------------------------------------------------

#include "AudioFile.h"

struct OggOpusFile;

class AudioFileOggOpus: public AudioFile
{
public:
	AudioFileOggOpus(void);
	~AudioFileOggOpus(void);
	int Open(AnsiString fileName);
	int Close(void);
	/** \todo Check actual number of channels if splitting to L+R */
	int GetChannelsCount(void);
	int GetRealChannelsCount(void);
	int GetSampleRate(void);
	int GetSamples(int16_t *buf, int *count);
	int SeekToPcmPosition(int64_t pos);
private:
	OggOpusFile *oggOpusFile;
	int realChannelsCount; 
};

#endif
