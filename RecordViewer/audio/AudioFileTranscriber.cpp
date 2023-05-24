//---------------------------------------------------------------------------


#pragma hdrstop

#include "AudioFileTranscriber.h"
#include "Log.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{

int Execute(AnsiString cmd, HANDLE &hProcess)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	hProcess = NULL;

	// Start the child process.
	if( !CreateProcess( NULL, // No module name (use command line)
		cmd.c_str(), // Command line
		NULL, // Process handle not inheritable
		NULL, // Thread handle not inheritable
		FALSE, // Set handle inheritance to FALSE
		BELOW_NORMAL_PRIORITY_CLASS, //priority,
		NULL, // Use parent's environment block
		NULL, // Use parent's starting directory
		&si, // Pointer to STARTUPINFO structure
		&pi ) // Pointer to PROCESS_INFORMATION structure
		)
	{
		LOG( "CreateProcess failed (%d)", GetLastError() );
		return -1;
	}

    hProcess = pi.hProcess;

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	DWORD exitCode;
	BOOL result = GetExitCodeProcess(pi.hProcess, &exitCode);

	if (result == false)
	{
		LOG("Failed to get exit code");
		exitCode = -2;
    }

	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return exitCode;
}

}	// namespace


int AudioFileTranscriber::Transcribe(AnsiString sourceFile, AnsiString whisperExe, AnsiString model, AnsiString language, unsigned int threadCount)
{
	// Example: main.exe -oj -l pl -m ../../models/ggml-model-whisper-base.bin  -f ../../samples/test_pl.wav
	AnsiString cmd;

	AnsiString languageParam;
	if (language != "")
		languageParam.sprintf(" -l %s", language.c_str());

	AnsiString modelParam;
	if (model != "")
		modelParam.sprintf(" -m \"%s\"", model.c_str());

	cmd.sprintf("\"%s\" --output-json --threads %u %s %s -f \"%s\"",
		whisperExe.c_str(),
		threadCount,
		languageParam.c_str(),
		modelParam.c_str(),
		sourceFile.c_str()
		);

	LOG("Transcribe cmd = %s", cmd.c_str());

	int ret = Execute(cmd, hProcess);

	if (ret == 0)
	{
#if 0
		if (FileExists(tmpFile))
		{
			std::auto_ptr<TStrings> strings(new TStringList());
			try
			{
				strings->LoadFromFile(tmpFile);
				response = strings->Text;
				if (response.Length() == 0)
				{
					LOG("Failed to fetch %s from %s, password mismatch?", cgi.c_str(), devInfo.getTarget().c_str());
					ret = -5;
				}
			}
			catch(...)
			{
				LOG("Failed to load data from file");
				DeleteFile(tmpFile);
				return -2;
			}
			DeleteFile(tmpFile);
		}
		else
		{
			return -3;
		}
#endif
	}
	else
	{
		LOG("Failed transcribe %s", sourceFile.c_str());
	}

	return ret;
}

void AudioFileTranscriber::Terminate(void)
{
    terminated = true;
	if (hProcess != NULL)
	{
		TerminateProcess(hProcess, -1);
	}
}

