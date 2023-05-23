//---------------------------------------------------------------------------


#pragma hdrstop

#include "Transcription.h"
#include "Log.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#include <json/json.h>

int Transcription::fromJson(const Json::Value &jv)
{
	if (jv.type() != Json::objectValue)
	{
		LOG("Transcription: unexpected JSON type");
		return -1;
	}

	{
		const Json::Value &jOffsets = jv["offsets"];
		if (jOffsets.type() != Json::objectValue)
		{
			LOG("Transcription/offsets: unexpected JSON type");
			return -2;
		}

		const Json::Value &jFrom = jOffsets["from"];
		if (jFrom.isConvertibleTo(Json::uintValue) == false)
		{
			LOG("Transcription/offsets/from: unexpected JSON type");
			return -3;
		}
		const Json::Value &jTo = jOffsets["to"];
		if (jTo.isConvertibleTo(Json::uintValue) == false)
		{
			LOG("Transcription/offsets/to: unexpected JSON type");
			return -4;
		}
		offsetFrom = jFrom.asUInt();
		offsetTo = jTo.asUInt();
	}

	const Json::Value &jText = jv["text"];
	if (jText.type() != Json::stringValue)
	{
		LOG("Transcription/text: unexpected JSON type");
		return -5;
	}

	text = jText.asAString();

	return 0;
}

int TranscriptionData::fromJson(const Json::Value &root)
{
	transcriptions.clear();
	if (root.type() != Json::objectValue)
	{
		LOG("TranscriptionData: unextected JSON root type");
		return -1;
	}
	const Json::Value &jTranscription = root["transcription"];
	if (jTranscription.type() != Json::arrayValue)
	{
		LOG("TranscriptionData/transcription: unexpected type");
		return -2;
	}
	for (unsigned int i=0; i<jTranscription.size(); i++)
	{
		const Json::Value &jv = jTranscription[i];
		struct Transcription tr;
		if (tr.fromJson(jv) == 0)
		{
			transcriptions.push_back(tr);
		}
		else
		{
			LOG("Failed to load transcription at index %u", i);
		}
	}
	return 0;
}

