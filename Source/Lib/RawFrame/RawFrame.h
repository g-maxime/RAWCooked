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
public:
    const uint8_t* const Data() const
    {
        return Data_;
    }

    const uint8_t& operator [] (size_t n) const
    {
        return Data_[n];
    }

    const size_t Size() const
    {
        return Size_;
    }

    const bool Empty() const
    {
        return !Size_;
    }

    string ToString()
    {
        return move(string((const char*)Data(), Size()));
    }

protected:
    buffer_base() = delete;
    buffer_base(const uint8_t* NewData, size_t NewSize) :
        Size_(NewSize),
        Data_(NewData)
    {}
    buffer_base(buffer_base& Buffer) = delete;
    buffer_base(buffer_base&& Buffer) = delete;
    buffer_base& operator = (const buffer_base& Buffer) = delete;
    buffer_base& operator = (const buffer_base&& Buffer) = delete;

    virtual ~buffer_base() = 0 {};

    void ClearBase()
    {
        Size_ = 0;
        Data_ = nullptr;
    }

    void ClearKeepSizeBase() // Use it only as intermediate setting
    {
        Data_ = nullptr;
    }

    void AssignBase(const buffer_base& Buffer)
    {
        AssignBase(Buffer.Data(), Buffer.Size());
    }

    void AssignBase(const uint8_t* NewData, size_t NewSize)
    {
        Data_ = NewData;
        Size_ = NewSize;
    }

    void AssignKeepSizeBase(const uint8_t* NewData)
    {
        Data_ = NewData;
    }

    void AssignKeepDataBase(size_t NewSize) // Use it only as intermediate setting
    {
        Size_ = NewSize;
    }

private:
    const uint8_t* Data_;
    size_t Size_;
};

struct buffer : buffer_base
{
    buffer() :
        buffer_base(nullptr, 0)
    {}
    buffer(const buffer& Buffer) = delete;
    buffer(buffer&& Buffer) :
        buffer_base(Buffer.Data(), Buffer.Size())
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
        delete[] Data();
    }

    uint8_t* Data() const
    {
        return (uint8_t*)buffer_base::Data(); // We are sure wa can modify this buffer
    }

    void Create(size_t NewSize)
    {
        delete[] Data();
        if (!NewSize)
        {
            ClearBase();
            return;
        }
        AssignBase(new uint8_t[NewSize], NewSize);
    }

    size_t CopyCut(size_t Offset, const buffer_base& Buffer_Source)
    {
        auto SizeToCopy_Max = Size();
        if (Offset > SizeToCopy_Max)
            return 0;
        SizeToCopy_Max -= Offset;
        auto SizeToCopy = Buffer_Source.Size();
        if (SizeToCopy > SizeToCopy_Max)
            SizeToCopy = SizeToCopy_Max;
        memcpy(Data() + Offset, Buffer_Source.Data(), SizeToCopy);
        return SizeToCopy;
    }
    size_t CopyCut(const buffer_base& Buffer_Source)
    {
        return CopyCut(0, Buffer_Source);
    }

    void CopyExpand(const uint8_t* const NewData, size_t NewSize)
    {
        Create(NewSize);
        memcpy(Data(), NewData, Size());
    }

    void SetZero()
    {
        memset(Data(), 0, Size());
    }

    size_t SetZero(size_t Offset, size_t Count)
    {
        auto SizeToZero = Size();
        if (Offset > SizeToZero)
            return 0;
        SizeToZero -= Offset;
        memset(Data(), 0, SizeToZero);
        return SizeToZero;
    }

    void Resize(size_t NewSize)
    {
        if (!NewSize)
        {
            ClearBase();
            return;
        }
        if (NewSize < Size())
        {
            AssignBase(Data(), NewSize); // We just change the Size value, no shrink of memory
            return;
        }
        auto NewBuffer = new uint8_t[NewSize];
        memcpy(NewBuffer, Data(), Size());
        AssignBase(NewBuffer, NewSize);
    }

    void Clear()
    {
        delete[] Data();
        ClearBase();
    }
};

struct buffer_view : buffer_base
{
    buffer_view() :
        buffer_base(nullptr, 0)
    {}
    buffer_view(const uint8_t* const NewData, size_t NewSize) :
        buffer_base(NewData, NewSize)
    {}
    buffer_view(const buffer_view& Buffer) :
        buffer_base(Buffer.Data(), Buffer.Size())
    {}
    buffer_view(const buffer_base& Buffer) :
        buffer_base(Buffer.Data(), Buffer.Size())
    {}
    buffer_view(buffer_view&& Buffer) :
        buffer_base(Buffer.Data(), Buffer.Size())
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
        buffer_base(nullptr, 0)
    {}
    buffer_or_view(const uint8_t* const NewData, size_t NewSize) :
        buffer_base(NewData, NewSize)
    {}
    buffer_or_view(const buffer_or_view& Buffer) :
        buffer_base(Buffer.Data(), Buffer.Size())
    {}
    buffer_or_view(const buffer& Buffer) :
        buffer_base(Buffer.Data(), Buffer.Size())
    {}
    buffer_or_view(const buffer_view& Buffer) :
        buffer_base(Buffer.Data(), Buffer.Size())
    {}
    buffer_or_view(buffer_or_view&& Buffer) :
        buffer_base(Buffer.Data(), Buffer.Size()),
        IsOwned(Buffer.IsOwned)
    {
        Buffer.ClearBase();
        Buffer.IsOwned = false;
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
        delete[] Data();
    }

    size_t GetSizeForModification() const
    {
        return IsOwned ? Size() : 0;
    }

    uint8_t* GetDataForModification() const
    {
        return IsOwned ? (uint8_t*)Data() : nullptr;
    }

    void Create(size_t NewSize)
    {
        if (IsOwned)
            delete[] Data();
        else
            IsOwned = true;
        AssignBase(new uint8_t[NewSize], NewSize);
    }

    void Copy(const uint8_t* NewData, size_t NewSize)
    {
        Create(NewSize);
        memcpy(GetDataForModification(), NewData, Size());
    }

    void Resize(size_t NewSize)
    {
        if (NewSize < Size())
        {
            AssignBase(Data(), NewSize); // We just change the Size value, no shrink of memory
            return;
        }
        auto OldBuffer = Data();
        auto NewBuffer = new uint8_t[NewSize];
        memcpy(NewBuffer, OldBuffer, NewSize);
        AssignBase(NewBuffer, NewSize);
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

private:
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
