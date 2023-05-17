//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioFileTranscription.h"
#include "AudioFile.h"
#include "AudioFileFactory.h"
#include "Log.h"
#include <assert.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{
	enum { WHISPER_REQUIRED_SAMPLING = 16000 };
}


DWORD WINAPI AudioFileTranscription::TranscriptionThreadProc(LPVOID data)
{
	int status = 0;
	AudioFileTranscription *aft = reinterpret_cast<AudioFileTranscription*>(data);
	assert(aft);
	AudioFile *file = aft->file;
	assert(file);

	if (file->GetRealChannelsCount() == 1)
	{
		int TODO__OPUS_SAMPLES_WOULD_BE_STEREO_DUE_TO_API_USED;

		if (file->GetSampleRate() == WHISPER_REQUIRED_SAMPLING)
		{

		}
		else
		{
			LOG("TODO: resample");
			status = -2;
		}
	}
	else if (file->GetRealChannelsCount() == 2)
	{
		LOG("TODO: stereo source files");
		// use whisper.cpp with --diarize option -> ale to tylko porównuje energiê L/R a próbki i tak miksuje przed rozpoznawaniem
		// threads: whisper_full_parallel: splitting: the transcription quality may be degraded near these boundaries		
		status = -3;
	}
	else
	{
		LOG("Transcription: unhandled number of channels in source file!");
		status = -1;
	}

	LOG("TranscriptionThread done");
	aft->running = false;
	return status;
}




int AudioFileTranscription::Transcribe(AnsiString fileName, AnsiString whisperExe, AnsiString model, AnsiString language, unsigned int threadCount)
{
	if (running)
	{
		LOG("Transcription already running");
		return -1;
	}

	this->whisperExe = whisperExe;
	this->model = model;
	this->language = language;
	this->threadCount = threadCount;

	file = CreateAudioFile(fileName);
	if (file == NULL)
	{
		LOG("Transcription: failed to open source audio file");
		return -2;
	}

	running = true;
	DWORD dwtid;	
	CreateThread(NULL, 0, TranscriptionThreadProc, this, THREAD_PRIORITY_BELOW_NORMAL, &dwtid);
}

void AudioFileTranscription::Stop(void)
{

}
