//---------------------------------------------------------------------------

#ifndef AudioFilePlayerH
#define AudioFilePlayerH
//---------------------------------------------------------------------------

#include <System.hpp>
#include <stdint.h>

class AudioFile;

class AudioFilePlayer
{
private:
	static DWORD WINAPI WaveOutThreadProc(LPVOID data);
	friend void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	HANDLE		WaveOutThread;
	DWORD dwtid;
	CRITICAL_SECTION waveCriticalSection;
	WAVEHDR*         waveBlocks;
	volatile int     waveFreeBlockCount;
	int              waveCurrentBlock;
	int iWaveOutDev;
	HWAVEOUT	hWaveOut;
	WAVEFORMATEX  waveFormat;
	WAVEHDR* allocateBlocks(int size, int count);
	int writeAudio(HWAVEOUT hWaveOut, int16_t* data, int size);
	unsigned int playBlockSize;

	AudioFile *file;

	volatile bool stopRequest;
	volatile bool pauseRequest;
	volatile bool playing;
	volatile int64_t seekPcmPositionRequest;

public:
	AudioFilePlayer(void);
	~AudioFilePlayer(void);
	int Play(AnsiString fileName, AnsiString audioDevice);
	int Pause(bool state);
	int Stop(void);
	bool IsPlaying(void) const {
		return playing;
	}
	bool IsPaused(void) const {
		return pauseRequest;
	}
	int64_t GetTotalPcmSamples(void) const;
	int64_t GetPcmPosition(void) const;
	unsigned int GetSampleRate(void) const;
	int SeekToPcmPosition(int64_t pos);
};

#endif
