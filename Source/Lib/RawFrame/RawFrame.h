/*  Copyright (c) MediaArea.net SARL & AV Preservation by reto.ch.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifndef RawFrameH
#define RawFrameH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Lib/Config.h"
#include <cstring>
#include <vector>
using namespace std;
//---------------------------------------------------------------------------

struct buffer_base
{
protected:
    buffer_base() = delete;
    buffer_base(size_t Size_, const uint8_t* Data_) :
        Size(Size_),
        Data(Data_)
    {}
    buffer_base(buffer_base& Buffer) = delete;
    buffer_base(buffer_base&& Buffer) = delete;
    buffer_base& operator = (const buffer_base& Buffer) = delete;
    buffer_base& operator = (const buffer_base&& Buffer) = delete;

    virtual ~buffer_base() = 0 {};

    void ClearBase()
    {
        Size = 0;
        Data = nullptr;
    }

    void AssignBase(const buffer_base& Buffer)
    {
        Size = Buffer.Size;
        Data = Buffer.Data;
    }

    void AssignBase(size_t Size_, const uint8_t* Data_)
    {
        Size = Size_;
        Data = Data_;
    }

public:
    const size_t GetSize() const
    {
        return Size;
    }

    const uint8_t* const GetData() const
    {
        return Data;
    }

    const uint8_t& operator [] (size_t n) const
    {
        return Data[n];
    }

    const bool Empty() const
    {
        return !Size;
    }

    string ToString()
    {
        return string((const char*)GetData(), GetSize());
    }

protected:
    size_t Size;
    const uint8_t* Data;
};

struct buffer : buffer_base
{
    buffer() :
        buffer_base(0, nullptr)
    {}
    buffer(const buffer& Buffer) :
        buffer_base(Buffer.Size, Buffer.Data)
    {}
    buffer(buffer&& Buffer) :
        buffer_base(Buffer.Size, Buffer.Data)
    {
        Buffer.ClearBase();
    }

    buffer& operator = (buffer&& Buffer)
    {
        if (this == &Buffer)
            return *this;
        AssignBase(Buffer);
        Buffer.ClearBase();
        return *this;
    }

    ~buffer()
    {
        delete[] GetData();
    }

    size_t GetSizeForModification() const
    {
        return GetSize();
    }

    uint8_t* GetDataForModification() const
    {
        return (uint8_t*)Data;
    }

    void Create(size_t Size_)
    {
        delete[] Data;
        AssignBase(Size_, new uint8_t[Size_]);
    }

    size_t CopyFrom(size_t Pos, const buffer_base& Buffer_Source)
    {
        auto SizeToCopy_Max = GetSizeForModification();
        if (Buffer_Source.Empty() || Pos > SizeToCopy_Max)
            return 0;
        SizeToCopy_Max  -= Pos;
        auto SizeToCopy = Buffer_Source.GetSize();
        if (SizeToCopy > SizeToCopy_Max)
            SizeToCopy = SizeToCopy_Max;
        memcpy(GetDataForModification() + Pos, Buffer_Source.GetData(), SizeToCopy);
        return SizeToCopy;
    }
    size_t CopyFrom(const buffer_base& Buffer_Source)
    {
        return CopyFrom(0, Buffer_Source);
    }

    void SetZero()
    {
        memset(GetDataForModification(), 0, Size);
    }

    void SetZero(size_t Offset, size_t Count)
    {
        if (Offset >= Size || Count >= Size || Offset >= Size - Count)
            return;
        memset(GetDataForModification(), 0, Size);
    }

    void Copy(const uint8_t* Data_, size_t Size_)
    {
        Create(Size_);
        memcpy(GetDataForModification(), Data_, Size);
    }

    void Resize(size_t Size_)
    {
        if (Size_ < GetSize())
        {
            AssignBase(Size_, GetData()); // We just change the Size value, not shrink of memory
            return;
        }
        auto NewBuffer = new uint8_t[Size_];
        memcpy(NewBuffer, GetData(), Size_);
        AssignBase(Size_, NewBuffer);
    }

    void Clear()
    {
        delete[] Data;
        ClearBase();
    }
};

struct buffer_view : buffer_base
{
    buffer_view() :
        buffer_base(0, nullptr)
    {}
    buffer_view(const uint8_t* Data_, size_t Size_) :
        buffer_base(Size_, Data_)
    {}
    buffer_view(const buffer_view& Buffer) :
        buffer_base(Buffer.Size, Buffer.Data)
    {}
    buffer_view(const buffer_base& Buffer) :
        buffer_base(Buffer.GetSize(), (uint8_t*)Buffer.GetData()) // static cast, we guarantee that the class permits only const
    {}
    buffer_view(buffer_view&& Buffer) :
        buffer_base(Buffer.Size, Buffer.Data)
    {
        Buffer.ClearBase();
    }

    buffer_view& operator = (const buffer_view& Buffer)
    {
        AssignBase(Buffer);
        return *this;
    }
    buffer_view& operator = (buffer_view&& Buffer)
    {
        if (this == &Buffer)
            return *this;
        AssignBase(Buffer);
        Buffer.ClearBase();
        return *this;
    }

    ~buffer_view() = default;
    
    void Clear()
    {
        ClearBase();
    }
};

struct buffer_or_view : buffer_base
{
    buffer_or_view() :
        buffer_base(0, nullptr)
    {}
    /*buffer_or_view(uint8_t* Data_, size_t Size_) :
        buffer_base(Size_, Data_),
        IsOwned(true)
    {}*/
    buffer_or_view(const uint8_t* Data_, size_t Size_) :
        buffer_base(Size_, Data_)
    {}
    buffer_or_view(const buffer_or_view& Buffer) :
        buffer_base(Buffer.Size, Buffer.Data)
    {}
    buffer_or_view(const buffer& Buffer) :
        buffer_base(Buffer.GetSize(), Buffer.GetData())
    {}
    buffer_or_view(buffer_or_view&& Buffer) :
        buffer_base(Buffer.Size, Buffer.Data),
        IsOwned(Buffer.IsOwned)
    {
        Buffer.ClearBase();
    }

    buffer_or_view& operator = (buffer_or_view&& Buffer)
    {
        if (this == &Buffer)
            return *this;
        AssignBase(Buffer);
        Buffer.ClearBase();
        if (Buffer.IsOwned)
        {
            IsOwned = true;
            Buffer.IsOwned = false;
        }
        return *this;
    }

    ~buffer_or_view()
    {
        if (!IsOwned)
            return;
        delete[] Data;
    }

    size_t GetSizeForModification() const
    {
        return IsOwned ? Size : 0;
    }

    uint8_t* GetDataForModification() const
    {
        return IsOwned ? (uint8_t*)Data : nullptr;
    }

    void Create(size_t Size_)
    {
        if (IsOwned)
            delete[] Data;
        else
            IsOwned = true;
        AssignBase(Size_, new uint8_t[Size_]);
    }

    void Copy(const uint8_t* Data_, size_t Size_)
    {
        Create(Size_);
        memcpy(GetDataForModification(), Data_, Size);
    }

    void Resize(size_t Size_)
    {
        if (Size_ < GetSize())
        {
            AssignBase(Size_, GetData()); // We just change the Size value, not shrink of memory
            return;
        }
        auto OldBuffer = GetData();
        auto NewBuffer = new uint8_t[Size_];
        memcpy(NewBuffer, OldBuffer, Size_);
        AssignBase(Size_, NewBuffer);
        if (IsOwned)
            delete[] OldBuffer;
        else
            IsOwned = true;
    }

    void Clear()
    {
        ClearBase();
        IsOwned = false;
    }

    bool IsOwned = false;
};

