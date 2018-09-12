/*  Copyright (c) MediaArea.net SARL & AV Preservation by reto.ch.
*
*  Use of this source code is governed by a BSD-style license that can
*  be found in the License.html file in the root of the source tree.
*/

//---------------------------------------------------------------------------
#ifndef License_ListH
#define License_ListH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Lib/DPX/DPX.h"
#include "Lib/RIFF/RIFF.h"
//---------------------------------------------------------------------------

static const uint8_t DPX_Flags_List[] =
{
    dpx::Raw_RGB_8,
    dpx::Raw_RGB_10_FilledA_LE,
    dpx::Raw_RGB_10_FilledA_BE,
    dpx::Raw_RGB_12_Packed_BE,
    dpx::Raw_RGB_12_FilledA_BE,
    dpx::Raw_RGB_12_FilledA_LE,
    dpx::Raw_RGB_16_BE,
    dpx::Raw_RGB_16_LE,
    dpx::Raw_RGBA_8,
    dpx::Raw_RGBA_10_FilledA_LE,
    dpx::Raw_RGBA_10_FilledA_BE,
    dpx::Raw_RGBA_12_Packed_BE,
    dpx::Raw_RGBA_12_FilledA_BE,
    dpx::Raw_RGBA_12_FilledA_LE,
    dpx::Raw_RGBA_16_BE,
    dpx::Raw_RGBA_16_LE,
    (uint8_t)-1,
};

static const uint8_t WAV_Flags_List[] =
{
    riff::PCM_48000_16_2_LE,
    riff::PCM_48000_16_6_LE,
    riff::PCM_48000_24_2_LE,
    riff::PCM_48000_24_6_LE,
    riff::PCM_48000_8_1_U,
    riff::PCM_48000_8_2_U,
    riff::PCM_48000_8_6_U,
    riff::PCM_48000_16_1_LE,
    riff::PCM_48000_24_1_LE,
    riff::PCM_96000_8_1_U,
    riff::PCM_96000_8_2_U,
    riff::PCM_96000_8_6_U,
    riff::PCM_44100_8_1_U,
    riff::PCM_44100_8_2_U,
    riff::PCM_44100_8_6_U,
    riff::PCM_44100_16_1_LE,
    riff::PCM_44100_16_2_LE,
    riff::PCM_44100_16_6_LE,
    riff::PCM_44100_24_1_LE,
    riff::PCM_44100_24_2_LE,
    riff::PCM_44100_24_6_LE,
    riff::PCM_96000_16_1_LE,
    riff::PCM_96000_16_2_LE,
    riff::PCM_96000_16_6_LE,
    riff::PCM_96000_24_1_LE,
    riff::PCM_96000_24_2_LE,
    riff::PCM_96000_24_6_LE,
    (uint8_t)-1,
};

//---------------------------------------------------------------------------
static void Get_EB(unsigned char* Buffer, uint64_t& Offset, uint64_t Size, uint64_t& Value)
{
    Value = Buffer[Offset];
    uint64_t s = 0;
    while (!(Value&(((uint64_t)1) << (7 - s))))
        s++;
    Value ^= (((uint64_t)1) << (7 - s));
    while (s)
    {
        Value <<= 8;
        Offset++;
        if (Offset >= Size)
            break;
        s--;
        Value |= Buffer[Offset];
    }
    Offset++;
}
static uint64_t Get_EB(unsigned char* Buffer, uint64_t& Offset, uint64_t Size) { uint64_t Value; Get_EB(Buffer, Offset, Size, Value); return Value; }

//---------------------------------------------------------------------------
static void Put_EB(uint8_t* Buffer, size_t& Offset, uint64_t Size, uint64_t Value)
{
    size_t N_l = 1;
    while (Value >> (N_l * 7))
        N_l++;
    uint64_t N2 = Value | (((uint64_t)1) << (N_l * 7));
    while (N_l)
    {
        Buffer[Offset] = (uint8_t)(N2 >> ((N_l - 1) * 8));
        Offset++;
        if (Offset >= Size)
        {
            Offset++;
            break;
        }
        N_l--;
    }
}

//---------------------------------------------------------------------------
struct license_internal
{
    time_t              Date;
    uint64_t            UserID;
    uint64_t            Features_Flags;
    uint64_t            DPX_Flags;
    uint64_t            WAV_Flags;

    license_internal();

    uint64_t            Flags(uint64_t Date);
    static void         AddFlavor(uint64_t &Value, const uint8_t List[], uint8_t Flavor);
    static const char*  SupportedFlavor(uint64_t &Value, const uint8_t List[], uint8_t Flavor);
};

#endif
