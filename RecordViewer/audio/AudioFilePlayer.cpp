//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "AudioFilePlayer.h"
#include "AudioFileFactory.h"
#include "AudioDevicesList.h"
#include "Log.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{
	enum { PLAY_BLOCK_COUNT = 3 };
}

void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if(uMsg != WOM_DONE)
		return;
	AudioFilePlayer *player = reinterpret_cast<AudioFilePlayer*>(dwInstance);
	EnterCriticalSection(&player->waveCriticalSection);
	player->waveFreeBlockCount++;
	LeaveCriticalSection(&player->waveCriticalSection);
}


DWORD WINAPI AudioFilePlayer::WaveOutThreadProc(LPVOID data)
{
	AudioFilePlayer *player = reinterpret_cast<AudioFilePlayer*>(data);

	int16_t buffer[4000]; /* intermediate buffer for reading */
	player->waveFreeBlockCount = PLAY_BLOCK_COUNT;
	player->waveCurrentBlock   = 0;
	int iError = 0;

	player->waveBlocks = player->allocateBlocks(player->playBlockSize, PLAY_BLOCK_COUNT);
    if(!player->waveBlocks)
    {
		iError = 1;
    }
	else
	{
        WAVEFORMATEX &waveFormat = player->waveFormat;
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nChannels = static_cast<WORD>(player->file->GetChannelsCount());
		waveFormat.nSamplesPerSec = player->file->GetSampleRate();
		waveFormat.wBitsPerSample = 16;
		waveFormat.nBlockAlign = static_cast<WORD>(waveFormat.wBitsPerSample / 8 * waveFormat.nChannels);
		waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;
		waveFormat.cbSize = 0;

		if(waveOutOpen(&player->hWaveOut, player->iWaveOutDev, &player->waveFormat, (DWORD_PTR)waveOutProc,
						(DWORD_PTR)player, CALLBACK_FUNCTION ) != MMSYSERR_NOERROR)
		{
			iError = 2;
		}
	}

	if (!iError)
	{
		bool bEof = false;

		while(!player->stopRequest && !bEof && !iError)
		{
			if (player->seekPcmPositionRequest >= 0)
			{
				player->file->SeekToPcmPosition(player->seekPcmPositionRequest);
				player->seekPcmPositionRequest = -1;
			}

			unsigned int count = sizeof(buffer)/sizeof(buffer[0]);
			int ret = 0;
			if (player->pauseRequest == false)
			{
				ret = player->file->GetSamples(buffer, &count);
			}
			else
			{
				memset(buffer, 0, sizeof(buffer));
			}
			if (ret == 0 && count > 0)
			{
				player->writeAudio(player->hWaveOut, buffer, sizeof(buffer));
			}
			else
			{
				bEof = true;
				break;
			}
		}
	#if 0
		if (player->stopRequested)
		{
			int writeStatus;
			memset(buffer, 0, sizeof(buffer));
			do {
				writeStatus = player->writeAudio(player->hWaveOut, buffer, sizeof(buffer));
			} while(writeStatus != 0);
			Sleep(50);
		}
	#endif
		// wait for all blocks to complete
		while(player->waveFreeBlockCount < PLAY_BLOCK_COUNT /* && !player->bStopRequested && !iError && !pAudioRec->bRewindRequested */)
			Sleep(10);
	}

	player->file->Close();
#if 0
	if(!pAudioRec->bStopRequested) //zatrzymanie nie wynika z wymuszenia z zewn¹trz
	{
		if (pAudioRec->hWnd != (LONG)INVALID_HANDLE_VALUE)
		{
			SendMessage((void*)pAudioRec->hWnd, pAudioRec->MessageStop, iError, 0);  //wywo³anie przy wymuszeniu z zewn¹trz powodowa³oby deadlock z oczekiwaniem na zakoñczenie w¹tku
		}
	}
	pAudioRec->bStopRequested = true;
#endif	
	HeapFree(GetProcessHeap(), 0, player->waveBlocks);

	LOG("WaveOutThread done");
	player->playing = false;
    return 0;
}



AudioFilePlayer::AudioFilePlayer(void):
	iWaveOutDev(WAVE_MAPPER),
	WaveOutThread(INVALID_HANDLE_VALUE),
	waveBlocks(NULL),
	waveFreeBlockCount(0),
	waveCurrentBlock(0),
	playBlockSize(8000),
	stopRequest(false),
	pauseRequest(false),
	playing(false),
	seekPcmPositionRequest(-1)
{
	InitializeCriticalSection(&waveCriticalSection);
}

AudioFilePlayer::~AudioFilePlayer(void)
{
	if (file)
	{
		delete file;
		file = NULL;
	}
	DeleteCriticalSection(&waveCriticalSection);
}

