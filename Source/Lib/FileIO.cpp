/*  Copyright (c) MediaArea.net SARL & AV Preservation by reto.ch.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#define _GNU_SOURCE // Needed for ftruncate on GNU compiler
#include "Lib/FileIO.h"
#include <iostream>
#include <sstream>
#if defined(_WIN32) || defined(_WINDOWS)
    #include "windows.h"
    #include <io.h> // File existence
    #include <direct.h> // Directory creation
    #define access _access_s
    #define mkdir _mkdir
    #define stat _stat
#else
    #include <dirent.h>
    #include <fcntl.h>
    #include <glob.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/mman.h>
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int filemap::Open_ReadMode(const char* FileName)
{
    Close();

    size_t NewSize;
#if defined(_WIN32) || defined(_WINDOWS)
    auto NewFile = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (NewFile != INVALID_HANDLE_VALUE)
    {
        DWORD FileSizeHigh;
        auto FileSizeLow = GetFileSize(NewFile, &FileSizeHigh);
        if ((FileSizeLow != INVALID_FILE_SIZE || GetLastError() == NO_ERROR) // If no error (special case with 32-bit max value)
            && (!FileSizeHigh || sizeof(size_t) >= 8)) // Mapping 4+ GiB files is not supported in 32-bit mode
        {
            NewSize = ((size_t)FileSizeHigh) << 32 | FileSizeLow;
            if (NewSize)
            {
                auto Mapping = (HANDLE&)Private2;
                Mapping = CreateFileMapping(NewFile, 0, PAGE_READONLY, 0, 0, 0);
                if (!Mapping)
                    Private = NewFile;
                else
                    CloseHandle(NewFile);
            }
            else
                Private = NewFile; // CreateFileMapping does not support 0-byte files, so we map manually to NULL
        }
        else
        {
            CloseHandle(NewFile);
        }
    }
#else
    auto fd = open(FileName, O_RDONLY, 0);
    if (fd != -1)
    {
        struct stat Fstat;
        if (!stat(FileName, &Fstat))
        {
            NewSize = Fstat.st_size;
            Private = fd;
        }
        else
            close(fd);
    }

#endif

    if (!Private)
        return 1; 
    return Remap(NewSize);
}

//---------------------------------------------------------------------------
int filemap::Remap(size_t NewSize)
{
    // Close previous map
    if (Data())
    {
#if defined(_WIN32) || defined(_WINDOWS)
            UnmapViewOfFile(Data());
#else
            munmap(Data(), NewSize());
#endif
        ClearBase();
    }

    // Special case for 0-byte files
    if (!NewSize)
        return 0;

    // New map
#if defined(_WIN32) || defined(_WINDOWS)
    auto Mapping = (HANDLE&)Private2;
    auto NewData = MapViewOfFile(Mapping, FILE_MAP_READ, 0, 0, 0);
    const decltype(NewData) NewData_Fail = 0;
#else
    auto fd = (int&)Private;
    auto NewData = mmap(NULL, NewSize, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
    const decltype(NewData) NewData_Fail = MAP_FAILED;
#endif

    // Assign
    if (NewData == NewData_Fail)
    {
        Close();
        return 1;
    }
    AssignBase((const uint8_t*)NewData, NewSize);
    return 0;
}

//---------------------------------------------------------------------------
int filemap::Close()
{
    // Close map
    if (Data())
    {
        #if defined(_WIN32) || defined(_WINDOWS)
            UnmapViewOfFile(Data());
        #else
            munmap(Data(), Size());
        #endif
        ClearBase();
    }

    // Close intermediate
#if defined(_WIN32) || defined(_WINDOWS)
    auto Mapping = (HANDLE&)Private2;
    if (Mapping)
    {
        CloseHandle(Mapping);
        Private2 = NULL;
    }
#endif

    // Close file
    if (Private)
    {
#if defined(_WIN32) || defined(_WINDOWS)
            auto File = (HANDLE&)Private;
            if (!CloseHandle(File))
            {
                Private = nullptr;
                return 1;
            }
#else
            int& fd = (int&)Private;
            if (close(fd))
            {
                Private = (void*)-1;
                return 1;
            }
#endif
        Private = nullptr;
    }

    return 0;
}

//---------------------------------------------------------------------------
// file

file::return_value file::Open_WriteMode(const string& BaseDirectory, const string& OutputFileName_Source, bool RejectIfExists, bool Truncate)
{
    Close();

    string FullName = BaseDirectory + OutputFileName_Source;

#if defined(_WIN32) || defined(_WINDOWS)
    HANDLE& P = (HANDLE&)Private;
    DWORD CreationDisposition = Truncate ? (RejectIfExists ? TRUNCATE_EXISTING : CREATE_ALWAYS) : (RejectIfExists ? CREATE_NEW : OPEN_ALWAYS);
    P = CreateFileA(FullName.c_str(), GENERIC_WRITE, 0, 0, CreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
    if (P == INVALID_HANDLE_VALUE)
#else
    int& P = (int&)Private;
    const int flags = O_WRONLY | O_CREAT | (RejectIfExists ? O_EXCL : 0) | (Truncate ? O_TRUNC : 0);
    const mode_t Mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    P = open(FullName.c_str(), flags, Mode);
    if (P == -1)
#endif
    {
        size_t i = 0;
        for (;;)
        {
            i = FullName.find_first_of("/\\", i + 1);
            if (i == (size_t)-1)
                break;
            string t = FullName.substr(0, i);
            if (access(t.c_str(), 0))
            {
                #if defined(_WIN32) || defined(_WINDOWS)
                if (mkdir(t.c_str()))
                #else
                if (mkdir(t.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
                #endif
                {
                    Private = (void*)-1;
                    return Error_CreateDirectory;
                }
            }
        }
#if defined(_WIN32) || defined(_WINDOWS)
        P = CreateFileA(FullName.c_str(), GENERIC_WRITE, 0, 0, CreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
        if (P == INVALID_HANDLE_VALUE)
#else
        P = open(FullName.c_str(), flags, Mode);
        if (P == -1)
#endif
        {
            Private = (void*)-1;
            return (access(FullName.c_str(), 0)) ? (RejectIfExists ? Error_FileCreate : Error_FileWrite) : Error_FileAlreadyExists;
        }
    }

    OutputFileName = OutputFileName_Source;
    return OK;
}

//---------------------------------------------------------------------------
// file

file::return_value file::Write(const uint8_t* Data, size_t Size)
{
    // Handle size of 0
    if (!Size)
        return OK;

    // Check that a file is open
    size_t Offset = 0;
#if defined(_WIN32) || defined(_WINDOWS)
    HANDLE& P = (HANDLE&)Private;
    BOOL Result;
    while (Offset < Size)
    {
        DWORD BytesWritten;
        DWORD Size_Temp;
        if (Size - Offset >= (DWORD)-1) // WriteFile() accepts only DWORDs
            Size_Temp = (DWORD)-1;
        else
            Size_Temp = (DWORD)(Size - Offset);
        Result = WriteFile(P, Data + Offset, Size_Temp, &BytesWritten, NULL);
        if (BytesWritten == 0 || Result == FALSE)
            break;
        Offset += BytesWritten;
    }
    if (Result == FALSE || Offset < Size)
#else
    int& P = (int&)Private;
    ssize_t BytesWritten;
    while (Offset < Size)
    {
        size_t Size_Temp = Size - Offset;
        BytesWritten = write(P, Data, Size_Temp);
        if (BytesWritten == 0 || BytesWritten == -1)
            break;
        Offset += (size_t)BytesWritten;
    }
    if (BytesWritten == -1 || Offset < Size)
#endif
    {
        return Error_FileWrite;
    }

    return OK;
}

//---------------------------------------------------------------------------
// file

file::return_value file::Seek(int64_t Offset, seek_value Method)
{
    if (Private == (void*)-1)
        return Error_FileCreate;
        
#if defined(_WIN32) || defined(_WINDOWS)
    HANDLE& P = (HANDLE&)Private;
    LARGE_INTEGER Offset2;
    Offset2.QuadPart = Offset;
    if (SetFilePointerEx(P, Offset2, NULL, Method) == 0)
#else
    int& P = (int&)Private;
    int whence;
    switch (Method)
    {
        case Begin   : whence =SEEK_SET; break;
        case Current : whence =SEEK_CUR; break;
        case End     : whence =SEEK_END; break;
        default      : whence =ios_base::beg;
    }
    if (lseek(P, Offset, whence) == (off_t)-1)
#endif
    {
        return Error_Seek;
    }

    return OK;
}

//---------------------------------------------------------------------------
// file

file::return_value file::SetEndOfFile()
{
    if (Private == (void*)-1)
        return Error_FileCreate;

#if defined(_WIN32) || defined(_WINDOWS)
    HANDLE& P = (HANDLE&)Private;
    if (!::SetEndOfFile(P))
#else
    int& P = (int&)Private;
    off_t length = lseek(P, 0, SEEK_CUR);
    if (length == (off_t)-1)
    {
        return Error_FileWrite;
    }
    if (ftruncate(P, length))
#endif
    {
        return Error_FileWrite;
    }

    return OK;
}

//---------------------------------------------------------------------------
// file

file::return_value file::Close()
{
    if (Private == (void*)-1)
        return OK;
        
#if defined(_WIN32) || defined(_WINDOWS)
    HANDLE& P = (HANDLE&)Private;
    if (CloseHandle(P) == 0)
#else
    int& P = (int&)Private;
    if (close(P))
#endif
    {
        Private = (void*)-1;
        OutputFileName.clear();
        return Error_FileWrite;
    }

    Private = (void*)-1;
    OutputFileName.clear();
    return OK;
}
