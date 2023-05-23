//---------------------------------------------------------------------------

#ifndef AudioTranscriptionFileNameH
#define AudioTranscriptionFileNameH
//---------------------------------------------------------------------------

#include <System.hpp>

#include "AudioFileChannel.h"

AnsiString GetTranscriptionFileName(AnsiString audioFileName, AudioFileChannel channel);

#endif
