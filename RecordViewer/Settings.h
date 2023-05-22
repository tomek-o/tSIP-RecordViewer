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
	void SetDefault(void);
	struct _gui
	{
		enum { SCALING_MIN = 50 };
		enum { SCALING_MAX = 200 };
		int scalingPct;					///< scaling (percentage)
	} gui;
	struct _frmMain
	{
		int iPosX, iPosY;				///< main window coordinates
		int iHeight, iWidth;			///< main window size
		bool bWindowMaximized;			///< is main window maximize?
		bool bAlwaysOnTop;
	} frmMain;
	struct _Logging
	{
		bool bLogToFile;
		bool bFlush;
		enum {
			MIN_MAX_FILE_SIZE = 0,
			MAX_MAX_FILE_SIZE = 1000*1024*1024
		};
		enum {
            DEF_MAX_FILE_SIZE = 10*1024*1024
        };
		int iMaxFileSize;
		unsigned int iMaxUiLogLines;
	} Logging;

	struct _Contacts
	{
		AnsiString fileName;
	} Contacts;

	struct _Recordings
	{
    	bool decodeBase64Uri;
	} Recordings;
	
	struct _Audio
	{
		AnsiString outputDevice;
	} Audio;

	struct _Transcription
	{
		AnsiString whisperExe;
		AnsiString model;
		AnsiString language;
		unsigned int threadCount;
		_Transcription(void):
			whisperExe("whisper.cpp\\main.exe"),
			model("whisper.cpp\\models\\ggml-base.en.bin"),
			threadCount(2)
		{}
	} Transcription;
};

extern Settings appSettings;

#endif
