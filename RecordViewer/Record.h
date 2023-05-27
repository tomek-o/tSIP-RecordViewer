//---------------------------------------------------------------------------

#ifndef RecordH
#define RecordH
//---------------------------------------------------------------------------

#include "Transcription.h"
#include <System.hpp>

struct S_RECORD
{
	AnsiString asFilename;
	enum DIR {
		DIR_UNKNOWN = 0,
		DIR_IN,
		DIR_OUT
	} dir;
	AnsiString asDateTime;
	AnsiString asNumber;
	AnsiString asDescription;
	int size;
	TranscriptionData tdataLMono;	// transcription for mono file or left channel of stereo file
	AnsiString fullTranscriptionTextLMono;
	TranscriptionData tdataR;		// transcription for right channel of stereo file
	AnsiString fullTranscriptionTextR;
	bool hasMonoTranscription;
	bool hasStereoTranscription;

	S_RECORD(void):
		dir(DIR_UNKNOWN),
		size(0),
		hasMonoTranscription(false),
		hasStereoTranscription(false)
	{}

	bool hasTranscription(void) const
	{
    	return hasMonoTranscription || hasStereoTranscription;
	}

	int loadTranscription(void);

	const TranscriptionData& getTranscriptionMono(void) const
	{
		return tdataLMono;
	}

	const TranscriptionData& getTranscriptionL(void) const
	{
		return tdataLMono;
	}
	const TranscriptionData& getTranscriptionR(void) const
	{
		return tdataR;
	}
};

#endif
