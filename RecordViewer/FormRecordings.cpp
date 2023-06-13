//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormRecordings.h"
#include "Contacts.h"
#include "Settings.h"
#include "AudioFileTranscription.h"
#include "FormTranscription.h"
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

AnsiString GetWhisperExe(void)
{
	AnsiString whisperExe = appSettings.transcription.whisperExe;
	AnsiString relPath = ExtractFileDir(Application->ExeName) + "\\" + whisperExe;
	if (FileExists(relPath))
		whisperExe = relPath;
	return whisperExe;
}

AnsiString GetModel(void)
{
	AnsiString model = appSettings.transcription.model;
	AnsiString relPath = ExtractFileDir(Application->ExeName) + "\\" + model;
	if (FileExists(relPath))
		model = relPath;
	return model;
}

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

	lblTranscriptionState->Caption = "";

	cbTranscriptionFilter->Clear();
	for (int i=0; i<Settings::Recordings::TR_FILTER__LIMITER; i++)
	{
		cbTranscriptionFilter->Items->Add(Settings::Recordings::getTranscriptionFilterName(static_cast<Settings::Recordings::TranscriptionFilter>(i)));
	}
	if (appSettings.recordings.transcriptionFilter >= 0 && appSettings.recordings.transcriptionFilter < cbTranscriptionFilter->Items->Count)
		cbTranscriptionFilter->ItemIndex = appSettings.recordings.transcriptionFilter;
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::lvRecordsData(TObject *Sender, TListItem *Item)
{
	int id = Item->Index;
	S_RECORD &record = records[recordsFilteredIds[id]];
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
	AnsiString transcriptionStatus;
	if (record.hasMonoTranscription)
		transcriptionStatus = "mono";
	else if (record.hasStereoTranscription)
		transcriptionStatus = "stereo";
	Item->SubItems->Add(transcriptionStatus);
	Item->SubItems->Add(record.size);
}
//---------------------------------------------------------------------------

