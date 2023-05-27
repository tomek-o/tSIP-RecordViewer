//---------------------------------------------------------------------------


#pragma hdrstop

#include "Record.h"
#include "AudioTranscriptionFileName.h"
#include "Log.h"
#include <SysUtils.hpp>
#include <json/json.h>
#include <fstream>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{

int LoadJson(AnsiString fileName, Json::Value &root)
{
	Json::Reader reader;

	try
	{
		std::ifstream ifs(fileName.c_str());
		std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		bool parsingSuccessful = reader.parse( strJson, root );
		if ( !parsingSuccessful )
		{
			LOG("Failed to parse text from %s as JSON", fileName.c_str());
			return 2;
		}
		else
		{
			return 0;
		}
	}
	catch(...)
	{
		LOG("Error reading %s", fileName.c_str());
		return 1;
	}
}

}

int S_RECORD::loadTranscription(void)
{
	hasMonoTranscription = false;
	hasStereoTranscription = false;
	fullTranscriptionTextLMono = "";
	fullTranscriptionTextR = "";

	AnsiString fileMono = GetTranscriptionFileName(asFilename, AUDIO_CHANNEL_MONO);
	AnsiString fileL = GetTranscriptionFileName(asFilename, AUDIO_CHANNEL_L);
	AnsiString fileR = GetTranscriptionFileName(asFilename, AUDIO_CHANNEL_R);

	if (FileExists(fileMono))
	{
		Json::Value root;
		if (LoadJson(fileMono, root) != 0)
		{
			return -1;
		}
		if (tdataLMono.fromJson(root) != 0)
		{
			return -1;
		}
		hasMonoTranscription = true;
		for (unsigned int i=0; i<tdataLMono.transcriptions.size(); i++)
		{
			const Transcription &tr = tdataLMono.transcriptions[i];
            fullTranscriptionTextLMono += (AnsiString)" " + tr.text;
		}
	}
	else
	{
		if (FileExists(fileL) && FileExists(fileR))
		{
			Json::Value rootL, rootR;
			if (LoadJson(fileL, rootL) != 0 || LoadJson(fileR, rootR) != 0)
			{
				return -1;
			}
			if (tdataLMono.fromJson(rootL) != 0 || tdataR.fromJson(rootR) != 0)
			{
				return -1;
			}
			hasStereoTranscription = true;
			for (unsigned int i=0; i<tdataLMono.transcriptions.size(); i++)
			{
				const Transcription &tr = tdataLMono.transcriptions[i];
				fullTranscriptionTextLMono += (AnsiString)" " + tr.text;
			}
			for (unsigned int i=0; i<tdataR.transcriptions.size(); i++)
			{
				const Transcription &tr = tdataR.transcriptions[i];
				fullTranscriptionTextR += (AnsiString)" " + tr.text;
			}
		}
		else
		{
			return -1;
		}
	}

	fullTranscriptionTextLMono = ::Utf8ToAnsi(fullTranscriptionTextLMono).LowerCase();
	fullTranscriptionTextR = ::Utf8ToAnsi(fullTranscriptionTextR).LowerCase();

	return 0;	
}
