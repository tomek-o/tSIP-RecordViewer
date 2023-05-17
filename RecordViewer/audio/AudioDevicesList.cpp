//---------------------------------------------------------------------------
#pragma hdrstop

#include "AudioDevicesList.h"
#include <mmsystem.h>

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)

AudioDevicesList::AudioDevicesList(void)
{

}

void AudioDevicesList::Refresh(void)
{
	winwaveDevsOut.clear();
	winwaveDevsOut.push_back("WAVE mapper (default device)");

	WAVEOUTCAPS woc;
	int nOutDevices = waveOutGetNumDevs();
	for (int i=0; i<nOutDevices; i++)
	{
		if (waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS))==MMSYSERR_NOERROR)
		{
			winwaveDevsOut.push_back(woc.szPname);
		}
	}
}

void AudioDevicesList::FillComboBox(Stdctrls::TComboBox *target, AnsiString selected)
{
    target->Tag = 0;
	target->Items->Clear();
	std::vector<AnsiString> *v = NULL;
	v = &AudioDevicesList::Instance().winwaveDevsOut;
	assert(v);
	for (unsigned int i=0; i<v->size(); i++)
	{
		AnsiString dev = v->at(i);
		target->Items->Add(dev);
		if (dev == selected)
		{
			target->ItemIndex = i;
		}
	}
	// convention: if selected device is not found - add it at last item with [NOT FOUND] text and set non-zero Tag
	if (target->ItemIndex < 0 && target->Items->Count > 0 && selected != "")
	{
		AnsiString text;
		text.sprintf("[NOT FOUND] %s", selected.c_str());
		target->Items->Add(text);
		target->ItemIndex = target->Items->Count - 1;
		target->Tag = 1;
	}
}
