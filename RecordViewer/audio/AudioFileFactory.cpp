//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioFileFactory.h"
#include "AudioFileOggOpus.h"
#include "AudioFileWavePcm.h"
#include "Log.h"
#include <SysUtils.hpp>


//---------------------------------------------------------------------------

#pragma package(smart_init)

AudioFile* CreateAudioFile(AnsiString fileName)
{
    int status;
	AudioFile *file = NULL;
	AnsiString ext = ExtractFileExt(fileName);
	if (ext == ".ogg")
	{
		file = new AudioFileOggOpus();
		status = file->Open(fileName);
		if (status != 0)
		{
			delete file;
			file = NULL;
		}
	}
	else if (ext == ".wav")
	{
		file = new AudioFileWavePcm();
		status = file->Open(fileName);
		if (status != 0)
		{
			delete file;
			file = NULL;
		}
	}
	else
	{
		LOG("Unhandled file extension!");
	}

	return file;
}
