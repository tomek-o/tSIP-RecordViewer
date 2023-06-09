//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioFileTranscription.h"
#include "AudioFile.h"
#include "AudioFileFactory.h"
#include "AudioFileConverter.h"
#include "AudioFileTranscriber.h"
#include "AudioTranscriptionFileName.h"
#include "Log.h"
#include "common/ScopedLock.h"
#include "common/TimeCounter.h"
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
	TimeCounter tc("CreateTranscription", false);
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
			if (FileExists(dest))
			{
				if (DeleteFile(dest) == false)
				{
                	LOG("Failed to delete existing file (%s) before moving new one", ExtractFileName(dest).c_str());
				}
			}
			if (MoveFile(src.c_str(), dest.c_str()))
			{
            	LOG("Transcription of %s, channel %s created in %.1f s", fileName.c_str(), GetAudioFileChannelName(channel), tc.getTimeMs()/1000); 
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
		if (FileExists(dest))
		{
			if (DeleteFile(dest) == false)
			{
				LOG("Failed to delete existing file (%s) before moving new one", ExtractFileName(dest).c_str());
			}
		}
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
			AnsiString fileNameL = GetTranscriptionFileName(fileName, AUDIO_CHANNEL_L);
			AnsiString fileNameR = GetTranscriptionFileName(fileName, AUDIO_CHANNEL_R);
            // if one of the L/R files exists (possibly transcription stopped in the middle), skip it
			if (!FileExists(fileNameL))
			{
				if (stopRequest == false)
					status |= CreateTranscription(file, fileName, AUDIO_CHANNEL_L, whisperExe, model, language, threadCount, hProcess, stopRequest);
			}
			if (!FileExists(fileNameR))
			{
				if (stopRequest == false)
					status |= CreateTranscription(file, fileName, AUDIO_CHANNEL_R, whisperExe, model, language, threadCount, hProcess, stopRequest);
			}
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

	if (status == 0)
	{
	    ScopedLock<Mutex> lock(mutex);	
    	transcribedIds.push_back(fileId);
	}

	return status;
}


int AudioFileTranscription::Transcribe(unsigned int fileId, AnsiString fileName, AnsiString whisperExe, AnsiString model, AnsiString language, unsigned int threadCount)
{
    int status = 0;
	if (running)
	{
		LOG("Transcription already running");
		return -1;
	}

	stopRequest = false;

	LOG("Transcribing %s", ExtractFileName(fileName).c_str());

	this->fileId = fileId;
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

int AudioFileTranscription::GetTranscribedId(unsigned int &id)
{
    ScopedLock<Mutex> lock(mutex);
	if (transcribedIds.empty())
		return -1;
	id = transcribedIds.front();
	transcribedIds.pop_front();
	return 0;
}

