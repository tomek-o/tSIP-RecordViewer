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

	enum { OFFSET_PER_SECOND = 1000 };

	static float offsetToSeconds(unsigned int offset)
	{
		return static_cast<float>(offset) / OFFSET_PER_SECOND;
	}
};

struct TranscriptionData
{
	std::vector<Transcription> transcriptions;
	int fromJson(const Json::Value &jv);
};

#endif
