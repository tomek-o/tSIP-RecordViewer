//---------------------------------------------------------------------------

#ifndef UtilsH
#define UtilsH
//---------------------------------------------------------------------------
#include <System.hpp>

AnsiString GetFileVer(AnsiString FileName);
AnsiString ExtractNumberFromUri(AnsiString uri);
AnsiString CleanNumber(AnsiString asNumber);
AnsiString GetFileSystemSafeText(AnsiString text);

#endif
