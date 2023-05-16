//---------------------------------------------------------------------------


#pragma hdrstop

#include <IniFiles.hpp>
#include "Settings.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

Settings appSettings;

bool Settings::Read(AnsiString asFileName)
{
	TIniFile *ini = NULL;
	try
	{
		ini = new TIniFile(asFileName);
		// main window startup position
		int maxX = GetSystemMetrics(SM_CXSCREEN);
		int maxY = GetSystemMetrics(SM_CYSCREEN);
		frmMain.iWidth = ini->ReadInteger("frmMain", "AppWidth", 350);
		frmMain.iHeight = ini->ReadInteger("frmMain", "AppHeight", 300);
		if (frmMain.iWidth < 250 || frmMain.iWidth > maxX + 20)
		{
			frmMain.iWidth = 250;
		}
		if (frmMain.iHeight < 200 || frmMain.iHeight > maxY + 20)
		{
			frmMain.iHeight = 200;
		}
		frmMain.iPosX = ini->ReadInteger("frmMain", "AppPositionX", 30);
		frmMain.iPosY = ini->ReadInteger("frmMain", "AppPositionY", 30);
		if (frmMain.iPosX < 0)
			frmMain.iPosX = 0;
		if (frmMain.iPosY < 0)
			frmMain.iPosY = 0;
		if (frmMain.iPosX + frmMain.iWidth > maxX)
			frmMain.iPosX = maxX - frmMain.iWidth;
		if (frmMain.iPosY + frmMain.iHeight > maxY)
			frmMain.iPosY = maxY - frmMain.iHeight;
		frmMain.bWindowMaximized = ini->ReadBool("frmMain", "Maximized", false);
		frmMain.bAlwaysOnTop = ini->ReadBool("frmMain", "AlwaysOnTop", false);

		Logging.bLogToFile = ini->ReadBool("Logging", "LogToFile", false);
		Logging.iMaxUiLogLines = ini->ReadInteger("Logging", "MaxUiLogLines", 1000);

		Contacts.fileName = ini->ReadString("Contacts", "FileName", "");

		delete ini;
	}
	catch (...)
	{
		if (ini)
		{
			delete ini;
		}
		return false;
	}
	return true;
}

bool Settings::Write(AnsiString asFileName)
{
	TIniFile *ini = NULL;
	try
	{
		ini = new TIniFile(asFileName);
		ini->WriteInteger("frmMain", "AppWidth", frmMain.iWidth);
		ini->WriteInteger("frmMain", "AppHeight", frmMain.iHeight);
		ini->WriteInteger("frmMain", "AppPositionX", frmMain.iPosX);
		ini->WriteInteger("frmMain", "AppPositionY", frmMain.iPosY);
		ini->WriteBool("frmMain", "Maximized", frmMain.bWindowMaximized);
		ini->WriteBool("frmMain", "AlwaysOnTop", frmMain.bAlwaysOnTop);

		ini->WriteBool("Logging", "LogToFile", Logging.bLogToFile);
		ini->WriteInteger("Logging", "MaxUiLogLines", Logging.iMaxUiLogLines);

		ini->WriteString("Contacts", "FileName", Contacts.fileName);

		delete ini;
	}
	catch (...)
	{
		if (ini)
		{
			delete ini;
		}
		return false;
	}
	return true;

}
