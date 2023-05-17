//---------------------------------------------------------------------------

#ifndef FormSettingsH
#define FormSettingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "Settings.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

class TfrmSettings : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TButton *btnCancel;
	TButton *btnApply;
	TPageControl *pcGeneral;
	TTabSheet *tsGeneral;
	TTabSheet *pcLogging;
	TCheckBox *chbAlwaysOnTop;
	TLabel *lblUiCapacity;
	TComboBox *cmbMaxUiLogLines;
	TCheckBox *chbLogToFile;
	TTabSheet *tsAudio;
	TLabel *lblAudioOutput;
	TButton *btnRefreshAudioDevicesLists;
	TComboBox *cbAudioOutput;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall btnApplyClick(TObject *Sender);
	void __fastcall chbAlwaysOnTopClick(TObject *Sender);
	void __fastcall cmbMaxUiLogLinesChange(TObject *Sender);
private:	// User declarations
	void RefreshAudioDevicesList(void);
public:		// User declarations
	__fastcall TfrmSettings(TComponent* Owner);
	Settings *appSettings;
	Settings tmpSettings;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
#endif
