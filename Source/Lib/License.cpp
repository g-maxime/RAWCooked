/*  Copyright (c) MediaArea.net SARL & AV Preservation by reto.ch.
*
*  Use of this source code is governed by a BSD-style license that can
*  be found in the License.html file in the root of the source tree.
*/

//---------------------------------------------------------------------------
#include "Lib/Matroska/Matroska_Common.h"
#include "Lib/License.h"
#include "Lib/License_Internal.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    #include <Shlobj.h>
    #define PathSeparator "\\"
#else
    #define PathSeparator "/"
#endif
using namespace std;
//---------------------------------------------------------------------------

//***************************************************************************
// Info
//***************************************************************************

//---------------------------------------------------------------------------
const char* Features_Names[license::Feature_Max]=
{
    "General options",
    "Input options",
    "Encoding options",
    "Multiple audio tracks",
    "Muxing to / demuxing from Matroska",
    "Encoding to / decoding from FFV1",
    "Encoding to / decoding from FLAC",
    "Encoding to / decoding from PCM",
};
//---------------------------------------------------------------------------

//***************************************************************************
// Secure version of some OS calls
//***************************************************************************

//---------------------------------------------------------------------------
string secure_getenv(const char* name)
{
    // Get size
    size_t len;
    if (getenv_s(&len, NULL, 0, name) || !len)
        return string();

    // Get value
    char* value = new char[len * sizeof(char)];
    if (getenv_s(&len, value, len, name))
        return string();

    // Create string
    string valueString(value, len);
    delete[] value;
    return valueString;
}

//---------------------------------------------------------------------------
tm secure_localtime(const time_t& time)
{
    tm result;
    #if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
        localtime_s(&result, &time);
    #else
        localtime_r(time, &result);
    #endif

    return result;
}

//---------------------------------------------------------------------------
string GetLocalConfigPath ()
{
    string result;
    #if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
        char szPath[MAX_PATH];
        if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
            result = szPath;
    #else
    #endif

    if (!result.empty())
        result += PathSeparator "RAWcooked";
    return result;
}

//---------------------------------------------------------------------------
bool CreateDirectory (const string& Dir)
{
    #if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
        if (SUCCEEDED(CreateDirectoryA(Dir.c_str(), NULL)))
            return false;
    #else
        if (!mkdir(Dir.c_str(), 0700))
            return false;
    #endif

    cerr << "Problem while creating " << Dir << " directory" << endl;
        
    return true;
}

//***************************************************************************
// License decode
//***************************************************************************

//---------------------------------------------------------------------------
bool DecodeLicense(const string& FromUser, license_internal* License)
{
    // Place holder for license key descrambling - Begin
    cerr << "License decoding not supported, please contact info@mediaarea.net" << endl;
    return true; // Not supported
    // Place holder for license key descrambling - End
}

//***************************************************************************
// license_internal
//***************************************************************************

//---------------------------------------------------------------------------
license_internal::license_internal()
    : Date((time_t)-1)
    , UserID(0)
    , Features_Flags(0)
    , DPX_Flags(0)
    , WAV_Flags(0)
{
    Features_Flags |= (1 << license::Feature_Matroska);
    Features_Flags |= (1 << license::Feature_FFV1);
    Features_Flags |= (1 << license::Feature_FLAC);

    AddFlavor(DPX_Flags, DPX_Flags_List, dpx::Raw_RGB_8);
    AddFlavor(DPX_Flags, DPX_Flags_List, dpx::Raw_RGB_10_FilledA_LE);
    AddFlavor(DPX_Flags, DPX_Flags_List, dpx::Raw_RGB_10_FilledA_BE);

    AddFlavor(WAV_Flags, WAV_Flags_List, riff::PCM_48000_16_2_LE);
}

void license_internal::AddFlavor(uint64_t &Value, const uint8_t List[], uint8_t Flavor)
{
    for (size_t i = 0; List[i] != (uint8_t)-1; i++)
        if (List[i] == Flavor)
            Value |= (((uint64_t)1) << i);
}

const char* license_internal::SupportedFlavor(uint64_t &Value, const uint8_t List[], uint8_t Flavor)
{
    for (size_t i = 0; List[i] != (uint8_t)-1; i++)
        if (List[i] == Flavor)
            return (Value & (((uint64_t)1) << i))?"Yes":"No ";
    return "No ";
}

uint64_t license_internal::Flags(uint64_t Date)
{
    return 0
            | ((Date             != (uint64_t)-1 ? 1 : 0) << 0)
            | ((UserID                           ? 1 : 0) << 1)
            | ((Features_Flags                   ? 1 : 0) << 2)
            | ((DPX_Flags                        ? 1 : 0) << 3)
            | ((WAV_Flags                        ? 1 : 0) << 4)
            ;
}

