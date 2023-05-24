//---------------------------------------------------------------------------


#pragma hdrstop

#include "Record.h"
#include "AudioTranscriptionFileName.h"
#include <SysUtils.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)

bool S_RECORD::hasTranscription(void) const
{
	AnsiString fileMono = GetTranscriptionFileName(asFilename, AUDIO_CHANNEL_MONO);
	AnsiString fileL = GetTranscriptionFileName(asFilename, AUDIO_CHANNEL_L);
	AnsiString fileR = GetTranscriptionFileName(asFilename, AUDIO_CHANNEL_R);
	if (FileExists(fileMono) || (FileExists(fileL) && FileExists(fileR)))
		return true;
	return false;
}
