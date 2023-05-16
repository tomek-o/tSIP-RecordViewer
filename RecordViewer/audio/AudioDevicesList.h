//---------------------------------------------------------------------------

#ifndef AudioDevicesListH
#define AudioDevicesListH
//---------------------------------------------------------------------------

#include <System.hpp>
#include <vector>

namespace Stdctrls
{
	class TComboBox;
}

class AudioDevicesList
{
private:
	AudioDevicesList(void);
	AudioDevicesList(const AudioDevicesList& source);
	AudioDevicesList& operator=(const AudioDevicesList&);
public:
	static AudioDevicesList& Instance(void)
	{
		static AudioDevicesList audioDevicesList;
		return audioDevicesList;
	}
	std::vector<AnsiString> winwaveDevsOut;
	void Refresh(void);

	static void FillComboBox(Stdctrls::TComboBox *target, AnsiString selected);
};

#endif

