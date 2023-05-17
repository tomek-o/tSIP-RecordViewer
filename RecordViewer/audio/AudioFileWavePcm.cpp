//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioFileWavePcm.h"
#include "Log.h"
#include <stdint.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)


namespace
{

enum {
	WAVE_FMT_SIZE = 16
};

struct wav_chunk
{
	uint8_t id[4];
	uint32_t size;
};

int read_u16(FILE *f, uint16_t *v)
{
	if (1 != fread(v, sizeof(*v), 1, f))
		return -1;
	return 0;
}


int read_u32(FILE *f, uint32_t *v)
{
	if (1 != fread(v, sizeof(*v), 1, f))
		return -1;
	return 0;
}

int chunk_decode(struct wav_chunk *chunk, FILE *f)
{
	if (1 != fread(chunk->id, sizeof(chunk->id), 1, f))
		return -1;

	return read_u32(f, &chunk->size);
}

int wav_header_decode(struct AudioFileWavePcm::WaveFormat *fmt, size_t *datasize, FILE *f)
{
	struct wav_chunk header, format, chunk;
	uint8_t rifftype[4];        /* "WAVE" */
	int err = 0;

	err = chunk_decode(&header, f);
	if (err)
		return err;

	if (memcmp(header.id, "RIFF", 4)) {
		LOG("RIFF id not matching");
		return -1;
	}

	if (1 != fread(rifftype, sizeof(rifftype), 1, f))
		return ferror(f);

	if (memcmp(rifftype, "WAVE", 4)) {
		LOG("WAVE id not matching");
		return -2;
	}

	err = chunk_decode(&format, f);
	if (err)
		return err;

	if (memcmp(format.id, "fmt ", 4)) {
		LOG("fmt id not matching");
		return -3;
	}

	if (format.size < WAVE_FMT_SIZE)
		return -4;

	err  = read_u16(f, &fmt->format);
	err |= read_u16(f, &fmt->channels);
	err |= read_u32(f, &fmt->srate);
	err |= read_u32(f, &fmt->byterate);
	err |= read_u16(f, &fmt->block_align);
	err |= read_u16(f, &fmt->bps);
	if (err)
		return -5;

	/* skip any extra bytes */
	if (format.size >= (WAVE_FMT_SIZE + 2)) {
		err = read_u16(f, &fmt->extra);
		if (err)
			return -6;

		if (fmt->extra > 0) {
			if (fseek(f, fmt->extra, SEEK_CUR))
				return -7;
		}
	}

	/* fast forward to "data" chunk */
	for (;;) {

		err = chunk_decode(&chunk, f);
		if (err)
			return -8;

		if (chunk.size > header.size) {
			LOG("chunk size too large (%u > %u)", chunk.size, header.size);
			return -9;
		}

		if (0 == memcmp(chunk.id, "data", 4)) {
			*datasize = chunk.size;
			break;
		}

		if (fseek(f, chunk.size, SEEK_CUR) < 0)
			return -10;
	}

	return 0;
}

int fsize(FILE *fp)
{
	int prev = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	int sz=ftell(fp);
	fseek(fp, prev, SEEK_SET); //go back to where we were
	return sz;
}


}


AudioFileWavePcm::AudioFileWavePcm(void):
	dataStartPosition(0),
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
	if (file)
	{
		fclose(file);
		file = NULL;
	}
	file = fopen(fileName.c_str(), "rb");
	if (file == NULL)
	{
		LOG("Failed to open %s", fileName.c_str());
		return -1;
	}

	size_t datasize = 0;
	if (wav_header_decode(&waveFormat, &datasize, file) != 0)
	{
		return -1;
	}

	if (waveFormat.format != WAVE_FORMAT_PCM)
	{
		LOG("Expecting WAVE PCM format, found: %u", static_cast<unsigned int>(waveFormat.format));
		return -2;
	}

	if (waveFormat.channels != 1 && waveFormat.channels != 2)
	{
		LOG("Expecting 1 or 2 channels in WAVE file, found %u", static_cast<unsigned int>(waveFormat.channels));
		return -3;
	}

	if (waveFormat.bps != 16)
	{
		LOG("Wave: expecting 16 bits per sample, found %u", static_cast<unsigned int>(waveFormat.bps));
		return -4;
	}

	dataStartPosition = ftell(file);
	uint32_t fileSize = fsize(file);
	totalPcmSamples = (fileSize - dataStartPosition) / sizeof(int16_t) / waveFormat.channels;

	LOG("File %s: channels = %u, srate = %u, total PCM frames %u",
		fileName.c_str(), static_cast<unsigned int>(waveFormat.channels),
		waveFormat.srate,
		static_cast<unsigned int>(totalPcmSamples)); 

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

int AudioFileWavePcm::GetChannelsCount(void)
{
	return waveFormat.channels;
}

int AudioFileWavePcm::GetRealChannelsCount(void)
{
	return waveFormat.channels;
}

int AudioFileWavePcm::GetSampleRate(void)
{
	return waveFormat.srate;
}

int AudioFileWavePcm::GetSamples(int16_t *buf, int *count)
{
	unsigned int left = *count;
	unsigned int pos = 0;

	int16_t tmp[1024];

	while (left > 0 && !eof) {
		int count = fread(tmp, sizeof(tmp[0]), std::min(left, sizeof(tmp)/sizeof(tmp[0])), file);
		if (count <= 0) {
			LOG("End of wave file");
			eof = true;
		} else {
			for (int i=0; i<count; i++) {
				buf[pos++] = tmp[i];
			}
			left -= count;
		}
	}

	if (left > 0) {
		// Filling up to full frame
		memset(buf + *count - left, 0, left * sizeof(int16_t));
		*count -= left;
	}

	pcmPosition = (ftell(file) - dataStartPosition) / sizeof(int16_t) / waveFormat.channels;

	return 0;
}

int AudioFileWavePcm::SeekToPcmPosition(int64_t pos)
{
	if (file == NULL)
		return -1;
	return fseek(file, static_cast<long>(dataStartPosition + (pos * sizeof(int16_t) * waveFormat.channels)), SEEK_SET);
}

