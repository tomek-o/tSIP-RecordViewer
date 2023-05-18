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
- playback for both .wav and .ogg files
- settings: output audio device selection


TODO:
	- settings: switch from ini to JSON
	- whisper.cpp already has -di / --diarize option
*/
