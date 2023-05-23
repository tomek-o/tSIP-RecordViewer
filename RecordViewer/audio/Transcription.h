//---------------------------------------------------------------------------

#ifndef TranscriptionH
#define TranscriptionH
//---------------------------------------------------------------------------

#include <System.hpp>
#include <vector>

namespace Json
{
	class Value;
}

struct Transcription
{
	unsigned int offsetFrom;
	unsigned int offsetTo;
	AnsiString text;
	Transcription(void):
		offsetFrom(0),
		offsetTo(0)
	{}
	int fromJson(const Json::Value &jv);
};

struct TranscriptionData
{
	std::vector<Transcription> transcriptions;
	int fromJson(const Json::Value &jv);
};

#endif
