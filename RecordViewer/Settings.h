/** \file
*/
//---------------------------------------------------------------------------

#ifndef SettingsH
#define SettingsH
//---------------------------------------------------------------------------
#include <System.hpp>

class Settings
{
public:
	int Read(AnsiString asFileName);
	int Write(AnsiString asFileName);

	struct Gui
	{
		enum { SCALING_MIN = 50 };
		enum { SCALING_MAX = 200 };
		int scalingPct;					///< scaling (percentage)
	} gui;
	struct FrmMain
	{
		int posX, posY;				///< main window coordinates
		int height, width;			///< main window size
		bool windowMaximized;			///< is main window maximize?
		bool alwaysOnTop;
		FrmMain(void):
			width(600),
			height(500),
			posX(30),
			posY(30),
			windowMaximized(false),
			alwaysOnTop(false)
		{}
	} frmMain;
	struct Logging
	{
		bool logToFile;
		bool flush;
		enum {
			MIN_MAX_FILE_SIZE = 0,
			MAX_MAX_FILE_SIZE = 1000*1024*1024
		};
		enum {
			DEF_MAX_FILE_SIZE = 10*1024*1024
		};
		int maxFileSize;
		unsigned int maxUiLogLines;
		Logging(void):
			logToFile(false),
			flush(false),
			maxFileSize(DEF_MAX_FILE_SIZE),
			maxUiLogLines(5000)
		{}
	} logging;

	struct Contacts
	{
		AnsiString fileName;
	} contacts;

	struct Recordings
	{
		bool decodeBase64Uri;
		enum TranscriptionFilter
		{
			TR_FILTER_META_ONLY = 0,
			TR_FILTER_META_ALL_TRANSCRIPTIONS,
			TR_FILTER_META_LOCAL_TRANSCRIPTIONS,
			TR_FILTER_META_2ND_PARTY_TRANSCRIPTIONS,
			TR_FILTER_ALL_TRANSCRIPTIONS,
			TR_FILTER_LOCAL_TRANSCRIPTIONS,
			TR_FILTER_2ND_PARTY_TRANSCRIPTIONS,
			TR_FILTER__LIMITER
		} transcriptionFilter;
		static const char* getTranscriptionFilterName(enum TranscriptionFilter type);
		Recordings(void):
			decodeBase64Uri(true),
			transcriptionFilter(TR_FILTER_META_ALL_TRANSCRIPTIONS)
		{}
	} recordings;

	struct Audio
	{
		AnsiString outputDevice;
	} audio;

	struct Transcription
	{
		AnsiString whisperExe;
		AnsiString model;
		AnsiString language;
		unsigned int threadCount;
		Transcription(void):
			whisperExe("whisper.cpp\\main.exe"),
			model("whisper.cpp\\models\\ggml-base.en.bin"),
			threadCount(2)
		{}
	} transcription;
};

extern Settings appSettings;

#endif
