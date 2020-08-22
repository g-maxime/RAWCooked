/*  Copyright (c) MediaArea.net SARL & AV Preservation by reto.ch.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifndef MatroskaH
#define MatroskaH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Lib/FFV1/FFV1_Frame.h"
#include "Lib/Input_Base.h"
#include "Lib/FileIO.h"
#include <bitset>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;
//---------------------------------------------------------------------------

namespace matroska_issue
{
    namespace undecodable { enum code : uint8_t; }
    namespace unsupported { enum code : uint8_t; }
}

class matroska;
class matroska_mapping;
class ThreadPool;
class flac_info;
class hashes;

class frame_writer
{
public:
    // Constructor / Destructor
    frame_writer(const string& BaseDirectory_Source, user_mode* UserMode_Soure, ask_callback Ask_Callback_Source, matroska* M_Source, errors* Errors_Source = nullptr) :
        BaseDirectory(BaseDirectory_Source),
        UserMode(UserMode_Soure),
        Ask_Callback(Ask_Callback_Source),
        M(M_Source),
        Errors(Errors_Source),
        MD5(nullptr)
    {
    }
    frame_writer(const frame_writer& Source) :
        Mode(Source.Mode),
        BaseDirectory(Source.BaseDirectory),
        UserMode(Source.UserMode),
        Ask_Callback(Source.Ask_Callback),
        M(Source.M),
        Errors(Source.Errors),
        Offset(Source.Offset),
        SizeOnDisk(Source.SizeOnDisk),
        MD5(Source.MD5)
    {
    }

    ~frame_writer();

    // Config
    enum mode
    {
        IsNotBegin,
        IsNotEnd,
        NoWrite,
        NoOutputCheck,
        mode_Max,
    };
    bitset<mode_Max>            Mode;

    // Actions
    void                        FrameCall(raw_frame* RawFrame, const string& OutputFileName);

private:
    bool                        WriteFile(raw_frame* RawFrame);
    bool                        CheckFile(raw_frame* RawFrame);
    bool                        CheckMD5(raw_frame* RawFrame);
    file                        File_Write;
    filemap                     File_Read;
    string                      BaseDirectory;
    user_mode*                  UserMode;
    ask_callback                Ask_Callback;
    matroska*                   M;
    errors*                     Errors;
    size_t                      Offset;
    size_t                      SizeOnDisk;
    void*                       MD5;
};

void SanitizeFileName(buffer& FileName);

class matroska : public input_base
{
public:
    matroska(const string& OutputDirectoryName, user_mode* Mode, ask_callback Ask_Callback, errors* Errors = nullptr);
    ~matroska();

    void                        Shutdown();

    bool                        Quiet;
    bool                        NoWrite;
    bool                        NoOutputCheck;
    hashes*                     Hashes_FromRAWcooked;
    hashes*                     Hashes_FromAttachments;

    // Theading relating functions
    void                        ProgressIndicator_Show();

    // libFLAC related helping functions
    void                        FLAC_Read(uint8_t buffer[], size_t* bytes);
    void                        FLAC_Tell(uint64_t* absolute_byte_offset);
    void                        FLAC_Metadata(uint8_t channels, uint8_t bits_per_sample);
    void                        FLAC_Write(const uint32_t* buffer[], size_t blocksize);

private:
    void                        ParseBuffer();
    void                        BufferOverflow();
    void                        Undecodable(matroska_issue::undecodable::code Code) { input_base::Undecodable((error::undecodable::code)Code); }
    void                        Unsupported(matroska_issue::unsupported::code Code) { input_base::Unsupported((error::unsupported::code)Code); }

    typedef void (matroska::*call)();
    typedef call(matroska::*name)(uint64_t);

    static const size_t Levels_Max = 16;
    struct levels_struct
    {
        name SubElements;
        uint64_t Offset_End;
    };
    levels_struct Levels[Levels_Max];
    size_t Level;
    bool IsList;

    #define MATROSKA_ELEMENT(_NAME) \
        void _NAME(); \
        call SubElements_##_NAME(uint64_t Name);

    #define MATROSKA_ELEM_XY(_NAME, _X, _Y) \
        void _NAME##_X##_Y() { Segment_Attachments_AttachedFile_FileData_RawCookedxxx_yyy(Element_##_Y, Type_##_X); } \
        call SubElements_##_NAME##_X##_Y(uint64_t Name);

    MATROSKA_ELEMENT(_);
    MATROSKA_ELEMENT(Segment);
    MATROSKA_ELEMENT(Segment_Attachments);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileName);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedAttachment);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedAttachment_FileHash);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedAttachment_FileName);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedAttachment_FileSize);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedBlock);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Block_, FileName);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Block_, AfterData);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Block_, BeforeData);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Block_, InData);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Block_MaskAddition, FileName);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Block_MaskAddition, AfterData);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Block_MaskAddition, BeforeData);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Block_MaskAddition, InData);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedBlock_FileHash);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedBlock_FileSize);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedSegment);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedSegment_LibraryName);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedSegment_LibraryVersion);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedSegment_PathSeparator);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedTrack);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Track_, FileName);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Track_, AfterData);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Track_, BeforeData);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Track_, InData);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Track_MaskBase, AfterData);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Track_MaskBase, BeforeData);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Track_MaskBase, FileName);
    MATROSKA_ELEM_XY(Segment_Attachments_AttachedFile_FileData_RawCooked, Track_MaskBase, InData);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedTrack_FileHash);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedTrack_LibraryName);
    MATROSKA_ELEMENT(Segment_Attachments_AttachedFile_FileData_RawCookedTrack_LibraryVersion);
    MATROSKA_ELEMENT(Segment_Cluster);
    MATROSKA_ELEMENT(Segment_Cluster_SimpleBlock);
    MATROSKA_ELEMENT(Segment_Cluster_Timestamp);
    MATROSKA_ELEMENT(Segment_Tracks);
    MATROSKA_ELEMENT(Segment_Tracks_TrackEntry);
    MATROSKA_ELEMENT(Segment_Tracks_TrackEntry_CodecID);
    MATROSKA_ELEMENT(Segment_Tracks_TrackEntry_CodecPrivate);
    MATROSKA_ELEMENT(Segment_Tracks_TrackEntry_Video);
    MATROSKA_ELEMENT(Segment_Tracks_TrackEntry_Video_PixelWidth);
    MATROSKA_ELEMENT(Segment_Tracks_TrackEntry_Video_PixelHeight);
    MATROSKA_ELEMENT(Void);

    enum format
    {
        Format_None,
        Format_FFV1,
        Format_FLAC,
        Format_PCM,
        Format_Max,
    };

    string                      RAWcooked_LibraryName;
    string                      RAWcooked_LibraryVersion;
    enum element
    {
        Element_FileName,
        Element_BeforeData,
        Element_AfterData,
        Element_InData,
        Element_Max,
    };
    enum type
    {
        Type_Block_,
        Type_Block_MaskAddition,
        Type_Track_,
        Type_Track_MaskBase,
    };
    struct reversibility_data
    {
        struct content_per_element
        {
            buffer              Mask;
            buffer*             Content = nullptr;

            void Check(size_t Size = 1000000)
            {
                if (Content)
                    return;
                Content = new buffer[Size];
                memset(Content, 0x00, Size * sizeof(buffer*));
            }

            ~content_per_element()
            {
                delete[] Content;
            }
        };
        struct filesize_per_element
        {
            uint64_t*           Value = nullptr;

            void Check(size_t Size)
            {
                if (Value)
                    return;
                Value = new uint64_t[Size];
                memset(Value, -1, Size * sizeof(uint64_t));
            }

            ~filesize_per_element()
            {
                delete[] Value;
            }
        };

        void AddFrame()
        {
            Pos = Count;
            Count++;
        }

        void StartParsing()
        {
            Pos = 0;
            if (!Count && Unique)
                Count++;
        }

        void NextFrame()
        {
            Pos++;
        }

        void SetUnique()
        {
            if (Unique)
                return;
            Unique = true;
        }

        void MoveToDataMask(element Element, buffer& Buffer)
        {
            Data[Element].Mask = move(Buffer);
        }

        void MoveToDataContent(element Element, buffer& Buffer, bool AddMask)
        {
            Data[Element].Check(Unique ? 1 : 1000000);
            Data[Element].Content[Pos] = move(Buffer);

            if (AddMask)
            {
                auto& Mask = Data[Element].Mask;
                auto Mask_Size = Mask.GetSize();
                auto Mask_Data = Mask.GetData();
                if (!Mask_Size)
                    return;

                auto& Content = Data[Element].Content[Pos];
                auto Content_Size = Content.GetSize();
                auto Content_Data = Content.GetData();
                for (size_t i = 0; i < Content_Size && i < Mask_Size; i++)
                    Content_Data[i] += Mask_Data[i];
            }

            if (Element == Element_FileName)
            {
                auto& Content = Data[Element_FileName].Content[Pos];
                ::SanitizeFileName(Content);
            }
        }

        buffer_view GetDataContent(element Element)
        {
            if (Pos >= Count || !Data[Element].Content || !Data[Element].Content[Pos].GetSize())
                return buffer_view();
            return buffer_view(Data[Element].Content[Pos]);
        }

        void SetFileSize(uint64_t Value)
        {
            FileSize.Check(Unique ? 1 : 1000000);
            FileSize.Value[Pos] = Value;
        }

        uint64_t GetFileSize()
        {
            if (!FileSize.Value)
                return (uint64_t)-1;
            return FileSize.Value[Pos];
        }

        size_t                  Pos = 0;
        size_t                  Count = 0;
        bool                    Unique = false;
        content_per_element     Data[Element_Max];
    private:
        filesize_per_element    FileSize;
    };
    struct trackinfo
    {
        frame_writer            FrameWriter;
        reversibility_data      ReversibilityData;
        raw_frame*              R_A;
        raw_frame*              R_B;
        input_base_uncompressed* DecodedFrameParser;
        flac_info*              FlacInfo;
        frame                   Frame;
        format                  Format;

        trackinfo(frame_writer& FrameWriter_Source) :
            FrameWriter(FrameWriter_Source),
            R_A(nullptr),
            R_B(nullptr),
            DecodedFrameParser(nullptr),
            FlacInfo(nullptr),
            Format(Format_None)
            {
            }
    };
    vector<trackinfo*>          TrackInfo;
    size_t                      TrackInfo_Pos;
    vector<uint8_t>             ID_to_TrackOrder;
    string                      AttachedFile_FileName;
    enum flags
    {
        IsInFromAttachments, // Has both name and size
        ReversibilityFileHasName,
        ReversibilityFileHasSize,
        Flags_Max
    };
    struct attached_file
    {
        uint64_t                FileSizeFromReversibilityFile = 0;
        uint64_t                FileSizeFromAttachments = 0;
        bitset<Flags_Max>       Flags;
    };
    map<string, attached_file>  AttachedFiles;
    set<string>                 AttachedFile_FileNames_IsHash; // Store the files detected as being hash file
    enum reversibility_compat
    {
        Compat_Modern,
        Compat_18_10_1,
    };
    reversibility_compat        ReversibilityCompat = Compat_Modern;
    ThreadPool*                 FramesPool;
    frame_writer                FrameWriter_Template;
    condition_variable          ProgressIndicator_IsEnd;
    bool                        ProgressIndicator_IsPaused = false;
    bool                        RAWcooked_FileNameIsValid;
    uint64_t                    Cluster_Timestamp;
    int16_t                     Block_Timestamp;
    friend class                frame_writer;  

    //Utils
    bool ConfigureVideoFormatAndFlavor(trackinfo* TrackInfo);
    bool GetFormatAndFlavor(trackinfo* TrackInfo, input_base_uncompressed* PotentialParser, raw_frame::flavor Flavor);
    bool ParseDecodedFrame(trackinfo* TrackInfo, input_base_uncompressed* Parser = nullptr); // If Parser is not provided, TrackInfo decoder and frame size is used
    void Uncompress(buffer& Buffer);
    void Segment_Attachments_AttachedFile_FileData_RawCookedxxx_yyy(element Element, type Type);
    void StoreFromCurrentToEndOfElement(buffer& Output);
    void RejectIncompatibleVersions();
    void ProcessCodecPrivate_FFV1();
    void ProcessCodecPrivate_FLAC();
    void ProcessFrame_FLAC();
};

//---------------------------------------------------------------------------
#endif
