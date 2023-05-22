//---------------------------------------------------------------------------

#ifndef AudioFileTranscriberH
#define AudioFileTranscriberH
//---------------------------------------------------------------------------

#include <System.hpp>

class AudioFile;

class AudioFileTranscriber
{
private:
	bool terminated;
	HANDLE hProcess;
public:
	AudioFileTranscriber(void):
		terminated(false),
		hProcess(NULL)
	{
	}

	~AudioFileTranscriber(void)
	{
	}

	/** \brief Convert mono file or one of the stereo file channels to new mono L16, 16ksps wave file
	*/
	int Transcribe(AnsiString sourceFile, AnsiString whisperExe, AnsiString model, AnsiString language, unsigned int threadCount);

	void Terminate(void);
};

#endif

