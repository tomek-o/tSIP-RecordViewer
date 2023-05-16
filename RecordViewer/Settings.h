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
	bool Read(AnsiString asFileName);
	bool Write(AnsiString asFileName);

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
};

extern Settings appSettings;

#endif
