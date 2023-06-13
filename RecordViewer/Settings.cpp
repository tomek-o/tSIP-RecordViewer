//---------------------------------------------------------------------------


#pragma hdrstop

#include "Settings.h"
#include <algorithm>
#include <fstream>
#include <json/json.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

Settings appSettings;

inline void strncpyz(char* dst, const char* src, int dstsize) {
	strncpy(dst, src, dstsize);
	dst[dstsize-1] = '\0';
}

const char* Settings::Recordings::getTranscriptionFilterName(enum Settings::Recordings::TranscriptionFilter type)
{
	switch (type)
	{
	case TR_FILTER_META_ONLY:
		return "meta only";
	case TR_FILTER_META_ALL_TRANSCRIPTIONS:
		return "meta + all transcribed text";
	case TR_FILTER_META_LOCAL_TRANSCRIPTIONS:
		return "meta + local text";
	case TR_FILTER_META_2ND_PARTY_TRANSCRIPTIONS:
		return "meta + 2nd party text";
	case TR_FILTER_ALL_TRANSCRIPTIONS:
		return "all transcribed text";
	case TR_FILTER_LOCAL_TRANSCRIPTIONS:
		return "local text";
	case TR_FILTER_2ND_PARTY_TRANSCRIPTIONS:
		return "2nd party text";
	default:
		return "???";
	}
}

int Settings::Read(AnsiString asFileName)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

	try
	{
		std::ifstream ifs(asFileName.c_str());
		std::string strConfig((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		bool parsingSuccessful = reader.parse( strConfig, root );
		if ( !parsingSuccessful )
		{
			return 2;
		}
	}
	catch(...)
	{
		return 1;
	}

	int maxX = GetSystemMetrics(SM_CXSCREEN);
	/** \todo Ugly fixed taskbar margin */
	int maxY = GetSystemMetrics(SM_CYSCREEN) - 32;

	{
		const Json::Value &frmMainJson = root["frmMain"];
		frmMainJson.getInt("width", frmMain.width);
		frmMainJson.getInt("height", frmMain.height);
		if (frmMain.width < 250 || frmMain.width > maxX + 20)
		{
			frmMain.width = 400;
		}
		if (frmMain.height < 200 || frmMain.height > maxY + 20)
		{
			frmMain.height = 300;
		}
		frmMainJson.getInt("positionX", frmMain.posX);
		frmMainJson.getInt("positionY", frmMain.posY);
		if (frmMain.posX < 0)
			frmMain.posX = 0;
		if (frmMain.posY < 0)
			frmMain.posY = 0;
		if (frmMain.posX + frmMain.width > maxX)
			frmMain.posX = maxX - frmMain.width;
		if (frmMain.posY + frmMain.height > maxY)
			frmMain.posY = maxY - frmMain.height;
		frmMainJson.getBool("maximized", frmMain.windowMaximized);
		frmMainJson.getBool("alwaysOnTop", frmMain.alwaysOnTop);
	}

	{
		const Json::Value &jv = root["logging"];
		jv.getBool("logToFile", logging.logToFile);
		jv.getBool("flush", logging.flush);
		jv.getInt("maxFileSize", logging.maxFileSize);
		if (logging.maxFileSize < Settings::Logging::MIN_MAX_FILE_SIZE || logging.maxFileSize > Settings::Logging::MAX_MAX_FILE_SIZE)
		{
			logging.maxFileSize = Settings::Logging::DEF_MAX_FILE_SIZE;
		}
		jv.getUInt("maxUiLogLines", logging.maxUiLogLines);
	}

	{
		const Json::Value &jv = root["contacts"];
		jv.getAString("fileName", contacts.fileName);
	}

	{
		const Json::Value &jv = root["audio"];
		jv.getAString("outputDevice", audio.outputDevice);
	}

	{
		const Json::Value &jv = root["transcription"];
		jv.getAString("whisperExe", transcription.whisperExe);
		jv.getAString("model", transcription.model);
		jv.getAString("language", transcription.language);
		jv.getUInt("threadCount", transcription.threadCount);
	}

	{
		const Json::Value &jv = root["recordings"];
		{
			int tmp = recordings.transcriptionFilter;
			jv.getInt("transcriptionFilter", tmp);
			if (tmp >= 0 && tmp < Recordings::TR_FILTER__LIMITER)
			{
				recordings.transcriptionFilter = static_cast<Recordings::TranscriptionFilter>(tmp);
			}
		}
	}

	return 0;
}

int Settings::Write(AnsiString asFileName)
{
	Json::Value root;
	Json::StyledWriter writer;

	{
		Json::Value &jv = root["frmMain"];
		jv["width"] = frmMain.width;
		jv["height"] = frmMain.height;
		jv["positionX"] = frmMain.posX;
		jv["positionY"] = frmMain.posY;
		jv["maximized"] = frmMain.windowMaximized;
		jv["alwaysOnTop"] = frmMain.alwaysOnTop;
	}

	{
		Json::Value &jv = root["logging"];
		jv["logToFile"] = logging.logToFile;
		jv["flush"] = logging.flush;
		jv["maxFileSize"] = logging.maxFileSize;
		jv["maxUiLogLines"] = logging.maxUiLogLines;
	}

	{
		Json::Value &jv = root["contacts"];
		jv["fileName"] = contacts.fileName;
	}

	{
		Json::Value &jv = root["audio"];
		jv["outputDevice"] = audio.outputDevice;
	}

	{
		Json::Value &jv = root["transcription"];
		jv["whisperExe"] = transcription.whisperExe;
		jv["model"] = transcription.model;
		jv["language"] = transcription.language;
		jv["threadCount"] = transcription.threadCount;
	}

	{
		Json::Value &jv = root["recordings"];
		{
			jv["transcriptionFilter"] = recordings.transcriptionFilter;
		}
	}

	std::string outputConfig = writer.write( root );

	try
	{
		std::ofstream ofs(asFileName.c_str());
		ofs << outputConfig;
		ofs.close();
	}
	catch(...)
	{
    	return 1;
	}

	return 0;

}
