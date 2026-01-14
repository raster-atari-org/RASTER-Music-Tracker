

// TODO: Replace the plugin interface with a permanent emulation core

#include "StdAfx.h"
#include "Pokey.h"
#include "Atari.h"

extern HWND g_hwnd;

APokeySound_Initialize_PROC APokeySound_Initialize;
APokeySound_PutByte_PROC APokeySound_PutByte;
APokeySound_GetRandom_PROC APokeySound_GetRandom;	// Unused?
APokeySound_Generate_PROC APokeySound_Generate;
APokeySound_About_PROC APokeySound_About;


Pokey_Initialise_PROC Pokey_Initialise;
Pokey_SoundInit_PROC Pokey_SoundInit;
Pokey_Process_PROC Pokey_Process;
Pokey_GetByte_PROC Pokey_GetByte;	// Unused?
Pokey_PutByte_PROC Pokey_PutByte;
Pokey_About_PROC Pokey_About;

CPokey::CPokey()
{
    m_soundDriver = NONE;
    m_pokey_dll = NULL;

    m_initialized = false;
    m_ntsc = false;
    m_stereo = false;
}

CPokey::~CPokey()
{
    DeInitSound();
}


void CPokey::InitSound() {
    m_soundDriver = InitPokeyDll();
}

void CPokey::DeInitSound() {
    m_soundDriver = NONE;
    m_about = "No POKEY emulation loaded";

    m_initialized = false;
    m_ntsc = false;
    m_stereo = false;

    DeInitPokeyDll();
}

CString CPokey::GetAbout() const {
    return m_about;
}

