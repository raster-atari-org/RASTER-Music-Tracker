#include "StdAfx.h"
#include "Global.h"
#include "PokeyRederer.h"
#include "RmtMidi.h"
#include "Clipboard.h"
#include "Tuning.h"
#include "Memory.h"
#include "TracksControl.h"

#include "Song.h"
#include "Undo.h"

CAtari g_Atari;
CAtariTrackerDriver* g_AtariTrackerDriver;


BOOL g_closeApplication = FALSE;			// Set when the application is busy shutting down
CDC* g_mem_dc = NULL;
CDC* g_gfx_dc = NULL;

int g_width = 0;
int g_height = 0;
int g_tracklines = 8;
int g_scaling_percentage = 100;


int g_notesperoctave = 12;	// by default there are 12 notes per octave TODO: Today missing in CSong


TTuningSettings g_tuning;

TTuningRatios g_tuningRatios;

HWND g_hwnd = NULL;
HWND g_viewhwnd = NULL;

BOOL g_changes = FALSE;	//have there been any changes in the module?

int g_RmtHasFocus;			// Track if RMT has focus, when it does not have focus and is not in prove mode, the MIDI input will be ignored (to avoid overwriting patterns accidentally)
BOOL g_shiftkey;
BOOL g_controlkey;
BOOL g_altkey;	//unfinished implementation, doesn't work yet for some reason

int g_tracks4_8 = 8; // TODO Move out // Had to be hardcoded to 8 to prevent "ReInitSound()" to run before RMT finished being initialised, otherwise it would crash lol


BOOL volatile g_screenupdate = 0;
BOOL volatile g_rmtroutine; // => TODO: PokeyRenderer?

int volatile g_prove;			// Test notes without editing (0 = off, 1 = mono jam, 2 = stereo jam)
int volatile g_respectvolume;	//does not change the volume if it is already there

WORD g_rmtstripped_adr_module;	//address for export RMT stripped file
BOOL g_rmtstripped_sfx;			//sfx offshoot RMT stripped file
BOOL g_rmtstripped_gvf;			//gvs GlobalVolumeFade for feat
BOOL g_rmtstripped_nos;			//nos NoStartingSongline for feat



Part last_activepart;		//if equal to g_activepart, no block clear necessary
Part last_active_ti;			//if equal to g_active_ti, no screen clear necessary
uint64_t last_ms = 0;
uint64_t last_sec = 0;
int real_fps = 0;
double last_fps = 0;
double avg_fps[120] = { 0 };

Part g_activepart;			// 0 info, 1 edittracks, 2 editinstruments, 3 song
Part g_active_ti;			// 1 tracks, 2 instrs

BOOL g_isEditingInstrumentName;		//0 no, 1 instrument name is edited
BOOL is_editing_infos;		//0 no, 1 song name is edited

int g_line_y = 0;			//active line coordinate, used to reference g_cursoractview to the correct position

int g_trackLinePrimaryHighlight = 8;	// Primary line highlighted every x lines
int g_trackLineSecondaryHighlight = 4;	// Secondary line highlighted every x lines
BOOL g_tracklinealtnumbering = 0;		// Alternative way of line numbering in tracks
int g_linesafter;						// Number of lines to scroll after inserting a note (initializes in CSong :: Clear)

BOOL g_nohwsoundbuffer = 0;	//Don't use hardware soundbuffer
int g_cursoractview = 0;		//default position, line 0

BOOL g_displayflatnotes = 0;	//flats instead of sharps
BOOL g_usegermannotation = 0;	//H notes instead of B

int g_channelon[SONGTRACKS];
int g_rmtinstr[SONGTRACKS];

TrackerDriverVersion g_trackerDriverVersion = PATCH16;
int g_timerGlobalCount = 0;	// Initialised once, ticking forever
long g_playtime = 0;	// Number of Atari frames played since the song started playing


TViewState g_view = {};

// TODO: Why redundant to g_mouse?
UINT g_mousebutt = 0;			//mouse button

// Mouse Information
struct TMouseInfomation g_mouse = { };


int g_lastKeyPressed = 0;		    // For debugging vk input

CString g_prgpath;					// Path to the directory from which the program was started (including a slash at the end)

void SetProgramFolderPath(const CString& folderPath) {
    g_prgpath = folderPath;
}

CString GetResourceFolderPath(const CString& folderName) {
    std::filesystem::path path;
    return path.append(g_prgpath.GetString()).append(folderName.GetString()).c_str();;

}

CString GetResourceFilePath(const std::filesystem::path& relativeFolderPath, const CString& fileName) {
    std::filesystem::path path;
    path.append(g_prgpath.GetString());
    path.append(relativeFolderPath.c_str());
    path.append(fileName.GetString());
    return path.c_str();
}


CString g_lastLoadPath_Songs;		// Path of the last song loaded
CString g_lastLoadPath_Instruments; //the path of the last instrument loaded
CString g_lastLoadPath_Tracks;		//the path of the last track loaded

CString g_defaultSongsPath;				// Default path for songs
CString g_defaultInstrumentsPath;		// Default path for instruments
CString g_defaultTracksPath;			// Default path for tracks

KeyboardLayout g_keyboard_layout = KeyboardLayout::AZERTY;	// Keyboard layout is used by RMT. eg: QWERTY, AZERTY, etc
BOOL g_keyboard_swapenter = 0;	//1 yes, 0 no, probably not needed anymore but will be kept for now
BOOL g_keyboard_playautofollow = 1;	//1 yes, 0 no
BOOL g_keyboard_updowncontinue = 1;	//1 yes, 0 no
BOOL g_keyboard_RememberOctavesAndVolumes = 1;	// 1 yes, 0 no, the last used octave and volume are stored in the instrument data
BOOL g_keyboard_escresetatarisound = 1;	//1 yes, 0 no
BOOL g_keyboard_askwhencontrol_s = 1;	//1 yes, 0 no

// ----------------------------------------------------------------------------
// Here are the main global objects that make up 99% of RMT.
//
CSong			g_Song;				// There is one active song
CRmtMidi		g_Midi;				// There is one midi interface
CUndo			g_Undo;				// Undo buffer tracker
CXPokey			g_Pokey;			// The simulated Pokey chip
CInstruments	g_Instruments;
CTracks			g_Tracks;
CTracksControl  g_TracksControl;
CTrackClipboard g_TrackClipboard;
CTuning			g_Tuning;			// Tuning calculations and POKEY tuning lookup tables generation
