//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormRecordings.h"
#include "Contacts.h"
#include "Settings.h"
#include "AudioFileTranscription.h"
#include "common/BtnController.h"
#include "common/TimeCounter.h"
#include "MyTrackBar.h"
#include "common/base64.h"
#include <Clipbrd.hpp>
#include <algorithm>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma warn -8091
#pragma resource "*.dfm"
TfrmRecordings *frmRecordings;

namespace {
	Contacts contacts;
	enum { TRACKBAR_TICKS_PER_SECOND = 10 };
	AudioFileTranscription transcription;	
}

//---------------------------------------------------------------------------
__fastcall TfrmRecordings::TfrmRecordings(TComponent* Owner)
	: TForm(Owner)
{

	sorting.column = -1;
	sorting.forward = false;

#if 0
	S_RECORD record;
	record.asFilename = "20110824_163357_M_586_6240_.wav";
	record.size = 666;
	records.push_back(record);
	Filter();
#endif
	((TMyTrackBar*)TrackBar)->OnMouseUp = TrackBarMouseUp;	
	((TMyTrackBar*)TrackBar)->OnMouseDown = TrackBarMouseDown;

	unsigned long vol;
	waveOutGetVolume(0, &vol);
	vol &= 0xFFFF;
	trbarVolume->Position = vol * (-655);

	lvRecords->Columns->Items[4]->Width = 0;	// filename
	lvRecords->DoubleBuffered = true;

	btnPlay->Enabled = true;
	btnStop->Enabled = false;
	btnPause->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::lvRecordsData(TObject *Sender, TListItem *Item)
{
	int id = Item->Index;
	S_RECORD &record = records_filtered[id];
	if (record.dir == S_RECORD::DIR_IN)
		Item->ImageIndex = 0;
	else if (record.dir == S_RECORD::DIR_OUT)
		Item->ImageIndex = 1;
	else
		Item->ImageIndex = -1;
	Item->SubItems->Add(record.asDateTime);
	Item->SubItems->Add(record.asNumber);
	Item->SubItems->Add(record.asDescription);
	Item->SubItems->Add(record.asFilename);
	Item->SubItems->Add(ExtractFileExt(record.asFilename));
	Item->SubItems->Add(record.size);
}
//---------------------------------------------------------------------------

void TfrmRecordings::Filter(void)
{
	for (unsigned int i=0; i<records.size(); i++)
	{
        records[i].id = i;
    }
	if (edFilter->Text == "")
	{
    	records_filtered = records;
	}
	else
	{
		records_filtered.clear();
		records_filtered.reserve(records.size());

		AnsiString asFilter = edFilter->Text;
		for (unsigned int i=0; i<records.size(); i++)
		{
			S_RECORD &record = records[i];
			bool match = false;
			if (record.asDateTime.Pos(asFilter))
			{
                match = true;
			}
			else if (record.asNumber.Pos(asFilter))
			{
				match = true;
			}
			else if (UpperCase(record.asDescription).Pos(UpperCase(asFilter)))
			{
				match = true;
			}

			if (match)
			{
				records_filtered.push_back(record);
            }
		}
	}

	lvRecords->Items->Count = records_filtered.size();

	if (sorting.column >= 0)
	{
		std::stable_sort(records_filtered.begin(), records_filtered.end(), v_compare_records);
    }

	lvRecords->Invalidate();
	lvRecords->Height += 1;
	lvRecords->Height -= 1;

//	lvRecords->Columns->Items[0]->AutoSize = true;
//	lvRecords->Columns->Items[0]->Width -= 20;
//	lvRecords->Columns->Items[0]->AutoSize = false;

	unsigned long long totalSize = 0;
	for (unsigned int i=0; i<records_filtered.size(); i++) {
		totalSize += records_filtered[i].size;
	}

	lblItemsCount->Caption = lvRecords->Items->Count;
	double mBytes = (double)totalSize / (1024*1024);
	lblItemsSize->Caption = FloatToStrF(mBytes, ffFixed, 8, 1) + " MB";
}


char *memfind(const char *buf,const char *tofind,size_t len)
{
	size_t findlen=strlen(tofind);
	if(findlen>len)
	{  return((char*)NULL);  }
	if(len<1)
	{  return((char*)buf);  }
	
	{
		const char *bufend=&buf[len-findlen+1];
		const char *c=buf;
		for(; c<bufend; c++)
		{
			if(*c==*tofind)  // first letter matches
			{
				if(!memcmp(c+1,tofind+1,findlen-1))  // found
				{  return((char*)c);  }
			}
		}
	}
	
	return((char*)NULL);
}

void __fastcall TfrmRecordings::miPopupRecordsCopyClick(TObject *Sender)
{
	if (!lvRecords->Selected)
		return;
	int id = lvRecords->Selected->Index;
	AnsiString filename = records_filtered[id].asFilename;
	Clipboard()->SetTextBuf(filename.c_str());
}
//---------------------------------------------------------------------------


bool __fastcall TfrmRecordings::v_compare_records(const TfrmRecordings::S_RECORD& d1, const TfrmRecordings::S_RECORD& d2)
{
	int column = frmRecordings->sorting.column;
	bool forward = frmRecordings->sorting.forward;
	if (column == 0)
	{
		if (forward)
			return d1.dir > d2.dir;
		else
			return d1.dir < d2.dir;
	}
	else if (column == 1)
	{
		if(forward)
			return d1.asDateTime > d2.asDateTime;
		else
			return d1.asDateTime < d2.asDateTime;
	}
	else if (column == 2)
	{
		if(forward)
			return d1.asNumber > d2.asNumber;
		else
			return d1.asNumber < d2.asNumber;
	}
	else if (column == 3)
	{
		if(forward)
			return d1.asDescription > d2.asDescription;
		else
			return d1.asDescription < d2.asDescription;
	}
	else if (column == 4)
	{
		if(forward)
			return d1.asFilename > d2.asFilename;
		else
			return d1.asFilename < d2.asFilename;
	}
	else
	{
		if(forward)
			return d1.size > d2.size;
		else
			return d1.size < d2.size;
	}
}

void __fastcall TfrmRecordings::lvRecordsColumnClick(TObject *Sender,
      TListColumn *Column)
{
	sorting.column = Column->Index;
	for (int i=0; i<lvRecords->Columns->Count; i++)
	{
		if (i == Column->Index)
			continue;
		lvRecords->Columns->Items[i]->ImageIndex = -1;
	}
	if (Column->ImageIndex == 0)
		Column->ImageIndex = 1;
	else
		Column->ImageIndex = 0;
	sorting.forward = (Column->ImageIndex == 0);

	std::stable_sort(records_filtered.begin(), records_filtered.end(), v_compare_records);

	lvRecords->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::edFilterChange(TObject *Sender)
{
	Filter();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::tmrRefreshPlayerPositionTimer(
      TObject *Sender)
{
	if (player.IsPlaying() == false)
	{
		tmrRefreshPlayerPosition->Enabled = false;
		TrackBar->Visible = false;
		btnPlay->Enabled = true;
		btnStop->Enabled = false;
		btnPause->Enabled = false;
		return;
	}
	if (player.IsPaused())
		return;

	TrackBar->Max = static_cast<int>(player.GetTotalPcmSamples() * TRACKBAR_TICKS_PER_SECOND / player.GetSampleRate());
	TrackBar->Position = static_cast<int>(player.GetPcmPosition() * TRACKBAR_TICKS_PER_SECOND / player.GetSampleRate());
	btnPlay->Enabled = false;
	btnStop->Enabled = true;
	btnPause->Enabled = true;

#if 0
	if (MediaPlayer->Mode == mpStopped)
	{
		tmrRefreshPlayerPosition->Enabled = false;
		TrackBar->Visible = false;
		MediaPlayer->Close();

		if (lvRecords->Selected)
		{
			int id = lvRecords->Selected->Index;
			AnsiString filename = records_filtered[id].asFilename;
			MediaPlayer->FileName = filename;
			if (FileExists(filename))
			{
				MediaPlayer->EnabledButtons = TButtonSet() << btPlay;
			}
			else
			{
				MediaPlayer->EnabledButtons = TButtonSet();
			}
		}
		else
		{
			MediaPlayer->EnabledButtons = TButtonSet();
		}
	}
	else if (MediaPlayer->Mode == mpPlaying)
	{
		TrackBar->Position = MediaPlayer->Position;
	}
#endif	
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::TrackBarMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (player.IsPlaying())
		player.Pause(true);
}

void __fastcall TfrmRecordings::TrackBarMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
	player.SeekToPcmPosition(static_cast<int64_t>(TrackBar->Position) * player.GetSampleRate() / TRACKBAR_TICKS_PER_SECOND);
	player.Pause(false);
}

void __fastcall TfrmRecordings::trbarVolumeChange(TObject *Sender)
{
	DWORD volval = trbarVolume->Position * (-655);
	volval |= (volval << 16);
	waveOutSetVolume(0, volval);
}
//---------------------------------------------------------------------------

void TfrmRecordings::RefreshList(void)
{
	records.clear();
	Filter();
	AnsiString dir = ExtractFileDir(Application->ExeName) + "\\recordings\\";
	AddDirectory(dir);

	Filter();
}

void TfrmRecordings::AddDirectory(AnsiString dir)
{
	if (dir[dir.Length()] != '\\')
		dir += "\\";
	WIN32_FIND_DATA file;
	AnsiString asSrchPath = dir + "*.*";
	HANDLE hFind = FindFirstFile(asSrchPath.c_str(), &file);
	int hasfiles = (hFind != INVALID_HANDLE_VALUE);

	while (hasfiles)
	{
		AnsiString ext = ExtractFileExt(file.cFileName);
		if (ext == ".wav" || ext == ".ogg")
		{
			S_RECORD record;
			record.asFilename = dir + file.cFileName;
			AnsiString asFile = file.cFileName;
			//20150228_212954_0_KjcwMQ==.wav
			int pos = asFile.Pos(".");
			if (pos > 0) {
				asFile = asFile.SubString(1, pos-1);
			}
			AnsiString asDate, asTime;
			pos = asFile.Pos("_");
			if (pos > 0) {
				asDate = asFile.SubString(1, pos-1);
			}
			if (pos < asFile.Length()) {
				asFile = asFile.SubString(pos+1, asFile.Length() - pos);
			}

			pos = asFile.Pos("_");
			if (pos > 0) {
				asTime = asFile.SubString(1, pos-1);
			}
			if (pos < asFile.Length()) {
				asFile = asFile.SubString(pos+1, asFile.Length() - pos);
			}

			if (asDate.Length() == 8 && asTime.Length() == 6) {
				const char *ptrDate = asDate.c_str();
				const char *ptrTime = asTime.c_str();
				record.asDateTime.sprintf("%.4s-%.2s-%.2s %.2s:%.2s:%.2s", ptrDate, ptrDate+4, ptrDate+6, ptrTime, ptrTime+2, ptrTime+4);
			}

			AnsiString asDir;
			pos = asFile.Pos("_");
			if (pos > 0) {
				asDir = asFile.SubString(1, pos-1);
				if (asDir == "0") {
					record.dir = S_RECORD::DIR_IN;
				} else if (asDir == "1") {
					record.dir = S_RECORD::DIR_OUT;
				}
			}
			if (pos < asFile.Length()) {
				asFile = asFile.SubString(pos+1, asFile.Length() - pos);
			}

			AnsiString asNumber;
			pos = asFile.Pos("_");
			if (pos > 0) {
				asNumber = asFile.SubString(1, pos-1);
			} else {
				asNumber = asFile;
			}
			record.asNumber = base64_decode(asNumber.c_str(), BASE64_ALPHABET_FSAFE).c_str();

			record.size = file.nFileSizeLow;

			Contacts::Entry* contact = contacts.GetEntry(record.asNumber);
			if (contact != NULL)
			{
				record.asDescription = contact->description;
			}

			records.push_back(record);
		}
		hasfiles = FindNextFile(hFind, &file);
	}
	FindClose(hFind);

}

void __fastcall TfrmRecordings::btnRefreshRecordListClick(TObject *Sender)
{
	RefreshList();
}
//---------------------------------------------------------------------------


void __fastcall TfrmRecordings::tmrStartupTimer(TObject *Sender)
{
	tmrStartup->Enabled = false;
	lvRecordsColumnClick(NULL, lvRecords->Columns->Items[1]);
	RefreshList();
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::lvRecordsDblClick(TObject *Sender)
{
	StopPlaySelected();
}
//---------------------------------------------------------------------------

void TfrmRecordings::StopPlaySelected(void)
{
	if (player.IsPlaying())
		player.Stop();
	Play();
}

void __fastcall TfrmRecordings::miCopyNumberClick(TObject *Sender)
{
	if (!lvRecords->Selected)
		return;
	int id = lvRecords->Selected->Index;
	AnsiString number = records_filtered[id].asNumber;
	Clipboard()->SetTextBuf(number.c_str());
}
//---------------------------------------------------------------------------

void TfrmRecordings::Play(void)
{
	if (lvRecords->Selected)
	{
		AnsiString filename = records_filtered[lvRecords->Selected->Index].asFilename;
		player.Play(filename, appSettings.Audio.outputDevice);
		TrackBar->Visible = true;
		tmrRefreshPlayerPosition->Enabled = true;
	}
}

void __fastcall TfrmRecordings::miDeleteFilesClick(TObject *Sender)
{
	int count = lvRecords->SelCount;
	AnsiString msg;
	if (count > 1)
	{
		msg.sprintf("Delete %d selected records?", count);
	}
	else if (count == 1)
	{
		msg.sprintf("Delete selected record?");
	}
	else
	{
		return;
	}
	if (MessageBox(this->Handle, msg.c_str(),
		this->Caption.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) != IDYES)
	{
		return;
	}
	for (int i=lvRecords->Items->Count-1; i>=0; i--)
	{
		if (lvRecords->Items->Item[i]->Selected)
		{
			int id = records_filtered[i].id;
			DeleteFile(records_filtered[i].asFilename);
			records.erase(records.begin() + id);
        }
	}
	Filter();
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::FormCreate(TObject *Sender)
{
	AnsiString asContactsFile;
	// try guessing contacts file name (main application may be branded if not present in configuration
	if (appSettings.Contacts.fileName != "")
	{
		asContactsFile.sprintf("%s\\%s", ExtractFileDir(Application->ExeName).c_str(),
			appSettings.Contacts.fileName.c_str());
    }
	else
	{
        AnsiString dir = ExtractFileDir(Application->ExeName);
		if (dir[dir.Length()] != '\\')
			dir += "\\";
		WIN32_FIND_DATA file;
		AnsiString asSrchPath = dir + "*_contacts.json";
		HANDLE hFind = FindFirstFile(asSrchPath.c_str(), &file);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			asContactsFile = dir + file.cFileName;
		}
		FindClose(hFind);
	}
	if (asContactsFile != "")
	{
		contacts.SetFilename(asContactsFile);
		contacts.Read();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::lvRecordsKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_RETURN)
	{
    	StopPlaySelected();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::lvRecordsKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (Key == VK_LEFT)
	{
		if (player.IsPlaying())
		{
			int64_t pos = player.GetPcmPosition();
			pos -= player.GetSampleRate() * 5;
			if (pos < 0)
				pos = 0;
			player.SeekToPcmPosition(pos);
		}
	}
	else if (Key == VK_RIGHT)
	{
		if (player.IsPlaying())
		{
			int64_t pos = player.GetPcmPosition();
			pos += player.GetSampleRate() * 5;
			if (pos < player.GetTotalPcmSamples())
				player.SeekToPcmPosition(pos);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::btnStopClick(TObject *Sender)
{
	player.Stop();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::btnPauseClick(TObject *Sender)
{
	if (player.IsPlaying())
	{
		int TODO__PAUSE_BUTTON_DOWN;
		player.Pause(!player.IsPaused());
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::btnPlayClick(TObject *Sender)
{
	Play();
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::miOpenFileInDefaultPlayerClick(TObject *Sender)
{
	if (lvRecords->Selected)
	{
		AnsiString filename = records_filtered[lvRecords->Selected->Index].asFilename;
		ShellExecute(NULL, "open", filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	player.Stop();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::miTranscribeFileClick(TObject *Sender)
{
	if (lvRecords->Selected)
	{
		AnsiString filename = records_filtered[lvRecords->Selected->Index].asFilename;
		int TODO__PREVENT_MULTIPLE_RUN_SAME_TIME;

		AnsiString relPath;

		AnsiString whisperExe = appSettings.Transcription.whisperExe;
		relPath = ExtractFileDir(Application->ExeName) + "\\" + whisperExe;
		if (FileExists(relPath))
			whisperExe = relPath;

		AnsiString model = appSettings.Transcription.model;
		relPath = ExtractFileDir(Application->ExeName) + "\\" + model;
		if (FileExists(relPath))
			model = relPath;

		transcription.Transcribe(filename, whisperExe, model,
			appSettings.Transcription.language, appSettings.Transcription.threadCount);
	}
}
//---------------------------------------------------------------------------

