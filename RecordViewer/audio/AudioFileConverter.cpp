//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioFileConverter.h"
#include "AudioFile.h"
#include "Log.h"
#include <speex/speex_resampler.h>
#include <stdint.h>
#include <stdio.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define PROMPT "AudioFileConverter "


#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY)/sizeof((ARRAY)[0]))

namespace
{

enum {
	WAVE_FMT_SIZE = 16
};

enum {
	OUTPUT_SAMPLING = 16000
};

const int SPEEX_RESAMP_QUALITY = 2;	// 0...10 

/** WAV-file chunk */
struct wav_chunk {
	uint8_t id[4];
	uint32_t size;
};

int write_u16(FILE *f, uint16_t v)
{
	if (1 != fwrite(&v, sizeof(v), 1, f))
		return -1;
	return 0;
}

int write_u32(FILE *f, uint32_t v)
{
	if (1 != fwrite(&v, sizeof(v), 1, f))
		return -1;
	return 0;
}

int chunk_encode(FILE *f, const char *id, size_t sz)
{
	if (1 != fwrite(id, 4, 1, f))
		return -1;
	return write_u32(f, (uint32_t)sz);
}

int wav_header_encode(FILE *f, uint16_t format, uint16_t channels, uint32_t srate, uint16_t bps, size_t bytes)
{
	int err;

	err = chunk_encode(f, "RIFF", 36 + bytes);
	if (err)
		return err;

	if (1 != fwrite("WAVE", 4, 1, f))
		return -1;

	err = chunk_encode(f, "fmt ", WAVE_FMT_SIZE);
	if (err)
		return err;

	err  = write_u16(f, format);
	err |= write_u16(f, channels);
	err |= write_u32(f, srate);
	err |= write_u32(f, srate * channels * bps / 8);
	err |= write_u16(f, static_cast<uint16_t>(channels * bps / 8));
	err |= write_u16(f, bps);
	if (err)
		return err;

	return chunk_encode(f, "data", bytes);
}

}


AudioFileConverter::~AudioFileConverter(void)
{

}


int AudioFileConverter::Convert(AudioFile *file, AnsiString outputFileName, enum OutputChannel channel)
{
	int sourceChannels = file->GetChannelsCount();
	int realSourceChannels = file->GetRealChannelsCount();

	LOG("Converting %s to %s", file->GetFileName().c_str(), outputFileName.c_str());

	if (sourceChannels != 1 && sourceChannels != 2)
	{
		LOG(PROMPT"Unhandled source channels number = %d", sourceChannels);
		return -1;
	}

	if (realSourceChannels > sourceChannels)
	{
		LOG(PROMPT"Unexpected source channels number = %d", realSourceChannels);
		return -1;
	}

	if (channel == OUTPUT_CHANNEL_MONO && realSourceChannels != 1)
	{
		LOG(PROMPT"Unexpected: mono output requestted for stereo file");
		return -1;
	}

	if ((channel == OUTPUT_CHANNEL_L || channel == OUTPUT_CHANNEL_R) && realSourceChannels != 2)
	{
		LOG(PROMPT"Unexpected: L/R output requestted for mono file");
		return -1;
	}

	FILE *fp = fopen(outputFileName.c_str(), "wb");
	if (fp == NULL)
	{
		LOG(PROMPT"Failed to create output file [%s]", outputFileName.c_str());
		return -1;
	}

	unsigned int outputBytes;
	if (file->GetSampleRate() == OUTPUT_SAMPLING)
	{
		outputBytes = static_cast<unsigned int>(file->GetTotalPcmSamples() * sizeof(int16_t));
	}
	else
	{
		int TODO__VERIFY_OUTPUT_BYTES_NUMBER_FROM_RESAMPLING;
		outputBytes = static_cast<unsigned int>(static_cast<int64_t>(file->GetTotalPcmSamples()) * OUTPUT_SAMPLING / file->GetSampleRate() * sizeof(int16_t));
	}
	LOG(PROMPT"Expected output bytes = %u", outputBytes);

	if (wav_header_encode(fp, WAVE_FORMAT_PCM, 1, OUTPUT_SAMPLING, 8 * sizeof(int16_t), outputBytes) != 0)
	{
		LOG(PROMPT"Failed to write output wave header");
		fclose(fp);
		return -1;
	}

	int status = 0;
	SpeexResamplerState_ *speex_state = NULL;

	if (OUTPUT_SAMPLING != file->GetSampleRate()) {
		int err = 0;
		speex_state = speex_resampler_init(1, file->GetSampleRate(), OUTPUT_SAMPLING, SPEEX_RESAMP_QUALITY, &err);
		if (speex_state == NULL || err != RESAMPLER_ERR_SUCCESS) {
			LOG(PROMPT"Failed to create resampler");
			fclose(fp);
			return -1;
		}

	}

	if (channel == OUTPUT_CHANNEL_MONO && realSourceChannels == 1 && sourceChannels == 2)
	{
		// Opus: 2 channel API, single channel data
		channel = OUTPUT_CHANNEL_L;
	}

	uint64_t left = file->GetTotalPcmSamples();
	while (left > 0)
	{
		int16_t inputBuf[2048];
		int16_t outputBuf[8192];

		unsigned int count = ARRAY_SIZE(inputBuf);
		status = file->GetSamples(inputBuf, &count);
		if (status != 0)
		{
			LOG(PROMPT"Error reading samples from source");
			break;
		}

		if (count == 0)
		{
			LOG(PROMPT"End of source file");
			break;
		}

		unsigned int inputSamplesCount;
		if (sourceChannels == 1)
		{
			inputSamplesCount = count;
			left -= inputSamplesCount;
		}
		else
		{
			// stereo -> mono
			if (count % 2)
			{
				status = -1;
				LOG(PROMPT"Unexpected: uneven number of samples (%u) from source file", count);
				break;
			}
			inputSamplesCount = count / 2;
			left -= inputSamplesCount;
			if (channel == OUTPUT_CHANNEL_L)
			{
				for (unsigned int i=0; i<inputSamplesCount; i++)
				{
					inputBuf[i] = inputBuf[i*2];
				}
			}
			else
			{
				for (unsigned int i=0; i<inputSamplesCount; i++)
				{
					inputBuf[i] = inputBuf[(i*2)+1];
				}
			}
		}
		unsigned int outputSamplesCount = ARRAY_SIZE(outputBuf);

		speex_resampler_process_int(speex_state, 0, inputBuf, &inputSamplesCount, outputBuf, &outputSamplesCount);

		if (fwrite(outputBuf, outputSamplesCount * sizeof(outputBuf[0]), 1, fp) != 1)
		{
			status = -1;
			LOG(PROMPT"Error writing output file");
			break;
		}
	}

	if (speex_state) {
		speex_resampler_destroy(speex_state);
		speex_state = NULL;
	}	

	fclose(fp);
	return status;
}

