//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioFileWavePcm.h"
#include "Log.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

AudioFileWavePcm::AudioFileWavePcm(void):
	file(NULL)
{

}

AudioFileWavePcm::~AudioFileWavePcm(void)
{
	Close();
}

int AudioFileWavePcm::Open(AnsiString fileName)
{
    eof = false;
	if (file) {
		fclose(file);
		file = NULL;
	}
	file = fopen(fileName.c_str(), "rb");
	if (file == NULL) {
		LOG("Failed to open %s", fileName.c_str());
		return -1;
	}

	

	totalPcmSamples = op_pcm_total(oggOpusFile, -1);
	pcmPosition = 0;

	return 0;
}

int AudioFileWavePcm::Close(void)
{
	if (file) {
		fclose(file);
		file = NULL;
		return 0;
	}
	return -1;
}

int AudioFileWavePcm::GetChannels(void)
{
	return 2;
}

int AudioFileWavePcm::GetSampleRate(void)
{
	return 48000;
}

int AudioFileWavePcm::GetSamples(int16_t *buf, int *count)
{
	unsigned int left = *count;
	unsigned int pos = 0;

	while (left > 0 && !eof) {
		int ret = op_read_stereo(oggOpusFile, buf + pos, left);
		/* returns: The number of samples read per channel on success, or a negative value on failure. */
		if (ret <= 0) {
			LOG("End of wave file, ret = %d", ret);
			eof = true;
		} else {
			int samplesCnt = ret * 2;
			left -= samplesCnt;
			pos += samplesCnt;
		}
	}

	if (left > 0) {
		// Filling up to full frame
		memset(buf + *count - left, 0, left * sizeof(int16_t));
		*count -= left;
	}

	pcmPosition = op_pcm_tell(oggOpusFile);

	return 0;
}

int AudioFileWavePcm::SeekToPcmPosition(int64_t pos)
{
	if (file == NULL)
		return -1;
	return op_pcm_seek(oggOpusFile, pos);	
}

