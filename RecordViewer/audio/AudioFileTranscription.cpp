//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioFileTranscription.h"
#include "AudioFile.h"
#include "AudioFileFactory.h"
#include "AudioFileConverter.h"
#include "AudioFileTranscriber.h"
#include "Log.h"
#include <assert.h>
#include <SysUtils.hpp>
#include <Forms.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{
	enum { WHISPER_REQUIRED_SAMPLING = 16000 };
}


DWORD WINAPI AudioFileTranscription::TranscriptionThreadProc(LPVOID data)
{
	AudioFileTranscription *aft = reinterpret_cast<AudioFileTranscription*>(data);
	assert(aft);
	int status = aft->Process();
	LOG("TranscriptionThread done");
	return status;
}

int AudioFileTranscription::Process(void)
{
	int status = 0;
	assert(file);

	if (file->GetRealChannelsCount() != 1 || file->GetChannelsCount() != 1 || file->GetSampleRate() != WHISPER_REQUIRED_SAMPLING)
	{
		// needs resampling and/or converting to mono wave
		if (file->GetRealChannelsCount() == 1)
		{
			AnsiString whisperSourceFileName = ExtractFileDir(Application->ExeName) + "\\tmp_mono.wav";
			AudioFileConverter converter;
			status = converter.Convert(file, whisperSourceFileName, AudioFileConverter::OUTPUT_CHANNEL_MONO);
			if (status == 0)
			{
				AudioFileTranscriber transcriber;
				status = transcriber.Transcribe(whisperSourceFileName, whisperExe, model, language, threadCount);
				if (status == 0)
				{


				}
			}
		}
		else
		{
			{
				AudioFileConverter converter;
				AnsiString whisperSourceFileNameL = ExtractFileDir(Application->ExeName) + "\\tmp_L.wav";
				status = converter.Convert(file, whisperSourceFileNameL, AudioFileConverter::OUTPUT_CHANNEL_L);
				if (status == 0)
				{
					AudioFileTranscriber transcriber;
					status = transcriber.Transcribe(whisperSourceFileNameL, whisperExe, model, language, threadCount);
					if (status == 0)
					{


					}
				}
			}

			{
				AudioFileConverter converter;
				AnsiString whisperSourceFileNameR = ExtractFileDir(Application->ExeName) + "\\tmp_R.wav";
				status = converter.Convert(file, whisperSourceFileNameR, AudioFileConverter::OUTPUT_CHANNEL_R);
				if (status == 0)
				{
					AudioFileTranscriber transcriber;
					status = transcriber.Transcribe(whisperSourceFileNameR, whisperExe, model, language, threadCount);
					if (status == 0)
					{


					}
				}
			}
		}
	}
	else
	{
    	LOG("No resampling / downmixing needed for file");
	}

    file->Close();
	delete file;

	running = false;
	return status;
}


int AudioFileTranscription::Transcribe(AnsiString fileName, AnsiString whisperExe, AnsiString model, AnsiString language, unsigned int threadCount)
{
    int status = 0;
	if (running)
	{
		LOG("Transcription already running");
		return -1;
	}

	this->fileName = fileName;
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

	if (file->GetRealChannelsCount() != 1 && file->GetRealChannelsCount() != 2)
	{
		LOG("Transcription: unhandled number of channels in source file!");
		status = -1;
	}
	else
	{
		running = true;

		DWORD dwtid;
		HANDLE thread = CreateThread(NULL, 0, TranscriptionThreadProc, this, THREAD_PRIORITY_NORMAL, &dwtid);
		if (thread == NULL)
		{
			running = false;
			status = -1;
			LOG("Transcription: failed to create thread");
		}
	}

	return status;
}

void AudioFileTranscription::Stop(void)
{

}
