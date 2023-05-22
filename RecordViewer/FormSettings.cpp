//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSettings.h"
#include "AudioDevicesList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
__fastcall TfrmSettings::TfrmSettings(TComponent* Owner)
	: TForm(Owner)
{
	this->appSettings = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::FormShow(TObject *Sender)
{
    assert(appSettings);
	tmpSettings = *appSettings;
	chbAlwaysOnTop->Checked = tmpSettings.frmMain.bAlwaysOnTop;

	chbLogToFile->Checked = tmpSettings.Logging.bLogToFile;
	cmbMaxUiLogLines->ItemIndex = -1;
	for (int i=0; i<cmbMaxUiLogLines->Items->Count; i++)
	{
		if ((unsigned int)StrToInt(cmbMaxUiLogLines->Items->Strings[i]) >= tmpSettings.Logging.iMaxUiLogLines)
		{
			cmbMaxUiLogLines->ItemIndex = i;
			break;
		}
	}
	if (cmbMaxUiLogLines->ItemIndex == -1)
	{
		cmbMaxUiLogLines->ItemHeight = cmbMaxUiLogLines->Items->Count - 1;
	}

	edWhisperExe->Text = tmpSettings.Transcription.whisperExe;
	edWhisperModel->Text = tmpSettings.Transcription.model;
	edWhisperLanguage->Text = tmpSettings.Transcription.language;
	if (tmpSettings.Transcription.threadCount > 0 && tmpSettings.Transcription.threadCount < static_cast<unsigned int>(cbWhisperThreadCount->Items->Count))
	{
		cbWhisperThreadCount->ItemIndex = tmpSettings.Transcription.threadCount - 1;
	}
	else
	{
    	cbWhisperThreadCount->ItemIndex = 1;
	}

	RefreshAudioDevicesList();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnCancelClick(TObject *Sender)
{
	this->Close();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnApplyClick(TObject *Sender)
{
	tmpSettings.Logging.bLogToFile = chbLogToFile->Checked;

	if (cbAudioOutput->ItemIndex == 0)
	{
		// WAVE mapper / default device
		tmpSettings.Audio.outputDevice = "";
	}
	else if (cbAudioOutput->Tag == 0 || cbAudioOutput->ItemIndex != cbAudioOutput->Items->Count - 1)
	{
		tmpSettings.Audio.outputDevice = cbAudioOutput->Text.c_str();
	}

	tmpSettings.Transcription.whisperExe = edWhisperExe->Text;
	tmpSettings.Transcription.model = edWhisperModel->Text;
	tmpSettings.Transcription.language = edWhisperLanguage->Text;
	tmpSettings.Transcription.threadCount = StrToIntDef(cbWhisperThreadCount->Text, tmpSettings.Transcription.threadCount);

	*appSettings = tmpSettings;
	this->Close();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::chbAlwaysOnTopClick(TObject *Sender)
{
	tmpSettings.frmMain.bAlwaysOnTop = chbAlwaysOnTop->Checked;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cmbMaxUiLogLinesChange(TObject *Sender)
{
	tmpSettings.Logging.iMaxUiLogLines = StrToInt(cmbMaxUiLogLines->Text);	
}
//---------------------------------------------------------------------------

void TfrmSettings::RefreshAudioDevicesList(void)
{
	std::vector<AnsiString> dev_list;
	int iDevCount;

	AudioDevicesList::Instance().Refresh();
	dev_list = AudioDevicesList::Instance().winwaveDevsOut;


	cbAudioOutput->Items->Clear();
	iDevCount = dev_list.size();
	for(int i=0; i<iDevCount; i++)
	{
    	cbAudioOutput->Items->Add(dev_list[i].c_str());
    }


	for(int i=0; i<cbAudioOutput->Items->Count; i++)
	{
		if (tmpSettings.Audio.outputDevice == "")
		{
			cbAudioOutput->ItemIndex = 0;
			break;
		}
		if(cbAudioOutput->Items->Strings[i] == tmpSettings.Audio.outputDevice)
		{
            cbAudioOutput->ItemIndex = i;
            break;
		}
	}
	{
		TComboBox *target = cbAudioOutput;
		AnsiString selected = tmpSettings.Audio.outputDevice;
		// convention: if selected device is not found - add it at last item with [NOT FOUND] text and set non-zero Tag
		target->Tag = 0;
		if (target->ItemIndex < 0 && target->Items->Count > 0 && selected != "")
		{
			AnsiString text;
			text.sprintf("[NOT FOUND] %s", selected.c_str());
			target->Items->Add(text);
			target->ItemIndex = target->Items->Count - 1;
			target->Tag = 1;
		}
	}
}
void __fastcall TfrmSettings::btnSelectWhisperExeClick(TObject *Sender)
{
	if (openDialog->Execute())
		edWhisperExe->Text = openDialog->FileName;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnSelectWhisperModelClick(TObject *Sender)
{
	if (openDialog->Execute())
		edWhisperModel->Text = openDialog->FileName;
}
//---------------------------------------------------------------------------