WAVEHDR* AudioFilePlayer::allocateBlocks(int size, int count)
{
	char* buffer;
	int i;
	WAVEHDR* blocks;
	DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;

	// allocate memory for the entire set in one go
	if((buffer = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBufferSize)) == NULL) {
		OutputDebugString("allocateBlocks: Memory allocation error\n");
		return NULL;
	}

	// and set up the pointers to each subblock
	blocks = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR) * count;
	for(i = 0; i < count; i++) {
		blocks[i].dwBufferLength = size;
		blocks[i].lpData = buffer;
		buffer += size;
	}

	return blocks;
}

int AudioFilePlayer::writeAudio(HWAVEOUT hWaveOut, short* data, int size)
{
    WAVEHDR* current;
    int remain;

    current = &waveBlocks[waveCurrentBlock];

    while(size > 0) {
        // make sure the header we're going to use is unprepared
        if(current->dwFlags & WHDR_PREPARED)
            waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));

        if(size < (int)(playBlockSize - current->dwUser)) {
            memcpy(current->lpData + current->dwUser, data, size);
            current->dwUser += size;
            return -1;
        }

		remain = playBlockSize - current->dwUser;
		memcpy(current->lpData + current->dwUser, data, remain);
		size -= remain;
		data += remain / sizeof(data[0]);
        current->dwBufferLength = playBlockSize;

		waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));
		//LOG("waveOutWrite");
        waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));

        EnterCriticalSection(&waveCriticalSection);
        waveFreeBlockCount--;
        LeaveCriticalSection(&waveCriticalSection);

        // wait for a block to become free
        while(!waveFreeBlockCount /* && !bRewindRequested*/)
            Sleep(10);
        // point to the next block
        waveCurrentBlock++;
        waveCurrentBlock %= PLAY_BLOCK_COUNT;

        current = &waveBlocks[waveCurrentBlock];
        current->dwUser = 0;
	#if 0
		if (bRewindRequested)
		{
			MMRESULT res = ::waveOutReset(/*pAudioRec->*/hWaveOut);//marks all blocks as DONE
			(void)res;
			// unprepare any blocks that are still prepared
			for(int i = 0; i < waveFreeBlockCount; i++)
			{
				if(waveBlocks[i].dwFlags & WHDR_PREPARED)
				{
					res = ::waveOutUnprepareHeader(/*pAudioRec->*/hWaveOut, &waveBlocks[i], sizeof(WAVEHDR));
					while ( res == WAVERR_STILLPLAYING )
					{
						// The buffer pointed to by the pwh parameter is still in the queue.
						Sleep( 50 );
						res = ::waveOutUnprepareHeader(/*pAudioRec->*/hWaveOut, &waveBlocks[i], sizeof(WAVEHDR));
					}
				}
			}
		}
	#endif
	}

	return 0;
}

int AudioFilePlayer::Play(AnsiString fileName, AnsiString audioDevice)
{
    LOG("AudioFilePlayer: play %s using [%s] audio device", fileName.c_str(), audioDevice.c_str());
	Stop();
	stopRequest = false;
	pauseRequest = false;

	iWaveOutDev = WAVE_MAPPER;
	if (audioDevice != "")
	{
		std::vector<AnsiString> devices = AudioDevicesList::Instance().winwaveDevsOut;
        bool found = false;
		for (unsigned int i=1 /* skipping wave mapper */; i<devices.size(); i++)
		{
			AnsiString devName = devices[i].c_str();
			if (devName == audioDevice)
			{
				iWaveOutDev = i - 1;
				found = true;
				break;
			}
		}
		if (!found)
		{
			LOG("Audio output device [%s] not found, using default / WAVE_MAPPER!", audioDevice.c_str());
		}
	}

	file = CreateAudioFile(fileName);
	if (file == NULL)
		return -1;

	playBlockSize = file->GetSampleRate() / 4;

	playing = true;
	WaveOutThread = CreateThread(NULL, 0, WaveOutThreadProc, this, THREAD_PRIORITY_HIGHEST, &dwtid);
	return 0;
}

int AudioFilePlayer::Pause(bool state)
{
	pauseRequest = state;
	LOG("AudioFilePlayer pause = %d", static_cast<int>(pauseRequest));
	return 0;
}

int AudioFilePlayer::Stop(void)
{
    LOG("AudioFilePlayer: stop");
	stopRequest = true;

	while (playing)
	{
		Application->ProcessMessages();
		Sleep(10);
	}

	if (file)
	{
		delete file;
		file = NULL;
	}
	return 0;
}

int64_t AudioFilePlayer::GetTotalPcmSamples(void) const
{
	if (!file)
		return 0;
	return file->GetTotalPcmSamples();
}

int64_t AudioFilePlayer::GetPcmPosition(void) const
{
	if (!file)
		return 0;
	return file->GetPcmPosition();
}

unsigned int AudioFilePlayer::GetSampleRate(void) const
{
	if (!file)
		return 16000;
	return file->GetSampleRate();
}

int AudioFilePlayer::SeekToPcmPosition(int64_t pos)
{
	if (!file)
		return -1;
	seekPcmPositionRequest = pos;
	return 0;
}