void TfrmRecordings::Filter(void)
{
	if (edFilter->Text == "")
	{
		recordsFilteredIds.resize(records.size());
		for (unsigned int i=0; i<records.size(); i++)
		{
			recordsFilteredIds[i] = i;
		}
	}
	else
	{
		recordsFilteredIds.clear();
		recordsFilteredIds.reserve(records.size());

		AnsiString asFilter = edFilter->Text;
		AnsiString asFilterLowerCase = asFilter.LowerCase();
		AnsiString asFilterUpperCase = asFilter.UpperCase();

		bool filterByMeta =
			(appSettings.recordings.transcriptionFilter == Settings::Recordings::TR_FILTER_META_ONLY) ||
			(appSettings.recordings.transcriptionFilter == Settings::Recordings::TR_FILTER_META_ALL_TRANSCRIPTIONS) ||
			(appSettings.recordings.transcriptionFilter == Settings::Recordings::TR_FILTER_META_LOCAL_TRANSCRIPTIONS) ||
			(appSettings.recordings.transcriptionFilter == Settings::Recordings::TR_FILTER_META_2ND_PARTY_TRANSCRIPTIONS);

		for (unsigned int i=0; i<records.size(); i++)
		{
			S_RECORD &record = records[i];
			bool match = false;

			if (filterByMeta)
			{
				if (record.asDateTime.Pos(asFilter))
				{
					match = true;
				}
				else if (record.asNumber.UpperCase().Pos(asFilterUpperCase))
				{
					match = true;
				}
				else if (record.asDescription.UpperCase().Pos(asFilterUpperCase))
				{
					match = true;
				}
			}

			if (match == false)
			{
				if (appSettings.recordings.transcriptionFilter == Settings::Recordings::TR_FILTER_META_ALL_TRANSCRIPTIONS ||
					appSettings.recordings.transcriptionFilter == Settings::Recordings::TR_FILTER_ALL_TRANSCRIPTIONS)
				{
					if (record.fullTranscriptionTextLMono.Pos(asFilterLowerCase) ||
						record.fullTranscriptionTextR.Pos(asFilterLowerCase))
					{
						match = true;
					}
				}
				else if (appSettings.recordings.transcriptionFilter == Settings::Recordings::TR_FILTER_META_LOCAL_TRANSCRIPTIONS ||
						appSettings.recordings.transcriptionFilter == Settings::Recordings::TR_FILTER_LOCAL_TRANSCRIPTIONS)
				{
					if (record.fullTranscriptionTextLMono.Pos(asFilterLowerCase))
					{
						match = true;
					}
				}
				else if (appSettings.recordings.transcriptionFilter == Settings::Recordings::TR_FILTER_META_2ND_PARTY_TRANSCRIPTIONS ||
						appSettings.recordings.transcriptionFilter == Settings::Recordings::TR_FILTER_2ND_PARTY_TRANSCRIPTIONS)
				{
					if (record.fullTranscriptionTextR.Pos(asFilterLowerCase))
					{
						match = true;
					}
				}
			}

			if (match)
			{
				recordsFilteredIds.push_back(i);
            }
		}
	}

	lvRecords->Items->Count = recordsFilteredIds.size();

	lvRecords->Invalidate();
	lvRecords->Height += 1;
	lvRecords->Height -= 1;

//	lvRecords->Columns->Items[0]->AutoSize = true;
//	lvRecords->Columns->Items[0]->Width -= 20;
//	lvRecords->Columns->Items[0]->AutoSize = false;

	unsigned long long totalSize = 0;
	for (unsigned int i=0; i<recordsFilteredIds.size(); i++) {
		totalSize += records[recordsFilteredIds[i]].size;
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
	const S_RECORD &record = records[recordsFilteredIds[id]];
	AnsiString filename = record.asFilename;
	Clipboard()->SetTextBuf(filename.c_str());
}
//---------------------------------------------------------------------------


bool __fastcall TfrmRecordings::v_compare_records(const S_RECORD& d1, const S_RECORD& d2)
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
		if (forward)
			return d1.asDateTime > d2.asDateTime;
		else
			return d1.asDateTime < d2.asDateTime;
	}
	else if (column == 2)
	{
		if (forward)
			return d1.asNumber > d2.asNumber;
		else
			return d1.asNumber < d2.asNumber;
	}
	else if (column == 3)
	{
		if (forward)
			return d1.asDescription > d2.asDescription;
		else
			return d1.asDescription < d2.asDescription;
	}
	else if (column == 4)
	{
		if (forward)
			return d1.asFilename > d2.asFilename;
		else
			return d1.asFilename < d2.asFilename;
	}
	else if (column == 5)
	{
		AnsiString ext1 = ExtractFileExt(d1.asFilename);
		AnsiString ext2 = ExtractFileExt(d2.asFilename);
		if (forward)
			return ext1 > ext2;
		else
			return ext1 < ext2;
	}
	else if (column == 6)
	{
		enum TranscriptionStatus {
			TR_NONE = 0,
			TR_MONO,
			TR_STEREO
		} tr1, tr2;

		if (d1.hasStereoTranscription)
			tr1 = TR_STEREO;
		else if (d1.hasMonoTranscription)
			tr1 = TR_MONO;
		else
			tr1 = TR_NONE;

		if (d2.hasStereoTranscription)
			tr2 = TR_STEREO;
		else if (d2.hasMonoTranscription)
			tr2 = TR_MONO;
		else
			tr2 = TR_NONE;

		if (forward)
		{
			return tr1 > tr2;
		}
		else
		{
			return tr1 < tr2;
		}
	}
	else if (column == 7)
	{
		if (forward)
			return d1.size > d2.size;
		else
			return d1.size < d2.size;
	}
	else
	{
		return false;
	}
}

