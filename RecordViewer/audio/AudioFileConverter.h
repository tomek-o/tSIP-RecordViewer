//---------------------------------------------------------------------------

#ifndef AudioFileConverterH
#define AudioFileConverterH
//---------------------------------------------------------------------------

#include "AudioFileChannel.h"
#include <System.hpp>

class AudioFile;

class AudioFileConverter
{
private:

public:
	AudioFileConverter(void)
	{}

	~AudioFileConverter(void);

	/** \brief Convert mono file or one of the stereo file channels to new mono L16, 16ksps wave file
	*/
	int Convert(AudioFile *file, AnsiString outputFileName, enum AudioFileChannel channel, bool &stopRequest);
};

#endif
