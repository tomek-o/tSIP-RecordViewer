//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("FormMain.cpp", frmMain);
USEFORM("FormSettings.cpp", frmSettings);
USEFORM("FormAbout.cpp", frmAbout);
USEFORM("LogUnit.cpp", frmLog);
USEFORM("FormRecordings.cpp", frmRecordings);
USEFORM("FormTranscription.cpp", frmTranscription);
//---------------------------------------------------------------------------
#pragma link "common.lib"
#pragma link "jsoncpp.lib"
#pragma link "ogg.lib"
#pragma link "libopus.lib"
#pragma link "libopusfile.lib"
#pragma link "libspeex.lib"


#include "Log.h"


WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "Record Viewer";
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmSettings), &frmSettings);
		Application->CreateForm(__classid(TfrmAbout), &frmAbout);
		Application->CreateForm(__classid(TfrmLog), &frmLog);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	CLog::Instance()->Destroy();	
	return 0;
}
//---------------------------------------------------------------------------
