//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioFileOggOpus.h"
#include "opusfile.h"
#include "Log.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

AudioFileOggOpus::AudioFileOggOpus(void):
	oggOpusFile(NULL)
{

}

AudioFileOggOpus::~AudioFileOggOpus(void)
{
	Close();
}

int AudioFileOggOpus::Open(AnsiString fileName)
{
    eof = false;
	if (oggOpusFile) {
		op_free(oggOpusFile);
		oggOpusFile = NULL;
	}
	int opRet = 0;
	oggOpusFile = op_open_file(fileName.c_str(), &opRet);
	if (oggOpusFile == NULL) {
		LOG("Failed to open %s as Opus/OGG, ret code = %d", fileName.c_str(), opRet);
		return -1;
	}

	totalPcmSamples = op_pcm_total(oggOpusFile, -1);
	pcmPosition = 0;

	return 0;
}

int AudioFileOggOpus::Close(void)
{
	if (oggOpusFile) {
		op_free(oggOpusFile);
		oggOpusFile = NULL;
		return 0;
	}
	return -1;
}

int AudioFileOggOpus::GetChannels(void)
{
	return 2;
}

int AudioFileOggOpus::GetSampleRate(void)
{
	return 48000;
}

int AudioFileOggOpus::GetSamples(int16_t *buf, int *count)
{
	unsigned int left = *count;
	unsigned int pos = 0;

	while (left > 0 && !eof) {
		int ret = op_read_stereo(oggOpusFile, buf + pos, left);
		/* returns: The number of samples read per channel on success, or a negative value on failure. */
		if (ret <= 0) {
			LOG("End of Opus/OGG file, ret = %d", ret);
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

int AudioFileOggOpus::SeekToPcmPosition(int64_t pos)
{
	if (oggOpusFile == NULL)
		return -1;
	return op_pcm_seek(oggOpusFile, pos);	
}
