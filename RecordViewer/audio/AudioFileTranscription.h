//---------------------------------------------------------------------------

#ifndef AudioFileTranscriptionH
#define AudioFileTranscriptionH
//---------------------------------------------------------------------------

#include <System.hpp>

class AudioFile;

class AudioFileTranscription
{
private:
	static DWORD WINAPI TranscriptionThreadProc(LPVOID data);
	bool running;
	AnsiString fileName;
	AudioFile *file;
	AnsiString whisperExe;
	AnsiString model;
	AnsiString language;
	unsigned int threadCount;
	int Process(void);
public:
	AudioFileTranscription(void):
		running(false),
		file(NULL),
		threadCount(2)
	{
	}
	int Transcribe(AnsiString fileName, AnsiString whisperExe, AnsiString model, AnsiString language, unsigned int threadCount);
	void Stop(void);
};

#endif
