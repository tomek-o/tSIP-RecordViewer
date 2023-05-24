//---------------------------------------------------------------------------

#ifndef RecordH
#define RecordH
//---------------------------------------------------------------------------

#include <System.hpp>

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
		id(-1),
		dir(DIR_UNKNOWN),
		size(0)
	{}
	bool hasTranscription(void) const;
};

#endif
