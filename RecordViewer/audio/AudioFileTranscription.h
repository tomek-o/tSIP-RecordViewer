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
	bool stopRequest;
	AnsiString fileName;
	AudioFile *file;
	AnsiString whisperExe;
	AnsiString model;
	AnsiString language;
	unsigned int threadCount;
	HANDLE hProcess;
	int Process(void);
public:
	AudioFileTranscription(void):
		running(false),
		stopRequest(false),
		file(NULL),
		threadCount(2),
		hProcess(NULL)
	{
	}
	int Transcribe(AnsiString fileName, AnsiString whisperExe, AnsiString model, AnsiString language, unsigned int threadCount);
	void Stop(void);
	bool IsRunning(void) const
	{
		return running;
	}
	AnsiString GetFileName(void) const
	{
    	return fileName;
	}
};

#endif