void __fastcall TfrmRecordings::lvRecordsColumnClick(TObject *Sender,
      TListColumn *Column)
{
	if (listTranscriptionProcess.active || transcription.IsRunning())
	{
		MessageBox(this->Handle, "Sorting is blocked while transcription is running.",
			this->Caption.c_str(), MB_ICONINFORMATION);
		return;
	}

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

	std::stable_sort(records.begin(), records.end(), v_compare_records);
	Filter();

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
				if (asDir == "1") {
					record.dir = S_RECORD::DIR_IN;
				} else if (asDir == "0") {
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

            record.loadTranscription();

			records.push_back(record);
		}
		hasfiles = FindNextFile(hFind, &file);
	}
	FindClose(hFind);

}

void __fastcall TfrmRecordings::btnRefreshRecordListClick(TObject *Sender)
{
	if (listTranscriptionProcess.active || transcription.IsRunning())
	{
		MessageBox(this->Handle, "Reloading list is blocked while transcription is running.",
			this->Caption.c_str(), MB_ICONINFORMATION);
		return;
	}
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
	const S_RECORD &record = records[recordsFilteredIds[id]];
	AnsiString number = record.asNumber;
	Clipboard()->SetTextBuf(number.c_str());
}
//---------------------------------------------------------------------------

void TfrmRecordings::Play(void)
{
	if (lvRecords->Selected)
	{
		int id = lvRecords->Selected->Index;
		const S_RECORD &record = records[recordsFilteredIds[id]];
		AnsiString filename = record.asFilename;
		player.Play(filename, appSettings.audio.outputDevice);
		TrackBar->Visible = true;
		tmrRefreshPlayerPosition->Enabled = true;
	}
}