//TODO: Add a method for letting the user chose which plugin they would like to use instead of the current default/fallback setup
CPokey::SoundDriver CPokey::InitPokeyDll()
{

    m_about = "";

    // apokeysnd.dll is first loaded, will be used in priority if it is found
    if (m_pokey_dll = LoadLibrary("apokeysnd.dll"))
    {
        CString warningMessage = "";

        APokeySound_Initialize = (APokeySound_Initialize_PROC)GetProcAddress(m_pokey_dll, "APokeySound_Initialize");
        if (!APokeySound_Initialize) warningMessage += "APokeySound_Initialize\n";

        APokeySound_PutByte = (APokeySound_PutByte_PROC)GetProcAddress(m_pokey_dll, "APokeySound_PutByte");
        if (!APokeySound_PutByte) warningMessage += "APokeySound_PutByte\n";

        APokeySound_GetRandom = (APokeySound_GetRandom_PROC)GetProcAddress(m_pokey_dll, "APokeySound_GetRandom");
        if (!APokeySound_GetRandom) warningMessage += "APokeySound_GetRandom\n";

        APokeySound_Generate = (APokeySound_Generate_PROC)GetProcAddress(m_pokey_dll, "APokeySound_Generate");
        if (!APokeySound_Generate) warningMessage += "APokeySound_Generate\n";

        APokeySound_About = (APokeySound_About_PROC)GetProcAddress(m_pokey_dll, "APokeySound_About");
        if (!APokeySound_About) warningMessage += "APokeySound_About\n";

        // Get "About" data from apokeysnd driver, then finalise the inisialisation
        if (warningMessage.IsEmpty())
        {
            const char* name, * author, * description;
            APokeySound_About(&name, &author, &description);
            m_about.Format("%s\n%s\n%s", name, author, description);
            return APOKEYSND;
        }

        // If an error is caught, the plugin will be unloaded with an error message showing the problematic procedures
        MessageBox(g_hwnd, "Error:\nNo compatible 'apokeysnd.dll',\ntherefore the Pokey sound can't be performed.\nIncompatibility with:" + warningMessage, "Pokey library error", MB_ICONEXCLAMATION);
        DeInitPokeyDll();
    }

    // sa_pokey.dll will be loaded next if apokeysnd.dll was not found or had an error, as a fallback
    if (m_pokey_dll = LoadLibrary("sa_pokey.dll"))
    {
        CString warningMessage = "";

        Pokey_Initialise = (Pokey_Initialise_PROC)GetProcAddress(m_pokey_dll, "Pokey_Initialise");
        if (!Pokey_Initialise) warningMessage += "Pokey_Initialise\n";

        Pokey_SoundInit = (Pokey_SoundInit_PROC)GetProcAddress(m_pokey_dll, "Pokey_SoundInit");
        if (!Pokey_SoundInit) warningMessage += "Pokey_SoundInit\n";

        Pokey_Process = (Pokey_Process_PROC)GetProcAddress(m_pokey_dll, "Pokey_Process");
        if (!Pokey_Process) warningMessage += "Pokey_Process\n";

        Pokey_GetByte = (Pokey_GetByte_PROC)GetProcAddress(m_pokey_dll, "Pokey_GetByte");
        if (!Pokey_GetByte) warningMessage += "Pokey_GetByte\n";

        Pokey_PutByte = (Pokey_PutByte_PROC)GetProcAddress(m_pokey_dll, "Pokey_PutByte");
        if (!Pokey_PutByte) warningMessage += "Pokey_PutByte\n";

        Pokey_About = (Pokey_About_PROC)GetProcAddress(m_pokey_dll, "Pokey_About");
        if (!Pokey_About) warningMessage += "Pokey_About\n";

        // Get "About" data from sa_pokey driver, then finalise the inisialisation
        if (warningMessage.IsEmpty())
        {
            char* name, * author, * description;
            Pokey_About(&name, &author, &description);
            m_about.Format("%s\n%s\n%s", name, author, description);
            Pokey_Initialise(0, 0);
            return SA_POKEY;
        }

        // If an error is caught, the plugin will be unloaded with an error message showing the problematic procedures
        MessageBox(g_hwnd, "Error:\nNo compatible 'sa_pokey.dll',\ntherefore the Pokey sound can't be performed.\nIncompatibility with:" + warningMessage, "Pokey library error", MB_ICONEXCLAMATION);
        DeInitPokeyDll();
    }

    // If no POKEY emulation plugin was found, no sound emulation will be output
    MessageBox(g_hwnd, "Warning:\nNone of 'apokeysnd.dll' or 'sa_pokey.dll' found,\ntherefore the Pokey sound can't be performed.", "LoadLibrary error", MB_ICONEXCLAMATION);

    return NONE;
}

void CPokey::DeInitPokeyDll() {

    if (m_pokey_dll)
    {
        FreeLibrary(m_pokey_dll);
        m_pokey_dll = NULL;
    }

}

CPokey::SoundDriver CPokey::GetSoundDriver() const {
    return m_soundDriver;
}

bool CPokey::IsSoundDriverLoaded() const {
    return m_soundDriver != NONE;
}


void CPokey::InitPokeys(const bool ntsc, const bool stereo, const DWORD samplesPerSec) {

    if (!m_initialized || m_ntsc != ntsc || m_stereo != stereo || m_samplesPerSec!= samplesPerSec) {
        switch (GetSoundDriver())
        {
        case CPokey::SoundDriver::APOKEYSND:
            APokeySound_Initialize(stereo);

            break;

        case CPokey::SoundDriver::SA_POKEY:
            // Currently cast to WORD, because no rate avve 64kHz are supported.
            Pokey_SoundInit(CAtari::GetClockFrequency(ntsc), (WORD)samplesPerSec, stereo ? 2 : 1);
            break;
        }

        m_initialized = true;
        m_ntsc = ntsc;
        m_stereo = stereo;
        m_samplesPerSec = samplesPerSec;
    }
}

void CPokey::PutByte(const byte address, const byte value) {
    switch (GetSoundDriver())
    {
    case CPokey::SoundDriver::APOKEYSND:

        APokeySound_PutByte(address, value);
        break;

    case CPokey::SoundDriver::SA_POKEY:

        Pokey_PutByte(address, value);
        break;
    }
}