//***************************************************************************
// License class
//***************************************************************************

//---------------------------------------------------------------------------
license::license()
{
    license_internal* License = new license_internal();
    Internal = License;
}

//---------------------------------------------------------------------------
license::~license()
{
    delete (license_internal*)Internal;
}

//---------------------------------------------------------------------------
void license::LoadLicense(string LicenseKey, bool StoreLicenseKey)
{
    // Read license from environment variable
    if (LicenseKey.empty())
        LicenseKey = secure_getenv("RAWcooked_License");

    // Read license from a file
    if (LicenseKey.empty())
    {
        string Path = GetLocalConfigPath() + PathSeparator "License.txt";
        ifstream F(Path);
        if (!F.fail())
            F >> LicenseKey;
    }

    // Decode
    if (!LicenseKey.empty())
    {
        if (DecodeLicense(LicenseKey, (license_internal*)Internal))
            return;

        if (StoreLicenseKey)
        {
            string Path = GetLocalConfigPath();
            CreateDirectory(Path);
            Path += PathSeparator "License.txt";
            ofstream F(Path, ios_base::trunc);
            if (!F.fail())
                F << LicenseKey;
        }
    }
}

//---------------------------------------------------------------------------
void license::ShowLicense(bool Verbose)
{
    license_internal* License = (license_internal*)Internal;

    // UserID
    if (License->UserID)
    {
        if (Verbose)
            cerr << "License found, ID=" << License->UserID << '.' << endl;
    }
    else if (License->Date == (int64_t)-1)
        cerr << "No license found, consider to support RAWcooked: https://MediaArea.net/RAWcooked" << endl;

    // Date
    if (License->Date != (int64_t)-1)
    {
        struct tm TimeInfo = secure_localtime(License->Date);
        cerr << "Temporary license, valid until " << TimeInfo.tm_year + 1900 << '-' << setfill('0') << setw(2) << TimeInfo.tm_mon + 1 << '-' << setfill('0') << setw(2) << TimeInfo.tm_mday << '.' << endl;

        time_t Time;
        time(&Time);
        if (Time > License->Date)
        {
            cerr << "Outdated license, using default license" << endl;
            *License = license_internal();
        }
    }

    if (!Verbose)
        return;

    // Features
    if (License->Features_Flags)
    {
        cerr << endl;
        cerr << "Licensed features:" << endl;
        for (int8_t i = 0; i < license::Feature_Max; i++)
        {
            const char* Supported = (License->Features_Flags & ((uint64_t)1 << i)) ? "Yes" : "No ";
            cerr << Supported << ' ' << Features_Names[i] << endl;
        }
    }

    // DPX
    if (License->DPX_Flags)
    {
        cerr << endl;
        cerr << "Licensed DPX flavors:" << endl;
        for (int8_t i = 0; i < dpx::Style_Max; i++)
        {
            const char* Supported = License->SupportedFlavor(License->DPX_Flags, DPX_Flags_List, i);
            cerr << Supported << ' ' << dpx::Flavor_String((dpx::style)i) << endl;
        }
    }

    // WAV
    if (License->WAV_Flags)
    {
        cerr << endl;
        cerr << "Licensed WAV/PCM flavors:" << endl;
        for (int8_t i = 0; i < riff::Style_Max; i++)
        {
            const char* Supported = License->SupportedFlavor(License->WAV_Flags, WAV_Flags_List, i);
            cerr << Supported << ' ' << riff::Flavor_String((riff::style)i) << endl;
        }
    }

    cerr << endl;
}

//---------------------------------------------------------------------------
bool license::IsSupported_Feature(enum feature Feature)
{
    license_internal* License = (license_internal*)Internal;

    return (License->Features_Flags & ((uint64_t)1 << Feature));
}

//---------------------------------------------------------------------------
bool license::IsSupported_DPX(uint64_t Style)
{
    license_internal* License = (license_internal*)Internal;

    const char* Supported = License->SupportedFlavor(License->DPX_Flags, DPX_Flags_List, (uint8_t)Style);
    return Supported[0] == 'Y';
}

//---------------------------------------------------------------------------
bool license::IsSupported_WAV(uint64_t Style)
{
    license_internal* License = (license_internal*)Internal;

    const char* Supported = License->SupportedFlavor(License->WAV_Flags, WAV_Flags_List, (uint8_t)Style);
    return Supported[0] == 'Y';
}
