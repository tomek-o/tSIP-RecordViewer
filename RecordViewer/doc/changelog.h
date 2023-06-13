/** \file
	\brief Changelog
*/

/** \page changelog Changelog

Version 0.0.1
- initial release

Version 0.1
- double click on record starts playing
- list context menu: delete selected records
- file name is hidden by default from list (0 width column)
- reading and displaying contact descriptions from tSIP contacts file; by default
uses first found *_contacts.json file in application directory (RecordViewer should be extracted
to same directory as tSIP), can be overriden by editing ini file
- sorting list by default starting from newest items

Version 0.2
- FIXED: if list was double clicked when playing previous, not newly selected file was played
- [Enter] on the file list works same as double clicking (starts or restarts playing)
- [Left] and [Right] on the file list or trackbar while playing are moving position by 5 seconds
- changed settings and "About" windows position to main window center

Version 0.2.1
- partial/minimal support for newly added Opus/OGG recording in tSIP:
	- listing also .ogg files
	- .ogg files are opened for playback using default application when double clicked or "play" is pressed

Version 0.3
- record transcription using whisper.cpp, version ~1.4
	- slow but CPU only speech to text, starting with Sandy Bridge i3/i5/i7 (AVX instruction set) or Haswell (AVX2 instruction set version)
	- local, offline, free (minus the power consumption)
	- English-only and multilanguage models in various sizes: 75 MB, 142 MB, 466 MB, 1.5 GB, 2.9 GB on disk, similar RAM usage
	- diarization for stereo recordings with separate processing for each audio channel and merging sentences back when displaying
	- works with all tSIP file formats: mono/stereo PCM wave and Opus OGG files
	- transcription text can be used to filter recordings
- TMediaPlayer replaced with own playback function
- playback for both .wav and .ogg files
- settings: output audio device selection
- settings format changed to JSON

Version 0.3.1
- when transcribing stereo file and transcription of one of the channels already exists, it is skipped
- added transcription status (none/mono/stereo) to recoding list
- added log showing transcription time at the end of transcribing L/R/mono file
- added min width/height constraints for main window 
- update _dist with correct AVX-only version of whisper.cpp (though binaries in zip were already corrected in 0.3)
- fixed filtering of metadata containing non-ANSI characters (UpperCase() vs Ansi::UpperCase())
- added options to filter by transcription text only (not by metadata)


TODO:
	- start transcription for multiple selected files
	- Tools / Browse transcriptions?
	- show transcription in part of the window if recording is selected?
	- rewind audio to matching position when playing and sentence was clicked
	- optimize for large number of recordings
	- fix some weird issue with playback stopping? not reproduced?
*/
