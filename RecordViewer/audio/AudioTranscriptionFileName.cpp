//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioTranscriptionFileName.h"
#include <SysUtils.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{

AnsiString ExtractFileNameWithoutExt(AnsiString fileName)
{
	fileName = ExtractFileName(fileName);
	const char* lastDot = strrchr(fileName.c_str(), '.');
	if (lastDot)
	{
		fileName = fileName.SubString(1, lastDot - fileName.c_str());
	}
	return fileName;
}

}


AnsiString GetTranscriptionFileName(AnsiString audioFileName, AudioFileChannel channel)
{
	return ExtractFilePath(audioFileName) + ExtractFileNameWithoutExt(audioFileName) + "_" + GetAudioFileChannelName(channel) + ".json";
}
