//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMain.h"
#include "FormAbout.h"
#include "FormSettings.h"
#include "FormRecordings.h"
#include "audio/AudioDevicesList.h"
#include "LogUnit.h"
#include "Log.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------


__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
	frmRecordings = new TfrmRecordings(this);
	frmRecordings->Parent = this;
	frmRecordings->Visible = true;
	AudioDevicesList::Instance().Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".json" );
	appSettings.Read(asConfigFile);
	if (this->BorderStyle != bsSingle)
	{
		this->Width = appSettings.frmMain.width;
		this->Height = appSettings.frmMain.height;
	}
	this->Top = appSettings.frmMain.posY;
	this->Left = appSettings.frmMain.posX;
	if (appSettings.frmMain.alwaysOnTop)
		this->FormStyle = fsStayOnTop;
	else
		this->FormStyle = fsNormal;
	if (appSettings.frmMain.windowMaximized)
		this->WindowState = wsMaximized;
	if (appSettings.logging.logToFile)
		CLog::Instance()->SetFile(ChangeFileExt(Application->ExeName, ".log").c_str());
	else
		CLog::Instance()->SetFile("");
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	frmRecordings->StopTranscribing();

	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".json" );
	appSettings.frmMain.windowMaximized = (this->WindowState == wsMaximized);
	if (!appSettings.frmMain.windowMaximized)
	{
		// these values are meaningless is wnd is maximized
		appSettings.frmMain.width = this->Width;
		appSettings.frmMain.height = this->Height;
		appSettings.frmMain.posY = this->Top;
		appSettings.frmMain.posX = this->Left;
	}
	appSettings.Write(asConfigFile);

	CanClose = true;	
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actShowAboutExecute(TObject *Sender)
{
	frmAbout->ShowModal();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actShowSettingsExecute(TObject *Sender)
{
	frmSettings->appSettings = &appSettings;
	frmSettings->ShowModal();
	if (appSettings.frmMain.alwaysOnTop)
		this->FormStyle = fsStayOnTop;
	else
		this->FormStyle = fsNormal;
	if (appSettings.logging.logToFile)
		CLog::Instance()->SetFile(ChangeFileExt(Application->ExeName, ".log").c_str());
	else
		CLog::Instance()->SetFile("");
	frmLog->SetLogLinesLimit(appSettings.logging.maxUiLogLines);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormShow(TObject *Sender)
{
    static bool once = false;
    if (!once)
    {
		once = true;
		frmLog->SetLogLinesLimit(appSettings.logging.maxUiLogLines);		
		CLog::Instance()->SetLevel(E_LOG_TRACE);
		CLog::Instance()->callbackLog = frmLog->OnLog;
	}
	LOG("Application started");
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	CLog::Instance()->Destroy();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actShowLogExecute(TObject *Sender)
{
	if (!frmLog->Visible)
		frmLog->Show();
	frmLog->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miStopTranscribingClick(TObject *Sender)
{
	frmRecordings->StopTranscribing();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miGenerateMissingTranscriptionsForFilteredFilesClick(
      TObject *Sender)
{
	frmRecordings->GenerateMissingTranscriptionsForFilteredFiles();	
}
//---------------------------------------------------------------------------