class raw_frame
{
public:
    uint64_t                    Flavor_Private; //Used by specialized flavor for marking the configuration of such flavor (e.g. endianness of DPX)
    buffer_view                 Pre;
    buffer_view                 Post;
    buffer_view                 In;
    buffer_or_view              Buffer;

    struct plane
    {
        buffer                  Buffer;
        size_t                  Width;
        size_t                  Width_Padding;
        size_t                  Height;
        size_t                  BitsPerBlock;
        size_t                  PixelsPerBlock;

        plane(size_t Width_, size_t Height_, size_t BitsPerBlock_, size_t PixelsPerBlock_ = 1)
            :
            Width(Width_),
            Height(Height_),
            BitsPerBlock(BitsPerBlock_),
            PixelsPerBlock(PixelsPerBlock_)
        {
            Width_Padding=0; //TODO: option for padding size
            if (Width_Padding)
                Width_Padding-=Width%Width_Padding;
                
            Buffer.Create((Width+Width_Padding)*Height*BitsPerBlock/PixelsPerBlock/8);
        }

        size_t ValidBytesPerLine()
        {
            return Width*BitsPerBlock/PixelsPerBlock/8;
        }

        size_t AllBytesPerLine()
        {
            return (Width+Width_Padding)*BitsPerBlock/PixelsPerBlock/8;
        }
    };
    std::vector<plane*> Planes;

    enum flavor
    {
        Flavor_FFmpeg,
        Flavor_DPX,
        Flavor_TIFF,
        Flavor_Max,
    };
    flavor                       Flavor;

    raw_frame() :
        Flavor_Private(0),
        Flavor(Flavor_Max)
    {
    }
    
    ~raw_frame()
    {
        for (size_t i = 0; i < Planes.size(); i++)
            delete Planes[i];
    }

    // Creation
    void Create(size_t colorspace_type, size_t width, size_t height, size_t bits_per_raw_sample, bool chroma_planes, bool alpha_plane, size_t h_chroma_subsample, size_t v_chroma_subsample);

    // Info
    size_t GetFrameSize();
    size_t GetTotalSize();

private:
    void FFmpeg_Create(size_t colorspace_type, size_t width, size_t height, size_t bits_per_raw_sample, bool chroma_planes, bool alpha_plane, size_t h_chroma_subsample, size_t v_chroma_subsample);
    void DPX_Create(size_t colorspace_type, size_t width, size_t height, size_t bits_per_raw_sample, bool chroma_planes, bool alpha_plane, size_t h_chroma_subsample, size_t v_chroma_subsample);
    void TIFF_Create(size_t colorspace_type, size_t width, size_t height, size_t bits_per_raw_sample, bool chroma_planes, bool alpha_plane, size_t h_chroma_subsample, size_t v_chroma_subsample);
};

//---------------------------------------------------------------------------
#endif
