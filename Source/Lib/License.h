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
#include <Lib/Input_Base.h>
using namespace std;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Features
static const size_t __feature_line = __LINE__;
enum class feature : uint8_t
{
    GeneralOptions,
    InputOptions,
    EncodingOptions,
    MultipleTracks,
};
static const size_t Feature_Max = __LINE__ - __feature_line - 4;

//---------------------------------------------------------------------------
// Muxer
static const size_t __muxer_line = __LINE__;
enum class muxer : uint8_t
{
    Matroska,
};
static const size_t Muxer_Max = __LINE__ - __muxer_line - 4;

//---------------------------------------------------------------------------
// Encoder
static const size_t __encoder_line = __LINE__;
enum class encoder : uint8_t
{
    FFV1,
    PCM,
    FLAC,
};
static const size_t Encoder_Max = __LINE__ - __encoder_line - 4;

//---------------------------------------------------------------------------
struct license
{
public:
    // Constructor/Destructor
    license();
    ~license();

    // Set license info
    void Feature(feature Value);
    void Muxer(muxer Value);
    void Encoder(encoder Value);

    // License management
    bool LoadLicense(string LicenseKey, bool StoreLicenseKey);
    void ShowLicense(bool Verbose=false);

    // Checks
    bool IsSupported();
    bool IsSupported(feature Feature);
    bool IsSupported(parser Parser, uint8_t Flavor);
    bool IsSupported_License();

private:
    // Internal
    void*       Internal;
};

#endif