void __fastcall TfrmRecordings::miDeleteFilesClick(TObject *Sender)
{
	if (listTranscriptionProcess.active || transcription.IsRunning())
	{
		MessageBox(this->Handle, "Deleting is blocked while transcription is running.",
			this->Caption.c_str(), MB_ICONINFORMATION);
		return;
	}

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
	std::set<int> idsToErase;
	for (int i=lvRecords->Items->Count-1; i>=0; i--)
	{
		if (lvRecords->Items->Item[i]->Selected)
		{
			int id = recordsFilteredIds[i];
			DeleteFile(records[id].asFilename);
			idsToErase.insert(id);
		}
	}
	for (std::set<int>::reverse_iterator iter = idsToErase.rbegin(); iter != idsToErase.rend(); ++iter)
	{
		records.erase(records.begin() + *iter);
	}
	Filter();
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::FormCreate(TObject *Sender)
{
	AnsiString asContactsFile;
	// try guessing contacts file name (main application may be branded if not present in configuration
	if (appSettings.contacts.fileName != "")
	{
		asContactsFile.sprintf("%s\\%s", ExtractFileDir(Application->ExeName).c_str(),
			appSettings.contacts.fileName.c_str());
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
		int id = lvRecords->Selected->Index;
		const S_RECORD &record = records[recordsFilteredIds[id]];
		AnsiString filename = record.asFilename;
		ShellExecute(NULL, "open", filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmRecordings::miTranscribeFileClick(TObject *Sender)
{
	if (CheckWhisper())
		return;

	if (transcription.IsRunning() || listTranscriptionProcess.active)
	{
		MessageBox(this->Handle, "Another transcription is already running",
			this->Caption.c_str(), MB_ICONINFORMATION);
		return;
	}

	if (lvRecords->Selected)
	{
		int id = lvRecords->Selected->Index;
		const S_RECORD &record = records[recordsFilteredIds[id]];
		if (record.hasTranscription())
		{
			if (MessageBox(this->Handle, "Transcription of this file already exist.\nAre you sure you want to run it again?",
				this->Caption.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION) != IDYES)
			{
            	return;
			}
		}
		TranscribeRecord(id, record);
	}
}
//---------------------------------------------------------------------------

void TfrmRecordings::TranscribeRecord(unsigned int recordId, const S_RECORD &record)
{
	AnsiString filename = record.asFilename;

	AnsiString whisperExe = GetWhisperExe();
	AnsiString model = GetModel();

	transcription.Transcribe(recordId, filename, whisperExe, model,
		appSettings.transcription.language, appSettings.transcription.threadCount);
}

int TfrmRecordings::CheckWhisper(void)
{
	AnsiString whisperExe = GetWhisperExe();
	AnsiString model = GetModel();

	if (!FileExists(whisperExe))
	{
		MessageBox(this->Handle, "whisper.cpp executable not found!\nSee settings.", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return -1;
	}

	if (!FileExists(model))
	{
		MessageBox(this->Handle, "whisper.cpp model file not found!\nNote: there are multiple models available, in multiple sizes, English-only or multilanguage.\nSee settings.",
			this->Caption.c_str(), MB_ICONEXCLAMATION);
		return -1;
	}

	return 0;
}


void __fastcall TfrmRecordings::tmrTransciptionTimer(TObject *Sender)
{
	AnsiString text;
	if (transcription.IsRunning())
	{
		text.sprintf("Transcribing %s...", ExtractFileName(transcription.GetFileName()).c_str());
	}
	else
	{
		ListTranscriptionProcess &ltr = listTranscriptionProcess;
		if (ltr.active)
		{
			// checking few files for missing transcription at one timer call to skip already transmitted files
			for (int i=0; i<100; i++)
			{
				if (ltr.listPosition >= ltr.recordIds.size())
				{
					ltr.active = false;
					break;
				}
				else
				{
					unsigned int id = ltr.recordIds[ltr.listPosition++];
					if (id < records.size())
					{
						S_RECORD &record = records[id];
						if (record.hasTranscription() == false)
						{
							TranscribeRecord(id, record);
							break;
						}
					}
				}
			}
		}
	}
	lblTranscriptionState->Caption = text;

	{
		unsigned int recordId = 0xFFFFFFFF;
		if (transcription.GetTranscribedId(recordId) == 0)
		{
			if (recordId < records.size())
			{
				S_RECORD &record = records[recordId];
				record.loadTranscription();
				lvRecords->Invalidate();
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::miShowFileTranscriptionClick(TObject *Sender)
{
	if (lvRecords->Selected)
	{
		int id = lvRecords->Selected->Index;
		const S_RECORD &record = records[recordsFilteredIds[id]];
		TfrmTranscription *frm = new TfrmTranscription(NULL, record);
		frm->Show();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmRecordings::popupRecordsPopup(TObject *Sender)
{
	miShowFileTranscription->Enabled = false;

	if (lvRecords->Selected)
	{
		int id = lvRecords->Selected->Index;
		const S_RECORD &record = records[recordsFilteredIds[id]];
		if (record.hasTranscription())
			miShowFileTranscription->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void TfrmRecordings::GenerateMissingTranscriptionsForFilteredFiles(void)
{
	if (CheckWhisper())
		return;

	if (listTranscriptionProcess.active || transcription.IsRunning())
	{
		MessageBox(this->Handle, "Another transcription is already running.\nStop it first if you want to start new transcription process.",
			this->Caption.c_str(), MB_ICONINFORMATION);
		return;
	}
	ListTranscriptionProcess &ltr = listTranscriptionProcess;
	ltr.recordIds = recordsFilteredIds;
	ltr.listPosition = 0;
	ltr.active = true;
}

void TfrmRecordings::StopTranscribing(void)
{
    listTranscriptionProcess.active = false;
	transcription.Stop();
}

void __fastcall TfrmRecordings::cbTranscriptionFilterChange(TObject *Sender)
{
	appSettings.recordings.transcriptionFilter = static_cast<Settings::Recordings::TranscriptionFilter>(cbTranscriptionFilter->ItemIndex);
	Filter();
}
//---------------------------------------------------------------------------

void TfrmRecordings::Shutdown(void)
{
	player.Stop();
	StopTranscribing();
}
