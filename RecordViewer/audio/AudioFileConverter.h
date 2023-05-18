//---------------------------------------------------------------------------

#ifndef AudioFileConverterH
#define AudioFileConverterH
//---------------------------------------------------------------------------

#include <System.hpp>

class AudioFile;

class AudioFileConverter
{
private:

public:
	enum OutputChannel {
		OUTPUT_CHANNEL_MONO = 0,
		OUTPUT_CHANNEL_L,
		OUTPUT_CHANNEL_R
	};

	AudioFileConverter(void)
	{}

	~AudioFileConverter(void);

	/** \brief Convert mono file or one of the stereo file channels to new mono L16, 16ksps wave file
	*/
	int Convert(AudioFile *file, AnsiString outputFileName, enum OutputChannel channel);
};

#endif
