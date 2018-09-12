/*  Copyright (c) MediaArea.net SARL & AV Preservation by reto.ch.
*
*  Use of this source code is governed by a BSD-style license that can
*  be found in the License.html file in the root of the source tree.
*/

//---------------------------------------------------------------------------
#ifndef LicenseH
#define LicenseH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <cstdint>
#include <string>
using namespace std;
//---------------------------------------------------------------------------



struct license
{
public:
    // Enums
    enum feature
    {
        Feature_GeneralOptions,
        Feature_InputOptions,
        Feature_EncodingOptions,
        Feature_MultipleTracks,
        Feature_Matroska,
        Feature_FFV1,
        Feature_FLAC,
        Feature_PCM,
        Feature_Max,
    };

    enum input
    {
        Input_DPX,
        Input_WAV,
        Input_Max,
    };

    // Constructor/Destructor
    license();
    ~license();

    // Info
    void LoadLicense(string LicenseKey, bool StoreLicenseKey);
    void ShowLicense(bool Verbose=false);

    // Checks
    bool IsSupported_Feature(enum feature Feature);
    bool IsSupported_DPX(uint64_t Style);
    bool IsSupported_WAV(uint64_t Style);

private:
    // Internal
    void* Internal;
};

#endif
