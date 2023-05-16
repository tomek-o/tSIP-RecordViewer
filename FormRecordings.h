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
	TMediaPlayer *MediaPlayer;
	TTimer *tmrRefreshPlayerPosition;
	TLabel *lblItemsSize;
	TTrackBar *trbarVolume;
	TButton *btnRefreshRecordList;
	TTimer *tmrStartup;
	TMenuItem *miCopyNumber;
	TMenuItem *miDeleteFiles;
	void __fastcall miPopupRecordsCopyClick(TObject *Sender);
	void __fastcall lvRecordsData(TObject *Sender, TListItem *Item);
	void __fastcall lvRecordsColumnClick(TObject *Sender, TListColumn *Column);
	void __fastcall edFilterChange(TObject *Sender);
	void __fastcall MediaPlayerClick(TObject *Sender, TMPBtnType Button,
          bool &DoDefault);
	void __fastcall MediaPlayerNotify(TObject *Sender);
	void __fastcall tmrRefreshPlayerPositionTimer(TObject *Sender);
	void __fastcall lvRecordsSelectItem(TObject *Sender, TListItem *Item,
          bool Selected);
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
private:	// User declarations
	void __fastcall TrackBarMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TrackBarMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);

	struct S_RECORD
	{
        int id;		///< important for records_filtered only; mapping records_filtered to records
		AnsiString asFilename;
		enum DIR {
			DIR_UNKNOWN = 0,
			DIR_IN,
			DIR_OUT
		} dir;
		AnsiString asDateTime;
		AnsiString asNumber;
		AnsiString asDescription;
		int size;
		S_RECORD(void):
			dir(DIR_UNKNOWN),
			size(0)
		{}
	};

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
	void Reselect(void);
	void Play(void);
	void StopPlaySelected(void);

public:		// User declarations
	__fastcall TfrmRecordings(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmRecordings *frmRecordings;
//---------------------------------------------------------------------------
#endif
