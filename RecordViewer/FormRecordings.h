//---------------------------------------------------------------------------

#ifndef FormRecordingsH
#define FormRecordingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
#include <MPlayer.hpp>
#include <vector>

#include "AudioFilePlayer.h"
#include "Record.h"

//---------------------------------------------------------------------------
class TfrmRecordings : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TLabel *lblItems;
	TLabel *lblItemsCount;
	TListView *lvRecords;
	TPopupMenu *popupRecords;
	TMenuItem *miPopupRecordsCopy;
	TLabel *lblFilter;
	TEdit *edFilter;
	TImageList *lvImages;
	TPanel *pnlPlayer;
	TTrackBar *TrackBar;
	TPanel *pnlPlayerControl;
	TTimer *tmrRefreshPlayerPosition;
	TLabel *lblItemsSize;
	TTrackBar *trbarVolume;
	TButton *btnRefreshRecordList;
	TTimer *tmrStartup;
	TMenuItem *miCopyNumber;
	TMenuItem *miDeleteFiles;
	TButton *btnPlay;
	TButton *btnStop;
	TButton *btnPause;
	TMenuItem *miOpenFileInDefaultPlayer;
	TMenuItem *miTranscribeFile;
	TTimer *tmrShowTransciption;
	TLabel *lblTranscriptionState;
	TMenuItem *miShowFileTranscription;
	void __fastcall miPopupRecordsCopyClick(TObject *Sender);
	void __fastcall lvRecordsData(TObject *Sender, TListItem *Item);
	void __fastcall lvRecordsColumnClick(TObject *Sender, TListColumn *Column);
	void __fastcall edFilterChange(TObject *Sender);
	void __fastcall tmrRefreshPlayerPositionTimer(TObject *Sender);
	void __fastcall trbarVolumeChange(TObject *Sender);
	void __fastcall btnRefreshRecordListClick(TObject *Sender);
	void __fastcall tmrStartupTimer(TObject *Sender);
	void __fastcall lvRecordsDblClick(TObject *Sender);
	void __fastcall miCopyNumberClick(TObject *Sender);
	void __fastcall miDeleteFilesClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall lvRecordsKeyPress(TObject *Sender, char &Key);
	void __fastcall lvRecordsKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall btnPauseClick(TObject *Sender);
	void __fastcall btnPlayClick(TObject *Sender);
	void __fastcall miOpenFileInDefaultPlayerClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall miTranscribeFileClick(TObject *Sender);
	void __fastcall tmrShowTransciptionTimer(TObject *Sender);
	void __fastcall miShowFileTranscriptionClick(TObject *Sender);
	void __fastcall popupRecordsPopup(TObject *Sender);
private:	// User declarations
	void __fastcall TrackBarMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TrackBarMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);

    AudioFilePlayer player;

	std::vector<S_RECORD> records;
	std::vector<S_RECORD> records_filtered;
	void Filter(void);
	struct Sorting {
		int column;
		bool forward;
	} sorting;
	static bool __fastcall v_compare_records(const S_RECORD& d1, const S_RECORD& d2);

	void RefreshList(void);
	void AddDirectory(AnsiString dir);
	void Play(void);
	void StopPlaySelected(void);

public:		// User declarations
	__fastcall TfrmRecordings(TComponent* Owner);
	void StopTranscribing(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmRecordings *frmRecordings;
//---------------------------------------------------------------------------
#endif
