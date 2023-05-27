//---------------------------------------------------------------------------

#ifndef AudioFileTranscriptionH
#define AudioFileTranscriptionH
//---------------------------------------------------------------------------

#include "common/Mutex.h"
#include <System.hpp>
#include <deque>

class AudioFile;

class AudioFileTranscription
{
private:
	static DWORD WINAPI TranscriptionThreadProc(LPVOID data);
	bool running;
	bool stopRequest;
	unsigned int fileId;
	AnsiString fileName;
	AudioFile *file;
	AnsiString whisperExe;
	AnsiString model;
	AnsiString language;
	unsigned int threadCount;
	HANDLE hProcess;
	int Process(void);
	Mutex mutex;
	std::deque<unsigned int> transcribedIds;
public:
	AudioFileTranscription(void):
		fileId(0xFFFFFFFF),
		running(false),
		stopRequest(false),
		file(NULL),
		threadCount(2),
		hProcess(NULL)
	{
	}
	int Transcribe(unsigned int fileId, AnsiString fileName, AnsiString whisperExe, AnsiString model, AnsiString language, unsigned int threadCount);
	void Stop(void);
	bool IsRunning(void) const
	{
		return running;
	}
	AnsiString GetFileName(void) const
	{
    	return fileName;
	}
	int GetTranscribedId(unsigned int &id);
};

#endif
