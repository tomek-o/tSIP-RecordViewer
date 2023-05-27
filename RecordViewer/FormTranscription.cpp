#include <vcl.h>
#pragma hdrstop

#include "FormTranscription.h"
#include "AudioTranscriptionFileName.h"
#include "Record.h"
#include "Transcription.h"
#include "Settings.h"
#include "Log.h"
#include <fstream>
#include <json/json.h>

#include <stdio.h>
#include <time.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"



//---------------------------------------------------------------------------
__fastcall TfrmTranscription::TfrmTranscription(TComponent* Owner, const struct S_RECORD &record)
	: TForm(Owner)
{
	UpdateTarget(target);
	AnsiString fileName = ExtractFileName(record.asFilename);
	edFile->Text = fileName;

	AnsiString caption;
	AnsiString other;
	other = "[empty number]";
	other = record.asDescription != "" ? record.asDescription : record.asNumber;
	if (record.dir == S_RECORD::DIR_IN)
	{
		caption.sprintf("Call from %s", other.c_str());
	}
	else
	{
		caption.sprintf("Call to %s", other.c_str());
	}
	SetTarget(caption);
	LoadTranscription(record, other);
}
//---------------------------------------------------------------------------
void __fastcall TfrmTranscription::MyWndProc (Messages::TMessage &Msg)
{
    if (Msg.Msg == WM_NOTIFY)
	{
		if (((NMHDR*)Msg.LParam)->code == EN_LINK)
        {
            ENLINK *E = (ENLINK*)Msg.LParam;
            if (E->msg == WM_LBUTTONDBLCLK)
				ShellExecute (NULL, NULL, memoMain->Text.SubString(E->chrg.cpMin + 1, E->chrg.cpMax - E->chrg.cpMin).c_str(), NULL, NULL, SW_SHOW);
		}
    }
	else
	{
		WndProc (Msg);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmTranscription::FormCreate(TObject *Sender)
{
    SetWindowLong (Handle, GWL_EXSTYLE, GetWindowLong (Handle, GWL_EXSTYLE) | WS_EX_APPWINDOW);

    SendMessage (memoMain->Handle, EM_AUTOURLDETECT, TRUE, 0);
    SendMessage (memoMain->Handle, EM_SETEVENTMASK, 0, ENM_LINK);
    WindowProc = MyWndProc;
}
//---------------------------------------------------------------------------

void __fastcall TfrmTranscription::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------

void TfrmTranscription::SetTarget(AnsiString target)
{
	UpdateTarget(target);
}

void TfrmTranscription::UpdateTarget(AnsiString val)
{
	target = val;
	AnsiString caption;
	caption.sprintf("Transcription: %s", target.c_str());
	this->Caption = caption;
}

void TfrmTranscription::AddSentence(const Transcription &tr, enum AudioFileChannel channel, AnsiString otherParty)
{
	memoMain->SelStart = memoMain->Lines->Text.Length();
	memoMain->SelAttributes->Size = 8;
	memoMain->SelAttributes->Style = TFontStyles() << fsBold;
	memoMain->SelAttributes->Color = clBlack;
	memoMain->Paragraph->FirstIndent = 0;

	AnsiString ts;
	if (channel == AUDIO_CHANNEL_L)
		ts.sprintf("Me, from %.1f s to %.1f s:", Transcription::offsetToSeconds(tr.offsetFrom), Transcription::offsetToSeconds(tr.offsetTo));
	else if (channel == AUDIO_CHANNEL_R)
		ts.sprintf("%s, from %.1f s to %.1f s:", otherParty.c_str(), Transcription::offsetToSeconds(tr.offsetFrom), Transcription::offsetToSeconds(tr.offsetTo));
	else
		ts.sprintf("From %.1f s to %.1f s:", Transcription::offsetToSeconds(tr.offsetFrom), Transcription::offsetToSeconds(tr.offsetTo));
	memoMain->Lines->Add(ts);

	memoMain->SelAttributes->Size = 10;
	memoMain->SelAttributes->Style = TFontStyles();
	if (channel == AUDIO_CHANNEL_L)
		memoMain->SelAttributes->Color = clBlue;
	else if (channel == AUDIO_CHANNEL_R)
		memoMain->SelAttributes->Color = clRed;
	else
		memoMain->SelAttributes->Color = clGray;
	memoMain->Paragraph->FirstIndent = 10;
	AnsiString asText = ::Utf8ToAnsi(tr.text);
	memoMain->Lines->Add(asText);
}

int TfrmTranscription::LoadTranscription(const struct S_RECORD &record, AnsiString otherParty)
{
	if (record.hasMonoTranscription)
	{
		const TranscriptionData &tdata = record.getTranscriptionMono();
		for (unsigned int i=0; i<tdata.transcriptions.size(); i++)
		{
			const Transcription &tr = tdata.transcriptions[i];
			AddSentence(tr, AUDIO_CHANNEL_MONO, "");
		}
	}
	else if (record.hasStereoTranscription)
	{
		TranscriptionData tdataL = record.getTranscriptionL();
		TranscriptionData tdataR = record.getTranscriptionR();
		// merge L and R transcriptions by "to" time ("from" time is very unprecise in my whisper.cpp tests)
		unsigned int posL = 0, posR = 0;
		for(;;)
		{
			unsigned int timeL = 0xFFFFFFFF, timeR = 0xFFFFFFFF;
			if (posL < tdataL.transcriptions.size())
				timeL = tdataL.transcriptions[posL].offsetTo;
			if (posR < tdataR.transcriptions.size())
				timeR = tdataR.transcriptions[posR].offsetTo;
			if (timeL == 0xFFFFFFFF && timeR == 0xFFFFFFFF)
				break;
			if (timeL < timeR)
			{
				const Transcription &tr = tdataL.transcriptions[posL++];
				AddSentence(tr, AUDIO_CHANNEL_L, otherParty);
			}
			else
			{
				const Transcription &tr = tdataR.transcriptions[posR++];
				AddSentence(tr, AUDIO_CHANNEL_R, otherParty);
			}
		}
	}
	else
	{
		memoMain->Text = "Transcription files not found!\r\nTry running transcription for the selected file first.";
		return -1;
	}

    SendMessage (memoMain->Handle, WM_VSCROLL, SB_TOP, NULL);	
	return 0;
}

