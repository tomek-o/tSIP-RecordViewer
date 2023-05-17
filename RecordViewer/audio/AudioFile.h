//---------------------------------------------------------------------------

#ifndef AudioFileH
#define AudioFileH
//---------------------------------------------------------------------------

#include <System.hpp>
#include <stdint.h>

class AudioFile
{
protected:
	bool eof;
	int64_t totalPcmSamples;
	int64_t pcmPosition;
public:
	AudioFile(void):
		eof(false),
		totalPcmSamples(0),
		pcmPosition(0)
	{}
	virtual ~AudioFile(void) {};
	virtual int Open(AnsiString fileName) = 0;
	virtual int Close(void) = 0;
	virtual int GetChannels(void) = 0;
	virtual int GetSampleRate(void) = 0;
	virtual int GetSamples(int16_t *buf, int *count) = 0;
	bool IsEof(void) const
	{
    	return eof;
	}
	int64_t GetTotalPcmSamples(void) const
	{
		return totalPcmSamples;
	}
	int64_t GetPcmPosition(void) const
	{
		return pcmPosition;
	}
	virtual int SeekToPcmPosition(int64_t pos) = 0;
};

#endif