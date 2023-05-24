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
- TMediaPlayer replaced with own playback function
- playback for both .wav and .ogg files
- settings: output audio device selection


TODO:
	- start transcription for all files with missing transcription
	- full text search for all transcriptions: Tools / Browse transcriptions
	- rewind audio to matching position when playing and sentence was clicked
	- test space in whisper directories (model file, source file)
	- optimize for large number of recordings
*/
