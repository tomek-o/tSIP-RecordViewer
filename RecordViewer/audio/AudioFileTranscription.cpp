//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioFileTranscription.h"
#include "AudioFile.h"
#include "AudioFileFactory.h"
#include "AudioFileConverter.h"
#include "AudioFileTranscriber.h"
#include "AudioTranscriptionFileName.h"
#include "Log.h"
#include <assert.h>
#include <SysUtils.hpp>
#include <Forms.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{

enum { WHISPER_REQUIRED_SAMPLING = 16000 };

int CreateTranscription(AudioFile *file, AnsiString fileName, AudioFileChannel channel,
	AnsiString whisperExe, AnsiString model, AnsiString language, unsigned int threadCount,
	HANDLE &hProcess, bool &stopRequest)
{
	AnsiString whisperSourceFileName = ExtractFileDir(Application->ExeName) + "\\tmp_" + GetAudioFileChannelName(channel) + ".wav";
	AudioFileConverter converter;
	int status = converter.Convert(file, whisperSourceFileName, channel, stopRequest);
	if (stopRequest)
	{
    	status = -1;
	}
	if (status == 0)
	{
		AudioFileTranscriber transcriber(hProcess);
		status = transcriber.Transcribe(whisperSourceFileName, whisperExe, model, language, threadCount);
		if (status == 0)
		{
			AnsiString src = whisperSourceFileName + ".json";
			AnsiString dest = GetTranscriptionFileName(fileName, channel);
			if (MoveFile(src.c_str(), dest.c_str()))
			{

			}
			else
			{
				LOG("Failed to move json file to %s", dest.c_str());
				status = -1;
			}
		}
	}
	return status;
}

int CreateTranscriptionWithoutConversion(AudioFile *file, AnsiString fileName,
	AnsiString whisperExe, AnsiString model, AnsiString language, unsigned int threadCount,
	HANDLE &hProcess, bool &stopRequest)
{
	AudioFileTranscriber transcriber(hProcess);
	int status = transcriber.Transcribe(fileName, whisperExe, model, language, threadCount);
	if (status == 0)
	{
		AnsiString src = fileName + ".json";
		AnsiString dest = GetTranscriptionFileName(fileName, AUDIO_CHANNEL_MONO);
		if (MoveFile(src.c_str(), dest.c_str()))
		{

		}
		else
		{
			LOG("Failed to move json file to %s", dest.c_str());
			return -1;
		}
	}
	return status;
}

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
			if (stopRequest == false)
				status = CreateTranscription(file, fileName, AUDIO_CHANNEL_MONO, whisperExe, model, language, threadCount, hProcess, stopRequest);
		}
		else
		{
			if (stopRequest == false)
				status |= CreateTranscription(file, fileName, AUDIO_CHANNEL_L, whisperExe, model, language, threadCount, hProcess, stopRequest);
			if (stopRequest == false)
				status |= CreateTranscription(file, fileName, AUDIO_CHANNEL_R, whisperExe, model, language, threadCount, hProcess, stopRequest);
		}
	}
	else
	{
		if (stopRequest == false)
		{
			LOG("No resampling / channel separation is needed for the file");
			status = CreateTranscriptionWithoutConversion(file, fileName, whisperExe, model, language, threadCount, hProcess, stopRequest);
		}
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

	stopRequest = false;

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
	stopRequest = true;
	if (hProcess != NULL)
	{
		TerminateProcess(hProcess, -1);
	}	
}
