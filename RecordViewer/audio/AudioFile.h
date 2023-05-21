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
	AnsiString fileName;
public:
	AudioFile(void):
		eof(false),
		totalPcmSamples(0),
		pcmPosition(0)
	{}
	virtual ~AudioFile(void) {};
	virtual int Open(AnsiString fileName) = 0;
	virtual int Close(void) = 0;
	virtual int GetChannelsCount(void) = 0;
	/** Opus would report two channels due to type of API being used but it might have mono content
	*/
	virtual int GetRealChannelsCount(void) = 0;
	virtual int GetSampleRate(void) = 0;
	virtual int GetSamples(int16_t *buf, unsigned int *count) = 0;
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
	AnsiString GetFileName(void) const
	{
    	return fileName;
	}
};

#endif
