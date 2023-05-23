#ifndef FormTranscriptionH
#define FormTranscriptionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>

#include "AudioFileChannel.h"
#include <set>
//---------------------------------------------------------------------------

struct S_RECORD;
struct Transcription;

class TfrmTranscription : public TForm
{
__published:	// IDE-managed Components
	TRichEdit *memoMain;
	TPanel *pnlTop;
	TLabel *lblFileName;
	TEdit *edFile;
    void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
	AnsiString target;
	int LoadTranscription(const struct S_RECORD &record, AnsiString otherParty);
	void AddSentence(const Transcription &tr, enum AudioFileChannel channel, AnsiString otherParty);
public:		// User declarations
	__fastcall TfrmTranscription(TComponent* Owner, const struct S_RECORD &record);
	void __fastcall MyWndProc (Messages::TMessage &Msg);
	void SetTarget(AnsiString target);
	void UpdateTarget(AnsiString val);

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmTranscription *frmTranscription;
//---------------------------------------------------------------------------
#endif

