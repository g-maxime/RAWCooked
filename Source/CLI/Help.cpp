/*  Copyright (c) MediaArea.net SARL & Reto Kromer.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "CLI/Help.h"
#include "iostream"
using namespace std;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
extern const char* LibraryName;
extern const char* LibraryVersion;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ReturnValue Help(const char* Name)
{
    // Copy of rawcooked.1 manpage, with limit to 80 columns
    cout <<
        "NAME\n"
        "       RAWcooked - encode and decode audio-visual RAW data with Matroska, FFV1\n"
        "       and FLAC\n"
        "\n"
        "SYNOPSIS\n"
        "       rawcooked [option ...] (folder | file ...) [option ...]\n"
        "\n"
        "DESCRIPTION\n"
        "       RAWcooked easily encodes RAW audio-visual sequences into a lossless\n"
        "       video stream, reducing the file size by between one and two thirds.\n"
        "       FFmpeg encodes the audio-visual data into a Matroska container (.mkv)\n"
        "       using the video codec FFV1, and audio codec FLAC. The metadata\n"
        "       accompanying the RAW data is fully preserved, along with additional\n"
        "       sidecar files such as MD5 checksums, LUT or XML if desired. This allows\n"
        "       for the management of these audio-visual file formats in an effective\n"
        "       and transparent way. The lossless Matroska video stream can be played\n"
        "       in VLC or MPV media players, and writing and retrieving from storage\n"
        "       devices such as LTO is significantly quicker.\n"
        "\n"
        "       If you need to use the RAW source in its original form, one line of\n"
        "       code will easily restore it bit-by-bit, faster than retrieving the same\n"
        "       file from LTO tape storage.\n"
        "\n"
        "       folder Every image file within the sequence's folder is encoded into a\n"
        "              single FFV1 video stream, and each audio track within the same\n"
        "              folder is encoded to the FLAC codec. Both the FFV1 and FLAC\n"
        "              contents are then muxed into a single Matroska container (.mkv).\n"
        "              The image filenames must end in a complete number sequence, so\n"
        "              that RAWcooked can parse each image within the sequence in the\n"
        "              correct order.\n"
        "\n"
        "       file   contains RAW data (e.g. a .dpx or .wav file):\n"
        "              Every image within a sequence folder containing the file is\n"
        "              encoded into a single FFV1 video stream, and each audio track\n"
        "              within the folder containing the same file is encoded to the\n"
        "              FLAC codec. Both the FFV1 and FLAC contents are then muxed into\n"
        "              a single Matroska container (.mkv).\n"
        "              The image filenames must end in a complete number sequence. By\n"
        "              entering one file within this sequence RAWcooked will parse\n"
        "              every image in the correct order.\n"
        "\n"
        "       file   is a Matroska container (.mkv):\n"
        "              Decodes the Matroska file back to the original RAW image\n"
        "              sequence, including restoration of the original metadata and\n"
        "              sidecar files. It is important to stress that the encoded files\n"
        "              can be fully decoded, and that this process will create bit-by-\n"
        "              bit identical files to the originals. Not only is the image and\n"
        "              audio content fully restored, but also all enclosed metadata and\n"
        "              all of the file's characteristics. Therefore, an encoded and\n"
        "              decoded RAW file cannot be differentiated from its original.\n"
        "\n"
        "OPTIONS\n"
        "   GENERAL OPTIONS\n"
        "       --help | -h\n"
        "              Displays the help guide.\n"
        "\n"
        "       --version\n"
        "              Tells you which installed version you are using.\n"
        "\n"
        "       --store-license value\n"
        "              Set the license key to value and store on hard drive.\n"
        "              (License is stored in ~/.config/RAWcooked/Config.txt on\n"
        "              Linux/Mac, %APPDATA%/RAWcooked/Config.txt on Windows)\n"
        "\n"
        "       --show-license\n"
        "              Displays information about the installed license.\n"
        "\n"
        "       --attachment-max-size value | -s value\n"
        "              Set maximum size of attachments to value (in bytes).\n"
        "              The default value is 1048576.\n"
        "\n"
        "       --sublicense value\n"
        "              Output a license for a sub-licensee with ID value.\n"
        "\n"
        "       --sublicense-dur value\n"
        "              Duration of the sublicense, in months. End date is the last day\n"
        "              of the month.\n"
        "              The default value is 1.\n"
        "\n"
        "       --display-command | -d\n"
        "              When an external encoder/decoder is used, display the command to\n"
        "              launch instead of just launching it.\n"
        "\n"
        "       --output-name value | -o value\n"
        "              Set the name of the output file or folder to value.\n"
        "              The default output value is opposite to the input. Expect\n"
        "              ${Input}.mkv if the input is a folder, or ${Input}.RAWcooked if\n"
        "              input is a file, such as a DPX.\n"
        "\n"
        "       --output-version value\n"
        "              Set the version of the output..\n"
        "              The default output value is 1.\n"
        "              Other possible value is 2. This is a preview release, this value\n"
        "              should be used used only when you know why you do it.\n"
        "\n"
        "       --bin-name value | -b value\n"
        "              Indicate the name of the encoder to use to value.\n"
        "              The default value is ffmpeg (using the default PATH).\n"
        "\n"
        "       --rawcooked-file-name value | -r value\n"
        "              Set during encoding, or retrieve by decoding, the name of the\n"
        "              RAWcooked reversibility data file to value.\n"
        "              The default name is ${Input}.rawcooked_reversibility_data.\n"
        "              Note: This file is deleted after encoding if the RAWcooked\n"
        "              reversibility data file is embedded in the output Matroska\n"
        "              wrapper during encoding.\n"
        "              Note: Not yet implemented for decoding.\n"
        "\n"
        "       --quiet\n"
        "              Do not show information related to RAWcooked.\n"
        "              External encoder or decoder may need an additional option.\n"
        "\n"
        "       -y     Automatic yes to prompts.\n"
        "              Assume yes in answer to all prompts, and run non-interactively.\n"
        "\n"
        "       -n     Automatic no to prompts.\n"
        "              Assume no as answer to all prompts, and run non-interactively.\n"
        "\n"
        "       --log-name value\n"
        "              Set the name of the RAWcooked log file written after\n"
        "              successful processing.\n"
        "\n"
        "   ACTIONS\n"
        "       --all  Same as --info --conch --decode --encode --hash --coherency\n"
        "              --check-padding --check --accept-gaps (see below)\n"
        "\n"
        "       --none Same as --no-info --no-conch --no-decode --no-encode --no-hash\n"
        "              --no-coherency --quick-check-padding --quick-check (see below)\n"
        "\n"
        "       --check\n"
        "              Check that the encoded file can be correctly decoded.\n"
        "              If input is raw content, encode then check that output would be\n"
        "              same as the input content.\n"
        "              If input is compressed content, check that output would be same\n"
        "              as the original content.\n"
        "              Disables decoding.\n"
        "\n"
        "       --quick-check\n"
        "              Run quick coherency checks of the encoded file. Allows user to\n"
        "              check that the file seems healthy without the additional time\n"
        "              taken to process the full check command.\n"
        "              Is ignored in case of compressed content.\n"
        "              This is the default, but may change in the future.\n"
        "\n"
        "       --no-check\n"
        "              Don't run any checks (see above).\n"
        "              This is the default, but may change in the future.\n"
        "\n"
        "       --info Provides extra information about the compressed file, for\n"
        "              example the presence of hashes for the raw data.\n"
        "              Disables encoding and decoding.\n"
        "\n"
        "       --no-info\n"
        "              Don't provide extra information (see above).\n"
        "              This is the default, but may change in the future.\n"
        "\n"
        "       --check-padding\n"
        "              Runs padding checks for DPX files that have no zero padding.\n"
        "              Data found in the padding is stored in the RAWcooked\n"
        "              reversibility file. Be aware check function can be demanding of\n"
        "              time and processor usage.\n"
        "              It is a slower process but guarantees reversibility.\n"
        "\n"
        "       --quick-check-padding\n"
        "              Switch to --check-padding or --no-check-padding depending on\n"
        "              what is found in the first image.\n"
        "              The program will stop with an error code if --check is not used\n"
        "              at the same time and zero-padding bits are in the content,\n"
        "              asking to choose what to do.\n"
        "              This is the default, but may change in the future.\n"
        "\n"
        "       --no-check-padding\n"
        "              Do not run padding checks, as they are demanding of time and\n"
        "              processor usage.\n"
        "              This method is quicker, but be aware it may lead to partial\n"
        "              reversibility with files that do no conform.\n"
        "\n"
        "       --coherency\n"
        "              Checks that the package and contents are coherent. For example,\n"
        "              is the audio file duration the same as the image sequence\n"
        "              duration, or are there gaps in the sequence numbering.\n"
        "              This is currently partially implemented.\n"
        "              This is default, but may change in the future.\n"
        "\n"
        "       --no-coherency\n"
        "              Do not carry out coherency check (see above).\n"
        "\n"
        "       --conch\n"
        "              Conformance check of the format, effective only when format is\n"
        "              supported.\n"
        "              This is currently partially implemented for DPX.\n"
        "              Disable encoding and decoding.\n"
        "\n"
        "       --no-conch\n"
        "              Do not carry out conformance check (see above).\n"
        "              This is default, but may change in the future.\n"
        "\n"
        "       --decode\n"
        "              Encode a compressed stream into audio-visual RAW data.\n"
        "              This is default.\n"
        "\n"
        "       --no-decode\n"
        "              Do not carry out decode (see above).\n"
        "\n"
        "       --encode\n"
        "              Encode audio-visual RAW data into a compressed stream.\n"
        "              This is default.\n"
        "\n"
        "       --no-encode\n"
        "              Do not carry out encode (see above).\n"
        "\n"
        "       --hash Computes the hash of audio-visual RAW data files.\n"
        "              During encoding it computes a hash for each file within a source\n"
        "              folder and stores this within the RAWcooked reversibility\n"
        "              metadata for comparison during --check or --check-padding.\n"
        "              During decoding of a matroska with hashes in the metadata the\n"
        "              file is decoded and new hashes generated for the which are then\n"
        "              tested against the source file hashes stored in the metadata.\n"
        "              Any issues raised by this check is considered a decoding error.\n"
        "              This permits a reversibility check without the original files.\n"
        "\n"
        "       --no-hash\n"
        "              Do not compute or test the hash of the file (see above).\n"
        "              This is default, but may change in the future.\n"
        "\n"
        "       --framemd5\n"
        "              Compute the framemd5 of input frames and store it to a sidecar\n"
        "              file.\n"
        "              See FFmpeg framemd5 documentation for more information.\n"
        "\n"
        "       --framemd5-an value\n"
        "              Disable audio streams in framemd5 output.\n"
        "              Imply --framemd5.\n"
        "\n"
        "       --framemd5-name value\n"
        "              Set the name of the framemd5 file to value.\n"
        "              Imply --framemd5.\n"
        "              Default value is ${Input}.framemd5.\n"
        "\n"
        "       --no-framemd5\n"
        "              Do not compute the framemd5 of input frames. (see above).\n"
        "              Is default.\n"
        "\n"
        "       --accept-gaps\n"
        "              Use if there are missing files within the sequence numbering.\n"
        "              RAWcooked creates a concatenated list of all files ensuring the\n"
        "              sequence can be encoded.\n"
        "\n"
        "       --no-accept-gaps\n"
        "              Do not accept-gaps within the sequence numbering. FFmpeg will\n"
        "              fail any encoding attempts where gaps are present.\n"
        "\n"
        "          INPUT RELATED OPTIONS\n"
        "\n"
        "       --file Unlock the compression of files, for example with .dpx or .wav.\n"
        "\n"
        "       -framerate value\n"
        "              Force the video frame rate value to value.\n"
        "              Default frame rate value is found in the image file metadata, if\n"
        "              available. Otherwise it will default to 24.\n"
        "\n"
        "   ENCODING RELATED OPTIONS\n"
        "       -c:a value\n"
        "              Use this command to force the audio encoding format to value:\n"
        "              copy (for example copy PCM to PCM, without modification), FLAC\n"
        "              The default value is FLAC.\n"
        "\n"
        "       -c:v value\n"
        "              Force the video encoding format value: only ffv1 is currently\n"
        "              allowed, which is the default value.\n"
        "\n"
        "       -coder value\n"
        "              If video encoding format is ffv1, set the Coder to value: 0\n"
        "              (Golomb-Rice), 1 (Range Coder), 2 (Range Coder with custom state\n"
        "              transition table).\n"
        "              The default value is 1.\n"
        "\n"
        "       -context value\n"
        "              If the video encoding format is ffv1, set the Context to value:\n"
        "              0 (small), 1 (large).\n"
        "              The default value is 0.\n"
        "\n"
        "       -format value\n"
        "              Set the container format to value: only matroska is currently\n"
        "              allowed, which is the default value.\n"
        "\n"
        "       -g value\n"
        "              If video encoding format is ffv1, set the GOP size to value 1\n"
        "              (generates a strict intra-frame bitstream), 0 (allows adaptable\n"
        "              context model across frames).\n"
        "              The default value is 1. Ensure you leave the setting at 1 for\n"
        "              archival use.\n"
        "\n"
        "       -level value\n"
        "              The video encoding format ffv1 can have Version set to value: 0,\n"
        "              1, 3.\n"
        "              The default value is the latest version 3.\n"
        "\n"
        "       -slicecrc value\n"
        "              If video encoding format is ffv1, you can set the CRC checksum\n"
        "              to value: 0 (CRC checksums off), 1 (CRC checksum on).\n"
        "              The default value is 1.\n"
        "\n"
        "       -slices value\n"
        "              If the video encoding format is ffv1, you can set the\n"
        "              multithreaded encoding slices to value: any integer over 1 (it\n"
        "              is recommended to use a figure divisible by your workstations\n"
        "              CPU core processors such as 2, 4, 6, 9, 16, 24...).\n"
        "              The default value is variable between 16 and 512, depending on\n"
        "              the video frame size and depth.\n"
        "\n"
        "EXAMPLE: Encoding using the --all action\n"
        "       rawcooked --all /path_to_av_raw_data/ This command comprises several\n"
        "       commands into one '--all' (see above) that ensures safe image sequence\n"
        "       encoding steps are taken. Please see individual flag differences to\n"
        "       understand the differences between its use during encoding and\n"
        "       decoding.\n"
        "       It can be used in conjunction with opposing commands. For example if\n"
        "       you want to use this command without --conch, you can add --no-conch\n"
        "       after the --all and the conch command will be skipped.\n"
        "\n"
        "EXAMPLE: Custom encoding with export of console messages to log file\n"
        "       rawcooked --check --coherency --conch --hash --encode -framerate 24\n"
        "       /path_to_av_raw_data/ >> RAWcooked_encoding.log If you want to retain\n"
        "       the console output of the RAWcooked encoding or decoding processes, you\n"
        "       can set the stdout to a separate log file. This option is useful if\n"
        "       you're automating batch encodings and need to assess the log outputs to\n"
        "       make decisions within the logic of your code.\n"
        "\n"
        "EXAMPLE: Decoding using --all action\n"
        "       rawcooked --all <file.mkv> This command works the same as the encoding\n"
        "       of raw audio-visual data, but decodes the Matroska file back to it's\n"
        "       original raw state. Please see individual flag differences (above) to\n"
        "       understand the differences between its use during encoding and\n"
        "       decoding.\n"
        "       It can be used in conjunction with opposing commands. For example if\n"
        "       you want to use this command without --conch, you can add --no-conch\n"
        "       after the --all and the conch command will be skipped.\n"
        "\n"
        "COPYRIGHT\n"
        "       Copyright (c) 2018-2023 MediaArea.net SARL & Reto Kromer\n"
        "\n"
        "LICENSE\n"
        "       RAWcooked is released under a BSD License.\n"
        "\n"
        "DISCLAIMER\n"
        "       RAWcooked is provided \"as is\" without warranty or support of any kind.\n"
        << endl;

  return ReturnValue_OK;
}

//---------------------------------------------------------------------------
ReturnValue Usage(const char* Name)
{
  printf("Usage: \"%s [options] DirectoryName [options]\"\n", Name);
  printf("\"%s --help\" for displaying more information\n", Name);
  printf("or \"man %s\" for displaying the man page\n", Name);

  return ReturnValue_OK;
}

//---------------------------------------------------------------------------
ReturnValue Version()
{
  printf("%s %s\n", LibraryName, LibraryVersion);

  return ReturnValue_OK;
}

//---------------------------------------------------------------------------
const char* GetLibraryName()
{
    return LibraryName;
}

//---------------------------------------------------------------------------
const char* GetLibraryVersion()
{
    return LibraryVersion;
}
