/*
** FAAD2 - Freeware Advanced Audio (AAC) Decoder including SBR decoding
** Copyright (C) 2003-2005 M. Bakker, Nero AG, http://www.nero.com
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** Any non-GPL usage of this software or parts of this software is strictly
** forbidden.
**
** The "appropriate copyright message" mentioned in section 2c of the GPLv2
** must read: "Code from FAAD2 is copyright (c) Nero AG, www.nero.com"
**
** Commercial non-GPL licensing of this software is possible.
** For more info contact Nero AG through Mpeg4AAClicense@nero.com.
**
** $Id: main.c,v 1.85 2008/09/22 17:55:09 menno Exp $
**/

#define LOG_TAG "AAC_DECODE"

#include "aac_decode.h"

#ifndef min
#define min(a,b) ( (a) < (b) ? (a) : (b) )
#endif

#define MAX_CHANNELS 6 /* make this higher to support files with
                          more channels */

//static int quiet = 0;

//static void faad_fprintf(FILE *stream, const char *fmt, ...)
//{
//    va_list ap;
//
//    if (!quiet)
//    {
//        va_start(ap, fmt);
//
//        vfprintf(stream, fmt, ap);
//
//        va_end(ap);
//    }
//}

#define faad_fprintf LOGD

//const unsigned long AacDecode::srates[12] = {
//        96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000,
//        12000, 11025, 8000
//};

//const char * AacDecode::file_ext[7] = {
//        NULL,
//        ".wav",
//        ".aif",
//        ".au",
//        ".au",
//        ".pcm",
//        NULL
//};

AacDecode::AacDecode() : checkBoxNal(false),showAdtsCount(0),countIndex(0)
{
    unsigned long cap = NeAACDecGetCapabilities();

    faad_fprintf(" *********** Ahead Software MPEG-4 AAC Decoder V%s ******************\n\n", FAAD2_VERSION);
    faad_fprintf(" Build: %s", __DATE__);
    faad_fprintf(" Copyright 2002-2004: Ahead Software AG");
    faad_fprintf(" http://www.audiocoding.com");
    if (cap & FIXED_POINT_CAP)
        faad_fprintf(" Fixed point version");
    else
        faad_fprintf(" Floating point version");
    faad_fprintf("\n");
    faad_fprintf(" This program is free software; you can redistribute it and/or modify");
    faad_fprintf(" it under the terms of the GNU General Public License.");
    faad_fprintf("\n");
    faad_fprintf(" **************************************************************************\n\n");

    pAacFrame = new AudioFrame;
    memset(this->progName,0, sizeof(this->progName));
    memcpy(this->progName,"Faad2-Android",strlen("Faad2-Android"));
}

AacDecode::AacDecode(const char* proName) : checkBoxNal(false),showAdtsCount(0),countIndex(0)
{
    unsigned long cap = NeAACDecGetCapabilities();

    faad_fprintf(" *********** Ahead Software MPEG-4 AAC Decoder V%s ******************\n\n", FAAD2_VERSION);
    faad_fprintf(" Build: %s", __DATE__);
    faad_fprintf(" Copyright 2002-2004: Ahead Software AG");
    faad_fprintf(" http://www.audiocoding.com");
    if (cap & FIXED_POINT_CAP)
        faad_fprintf(" Fixed point version");
    else
        faad_fprintf(" Floating point version");
    faad_fprintf("\n");
    faad_fprintf(" This program is free software; you can redistribute it and/or modify");
    faad_fprintf(" it under the terms of the GNU General Public License.");
    faad_fprintf("\n");
    faad_fprintf(" **************************************************************************\n\n");

    pAacFrame = new AudioFrame;
    memset(this->progName,0, sizeof(this->progName));
    memcpy(this->progName,proName,strlen(proName));
}

AacDecode::~AacDecode()
{
    delete pAacFrame;
    pAacFrame = NULL;
    this->pBasicAudioInfoFunc = NULL;
    this->pAacFrameFunc = NULL;
    this->pDecodeProgressFunc = NULL;
}

void AacDecode::setBasicInfoCallback(void (*func)(map<string, string>& containerInfo,map<string, string>& metaDataInfo))
{
    this->pBasicAudioInfoFunc = func;
}

void AacDecode::setCheckBox(bool isCheck,int showNalCount)
{
    this->showAdtsCount = showNalCount;
    this->checkBoxNal = isCheck;
}

void AacDecode::setProgressCallback(void (*func)(int progressPercent))
{
    this->pDecodeProgressFunc = func;
}

void AacDecode::setAacFrameCallback(void (*func)(AudioFrame* aacFrame))
{
    this->pAacFrameFunc = func;
}

int AacDecode::fill_buffer(aac_buffer *b)
{
    int bread;

    if (b->bytes_consumed > 0)
    {
        if (b->bytes_into_buffer)
        {
            memmove((void*)b->buffer, (void*)(b->buffer + b->bytes_consumed),
                b->bytes_into_buffer*sizeof(unsigned char));
        }

        if (!b->at_eof)
        {
            bread = fread((void*)(b->buffer + b->bytes_into_buffer), 1,
                b->bytes_consumed, b->infile);

            if (bread != b->bytes_consumed)
                b->at_eof = 1;

            b->bytes_into_buffer += bread;
        }

        b->bytes_consumed = 0;

        if (b->bytes_into_buffer > 3)
        {
            if (memcmp(b->buffer, "TAG", 3) == 0)
                b->bytes_into_buffer = 0;
        }
        if (b->bytes_into_buffer > 11)
        {
            if (memcmp(b->buffer, "LYRICSBEGIN", 11) == 0)
                b->bytes_into_buffer = 0;
        }
        if (b->bytes_into_buffer > 8)
        {
            if (memcmp(b->buffer, "APETAGEX", 8) == 0)
                b->bytes_into_buffer = 0;
        }
    }

    return 1;
}

void AacDecode::advance_buffer(aac_buffer *b, int bytes)
{
    b->file_offset += bytes;
    b->bytes_consumed = bytes;
    b->bytes_into_buffer -= bytes;
	if (b->bytes_into_buffer < 0)
		b->bytes_into_buffer = 0;
}

static int adts_sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350,0,0,0};

int AacDecode::adts_parse(aac_buffer *b, int *bitrate, float *length)
{
    int frames, frame_length;
    int t_framelength = 0;
    int samplerate;
    float frames_per_sec, bytes_per_frame;

    /* Read all frames to ensure correct time and bitrate */
    for (frames = 0; /* */; frames++)
    {
        fill_buffer(b);

        if (b->bytes_into_buffer > 7)
        {
            /* check syncword */
            if (!((b->buffer[0] == 0xFF)&&((b->buffer[1] & 0xF6) == 0xF0)))
                break;

            if (frames == 0)
                samplerate = adts_sample_rates[(b->buffer[2]&0x3c)>>2];

            frame_length = ((((unsigned int)b->buffer[3] & 0x3)) << 11)
                | (((unsigned int)b->buffer[4]) << 3) | (b->buffer[5] >> 5);

            t_framelength += frame_length;

            if (frame_length > b->bytes_into_buffer)
                break;

            advance_buffer(b, frame_length);
        } else {
            break;
        }
    }

    frames_per_sec = (float)samplerate/1024.0f;
    if (frames != 0)
        bytes_per_frame = (float)t_framelength/(float)(frames*1000);
    else
        bytes_per_frame = 0;
    *bitrate = (int)(8. * bytes_per_frame * frames_per_sec + 0.5);
    if (frames_per_sec != 0)
        *length = (float)frames/frames_per_sec;
    else
        *length = 1;

    return 1;
}

uint32_t AacDecode::read_callback(void *user_data, void *buffer, uint32_t length)
{
    return fread(buffer, 1, length, (FILE*)user_data);
}

uint32_t AacDecode::seek_callback(void *user_data, uint64_t position)
{
    return fseek((FILE*)user_data, position, SEEK_SET);
}

long AacDecode::aacChannelConfig2wavexChannelMask(NeAACDecFrameInfo *hInfo)
{
    if (hInfo->channels == 6 && hInfo->num_lfe_channels)
    {
        return SPEAKER_FRONT_LEFT + SPEAKER_FRONT_RIGHT +
            SPEAKER_FRONT_CENTER + SPEAKER_LOW_FREQUENCY +
            SPEAKER_BACK_LEFT + SPEAKER_BACK_RIGHT;
    } else {
        return 0;
    }
}

const char * AacDecode::position2string(int position)
{
    switch (position)
    {
    case FRONT_CHANNEL_CENTER: return "Center front";
    case FRONT_CHANNEL_LEFT:   return "Left front";
    case FRONT_CHANNEL_RIGHT:  return "Right front";
    case SIDE_CHANNEL_LEFT:    return "Left side";
    case SIDE_CHANNEL_RIGHT:   return "Right side";
    case BACK_CHANNEL_LEFT:    return "Left back";
    case BACK_CHANNEL_RIGHT:   return "Right back";
    case BACK_CHANNEL_CENTER:  return "Center back";
    case LFE_CHANNEL:          return "LFE";
    case UNKNOWN_CHANNEL:      return "Unknown";
    default: return "";
    }

    return "";
}

void AacDecode::print_channel_info(NeAACDecFrameInfo *frameInfo)
{
    /* print some channel info */
    int i;
    long channelMask = aacChannelConfig2wavexChannelMask(frameInfo);

    faad_fprintf("  ---------------------\n");
    if (frameInfo->num_lfe_channels > 0)
    {
        faad_fprintf(" | Config: %2d.%d Ch     |", frameInfo->channels-frameInfo->num_lfe_channels, frameInfo->num_lfe_channels);
    } else {
        faad_fprintf(" | Config: %2d Ch       |", frameInfo->channels);
    }
    if (channelMask)
        faad_fprintf(" WARNING: channels are reordered according to\n");
    else
        faad_fprintf("\n");
    faad_fprintf("  ---------------------");
    if (channelMask)
        faad_fprintf("  MS defaults defined in WAVE_FORMAT_EXTENSIBLE\n");
    else
        faad_fprintf("\n");
    faad_fprintf(" | Ch |    Position    |\n");
    faad_fprintf("  ---------------------\n");
    for (i = 0; i < frameInfo->channels; i++)
    {
        faad_fprintf(" | %.2d | %-14s |\n", i, position2string((int)frameInfo->channel_position[i]));
    }
    faad_fprintf("  ---------------------\n");
    faad_fprintf("\n");
}

int AacDecode::FindAdtsSRIndex(int sr)
{
    int i;

    for (i = 0; i < 16; i++)
    {
        if (sr == adts_sample_rates[i])
            return i;
    }
    return 16 - 1;
}

unsigned char * AacDecode::MakeAdtsHeader(int *dataSize, NeAACDecFrameInfo *hInfo, int old_format)
{
    unsigned char *data;
    int profile = (hInfo->object_type - 1) & 0x3;
    int sr_index = ((hInfo->sbr == SBR_UPSAMPLED) || (hInfo->sbr == NO_SBR_UPSAMPLED)) ?
        FindAdtsSRIndex(hInfo->samplerate / 2) : FindAdtsSRIndex(hInfo->samplerate);
    int skip = (old_format) ? 8 : 7;
    int framesize = skip + hInfo->bytesconsumed;

    if (hInfo->header_type == ADTS)
        framesize -= skip;

    *dataSize = 7;

    data = (unsigned char *)malloc(*dataSize * sizeof(unsigned char));
    memset(data, 0, *dataSize * sizeof(unsigned char));

    data[0] += 0xFF; /* 8b: syncword */

    data[1] += 0xF0; /* 4b: syncword */
    /* 1b: mpeg id = 0 */
    /* 2b: layer = 0 */
    data[1] += 1; /* 1b: protection absent */

    data[2] += ((profile << 6) & 0xC0); /* 2b: profile */
    data[2] += ((sr_index << 2) & 0x3C); /* 4b: sampling_frequency_index */
    /* 1b: private = 0 */
    data[2] += ((hInfo->channels >> 2) & 0x1); /* 1b: channel_configuration */

    data[3] += ((hInfo->channels << 6) & 0xC0); /* 2b: channel_configuration */
    /* 1b: original */
    /* 1b: home */
    /* 1b: copyright_id */
    /* 1b: copyright_id_start */
    data[3] += ((framesize >> 11) & 0x3); /* 2b: aac_frame_length */

    data[4] += ((framesize >> 3) & 0xFF); /* 8b: aac_frame_length */

    data[5] += ((framesize << 5) & 0xE0); /* 3b: aac_frame_length */
    data[5] += ((0x7FF >> 6) & 0x1F); /* 5b: adts_buffer_fullness */

    data[6] += ((0x7FF << 2) & 0x3F); /* 6b: adts_buffer_fullness */
    /* 2b: num_raw_data_blocks */

    return data;
}

void AacDecode::usage(void)
{
    faad_fprintf("\nUsage:\n");
    faad_fprintf("%s [options] infile.aac\n", progName);
    faad_fprintf("Options:\n");
    faad_fprintf(" -h    Shows this help screen.\n");
    faad_fprintf(" -i    Shows info about the input file.\n");
    faad_fprintf(" -a X  Write MPEG-4 AAC ADTS output file.\n");
    faad_fprintf(" -t    Assume old ADTS format.\n");
    faad_fprintf(" -o X  Set output filename.\n");
    faad_fprintf(" -f X  Set output format. Valid values for X are:\n");
    faad_fprintf("        1:  Microsoft WAV format (default).\n");
    faad_fprintf("        2:  RAW PCM data.\n");
    faad_fprintf(" -b X  Set output sample format. Valid values for X are:\n");
    faad_fprintf("        1:  16 bit PCM data (default).\n");
    faad_fprintf("        2:  24 bit PCM data.\n");
    faad_fprintf("        3:  32 bit PCM data.\n");
    faad_fprintf("        4:  32 bit floating point data.\n");
    faad_fprintf("        5:  64 bit floating point data.\n");
    faad_fprintf(" -s X  Force the samplerate to X (for RAW files).\n");
    faad_fprintf(" -l X  Set object type. Supported object types:\n");
    faad_fprintf("        1:  Main object type.\n");
    faad_fprintf("        2:  LC (Low Complexity) object type.\n");
    faad_fprintf("        4:  LTP (Long Term Prediction) object type.\n");
    faad_fprintf("        23: LD (Low Delay) object type.\n");
    faad_fprintf(" -d    Down matrix 5.1 to 2 channels\n");
    faad_fprintf(" -w    Write output to stdio instead of a file.\n");
    faad_fprintf(" -g    Disable gapless decoding.\n");
    faad_fprintf(" -q    Quiet - suppresses status messages.\n");
    faad_fprintf("Example:\n");
    faad_fprintf("       %s infile.aac\n", progName);
    faad_fprintf("       %s infile.mp4\n", progName);
    faad_fprintf("       %s -o outfile.wav infile.aac\n", progName);
    faad_fprintf("       %s -w infile.aac > outfile.wav\n", progName);
    faad_fprintf("       %s -a outfile.aac infile.aac\n", progName);
    return;
}

int AacDecode::decodeAACfile(char *aacfile, char *sndfile, char *adts_fn, int to_stdout,
                  int def_srate, int object_type, int outputFormat, int fileType,
                  int downMatrix, int infoOnly, int adts_out, int old_format,
                  float *song_length)
{
    int tagsize;
    unsigned long samplerate;
    unsigned char channels;
    void *sample_buffer;

    audio_file *aufile;

    FILE *adtsFile;
    unsigned char *adtsData;
    int adtsDataSize;

    NeAACDecHandle hDecoder;
    NeAACDecFrameInfo frameInfo;
    NeAACDecConfigurationPtr config;

    char percents[200];
    int percent, old_percent = -1;
    int bread, fileread;
    int header_type = 0;
    int bitrate = 0;
    float length = 0;

    int first_time = 1;

    aac_buffer b;

    memset(&b, 0, sizeof(aac_buffer));

    if (adts_out)
    {
        adtsFile = fopen(adts_fn, "wb");
        if (adtsFile == NULL)
        {
            faad_fprintf("%s: Error opening file: %s\n", __FUNCTION__,adts_fn);
            return 1;
        }
    }

    b.infile = fopen(aacfile, "rb");
    if (b.infile == NULL)
    {
        /* unable to open file */
        faad_fprintf("%s: Error opening file: %s\n", __FUNCTION__,aacfile);
        return 1;
    }

    fseek(b.infile, 0, SEEK_END);
    fileread = ftell(b.infile);
    fseek(b.infile, 0, SEEK_SET);

    if (!(b.buffer = (unsigned char*)malloc(FAAD_MIN_STREAMSIZE*MAX_CHANNELS)))
    {
        faad_fprintf("%s: Memory allocation error\n",__FUNCTION__);
        return 0;
    }
    memset(b.buffer, 0, FAAD_MIN_STREAMSIZE*MAX_CHANNELS);

    bread = fread(b.buffer, 1, FAAD_MIN_STREAMSIZE*MAX_CHANNELS, b.infile);
    b.bytes_into_buffer = bread;
    b.bytes_consumed = 0;
    b.file_offset = 0;

    if (bread != FAAD_MIN_STREAMSIZE*MAX_CHANNELS)
        b.at_eof = 1;

    tagsize = 0;
    if (!memcmp(b.buffer, "ID3", 3))
    {
        /* high bit is not used */
        tagsize = (b.buffer[6] << 21) | (b.buffer[7] << 14) |
            (b.buffer[8] <<  7) | (b.buffer[9] <<  0);

        tagsize += 10;
        advance_buffer(&b, tagsize);
        fill_buffer(&b);
    }

    hDecoder = NeAACDecOpen();

    /* Set the default object type and samplerate */
    /* This is useful for RAW AAC files */
    config = NeAACDecGetCurrentConfiguration(hDecoder);
    if (def_srate)
        config->defSampleRate = def_srate;
    config->defObjectType = object_type;
    config->outputFormat = outputFormat;
    config->downMatrix = downMatrix;
    config->useOldADTSFormat = old_format;
    //config->dontUpSampleImplicitSBR = 1;
    NeAACDecSetConfiguration(hDecoder, config);

    /* get AAC infos for printing */
    header_type = 0;
    if ((b.buffer[0] == 0xFF) && ((b.buffer[1] & 0xF6) == 0xF0))
    {
        adts_parse(&b, &bitrate, &length);
        fseek(b.infile, tagsize, SEEK_SET);

        bread = fread(b.buffer, 1, FAAD_MIN_STREAMSIZE*MAX_CHANNELS, b.infile);
        if (bread != FAAD_MIN_STREAMSIZE*MAX_CHANNELS)
            b.at_eof = 1;
        else
            b.at_eof = 0;
        b.bytes_into_buffer = bread;
        b.bytes_consumed = 0;
        b.file_offset = tagsize;

        header_type = 1;
    } else if (memcmp(b.buffer, "ADIF", 4) == 0) {
        int skip_size = (b.buffer[4] & 0x80) ? 9 : 0;
        bitrate = ((unsigned int)(b.buffer[4 + skip_size] & 0x0F)<<19) |
            ((unsigned int)b.buffer[5 + skip_size]<<11) |
            ((unsigned int)b.buffer[6 + skip_size]<<3) |
            ((unsigned int)b.buffer[7 + skip_size] & 0xE0);

        length = (float)fileread;
        if (length != 0)
        {
            length = ((float)length*8.f)/((float)bitrate) + 0.5f;
        }

        bitrate = (int)((float)bitrate/1000.0f + 0.5f);

        header_type = 2;
    }

    *song_length = length;

    fill_buffer(&b);
    if ((bread = NeAACDecInit(hDecoder, b.buffer,
        b.bytes_into_buffer, &samplerate, &channels)) < 0)
    {
        /* If some error initializing occured, skip the file */
        faad_fprintf("%s: Error initializing decoder library.\n",__FUNCTION__);
        if (b.buffer)
            free(b.buffer);
        NeAACDecClose(hDecoder);
        fclose(b.infile);
        return 1;
    }
    advance_buffer(&b, bread);
    fill_buffer(&b);

    /* print AAC file info */
    faad_fprintf("%s: %s file info:", __FUNCTION__,aacfile);
    switch (header_type)
    {
    case 0:
        faad_fprintf("RAW\n");
        break;
    case 1:
        faad_fprintf("ADTS, %.3f sec, %d kbps, %lu Hz\n",
            length, bitrate, samplerate);
        break;
    case 2:
        faad_fprintf("ADIF, %.3f sec, %d kbps, %lu Hz\n",
            length, bitrate, samplerate);
        break;
    }

    if (infoOnly)
    {
        NeAACDecClose(hDecoder);
        fclose(b.infile);
        if (b.buffer)
            free(b.buffer);
        return 0;
    }

    do
    {
        sample_buffer = NeAACDecDecode(hDecoder, &frameInfo,
            b.buffer, b.bytes_into_buffer);

        if (adts_out == 1)
        {
            int skip = (old_format) ? 8 : 7;
            adtsData = MakeAdtsHeader(&adtsDataSize, &frameInfo, old_format);

            /* write the adts header */
            fwrite(adtsData, 1, adtsDataSize, adtsFile);

            /* write the frame data */
            if (frameInfo.header_type == ADTS)
                fwrite(b.buffer + skip, 1, frameInfo.bytesconsumed - skip, adtsFile);
            else
                fwrite(b.buffer, 1, frameInfo.bytesconsumed, adtsFile);
        }

        /* update buffer indices */
        advance_buffer(&b, frameInfo.bytesconsumed);

        if (frameInfo.error > 0)
        {
            faad_fprintf("Error: %s",
                NeAACDecGetErrorMessage(frameInfo.error));
        }

        /* open the sound file now that the number of channels are known */
        if (first_time && !frameInfo.error)
        {
            /* print some channel info */
            print_channel_info(&frameInfo);
            metaDataInfo.clear();
            //输出信息
            //-------------
            int tempnum = 0;
            char tempstr[256] = {0};
            //声道数
            sprintf(tempstr, "%d", frameInfo.channels);
            metaDataInfo.insert(make_pair("Channels", tempstr));
            //采样率
            sprintf(tempstr, "%d", frameInfo.samplerate);
            metaDataInfo.insert(make_pair("Sample Rate", tempstr));
            //AAC类型-------
            tempnum = frameInfo.object_type;
            switch (tempnum) {
                case MAIN:
                    sprintf(tempstr, "%s","MAIN");
                    break;
                case LC:
                    sprintf(tempstr, "%s","LC");
                    break;
                case SSR:
                    sprintf(tempstr, "%s","SSR");
                    break;
                case LTP:
                    sprintf(tempstr, "%s","LTP");
                    break;
                case HE_AAC:
                    sprintf(tempstr, "%s","HE_AAC");
                    break;
                case ER_LC:
                    sprintf(tempstr, "%s","ER_LC");
                    break;
                case ER_LTP:
                    sprintf(tempstr, "%s","ER_LTP");
                    break;
                case LD:
                    sprintf(tempstr, "%s","LD");
                    break;
                case DRM_ER_LC:
                    sprintf(tempstr, "%s","DRM_ER_LC");
                    break;
            }
            metaDataInfo.insert(make_pair("Object Type", tempstr));
            //Header Type
            tempnum = frameInfo.header_type;
            switch (tempnum) {
                case RAW:
                    sprintf(tempstr, "%s","RAW");
                    break;
                case ADIF:
                    sprintf(tempstr, "%s","ADIF");
                    break;
                case ADTS:
                    sprintf(tempstr, "%s","ADTS");
                    break;
                case LATM:
                    sprintf(tempstr, "%s","LATM");
                    break;
            }
            metaDataInfo.insert(make_pair("Header Type", tempstr));

            //SBR
            tempnum = frameInfo.sbr;
            switch (tempnum) {
                case NO_SBR:
                    sprintf(tempstr, "%s","off");
                    break;
                case SBR_UPSAMPLED:
                    sprintf(tempstr, "%s","on;upsample");
                    break;
                case SBR_DOWNSAMPLED:
                    sprintf(tempstr, "%s","on;downsampled");
                    break;
                case NO_SBR_UPSAMPLED:
                    sprintf(tempstr, "%s","off;upsampled");
                    break;
            }
            metaDataInfo.insert(make_pair("SBR", tempstr));

            //PS
            tempnum = frameInfo.ps;
            switch (tempnum) {
                case 0:
                    sprintf(tempstr, "%s","off");
                    break;
                case 1:
                    sprintf(tempstr, "%s","on");
                    break;
            }
            metaDataInfo.insert(make_pair("PS", tempstr));
            (*pBasicAudioInfoFunc)(containerInfo,metaDataInfo);

            if (!adts_out)
            {
                /* open output file */
                if (!to_stdout)
                {
                    aufile = open_audio_file(sndfile, frameInfo.samplerate, frameInfo.channels,
                        outputFormat, fileType, aacChannelConfig2wavexChannelMask(&frameInfo));
                } else {
                    aufile = open_audio_file((char*)"-", frameInfo.samplerate, frameInfo.channels,
                        outputFormat, fileType, aacChannelConfig2wavexChannelMask(&frameInfo));
                }
                if (aufile == NULL)
                {
                    if (b.buffer)
                        free(b.buffer);
                    NeAACDecClose(hDecoder);
                    fclose(b.infile);
                    return 0;
                }
            } else {
                faad_fprintf("Writing output MPEG-4 AAC ADTS file.\n\n");
            }
            first_time = 0;
        }

        pAacFrame->size = frameInfo.bytesconsumed;
        pAacFrame->sampleNum = frameInfo.samples;
        //AAC每帧数据对应的采样数为1024，对应的每帧时间间隔(ms)为 1024*1000 / 采样率。
        //如48K采样每帧递增21；44.1K采样每帧递增23
        pAacFrame->duration = 1024*1000/frameInfo.samplerate;

        percent = min((int)(b.file_offset*100)/fileread, 100);

        if(checkBoxNal && countIndex++ < showAdtsCount)
            (*pAacFrameFunc)(pAacFrame);
        if(!checkBoxNal)
            (*pAacFrameFunc)(pAacFrame);

        if (percent > old_percent)
        {
            old_percent = percent;
            sprintf(percents, "%d%% decoding %s.", percent, aacfile);
            faad_fprintf("%s\r", percents);
            pDecodeProgressFunc(percent);
#ifdef _WIN32
            SetConsoleTitle(percents);
#endif
        }

        if ((frameInfo.error == 0) && (frameInfo.samples > 0) && (!adts_out))
        {
            if (write_audio_file(aufile, sample_buffer, frameInfo.samples, 0) == 0)
                break;
		}

        /* fill buffer */
        fill_buffer(&b);

        if (b.bytes_into_buffer == 0)
            sample_buffer = NULL; /* to make sure it stops now */

    } while (sample_buffer != NULL);

    NeAACDecClose(hDecoder);

    if (adts_out == 1)
    {
        fclose(adtsFile);
    }

    fclose(b.infile);

    if (!first_time && !adts_out)
        close_audio_file(aufile);

    if (b.buffer)
        free(b.buffer);

    return frameInfo.error;
}

int AacDecode::GetAACTrack(mp4ff_t *infile)
{
    /* find AAC track */
    int i, rc;
    int numTracks = mp4ff_total_tracks(infile);

    for (i = 0; i < numTracks; i++)
    {
        unsigned char *buff = NULL;
        int buff_size = 0;
        mp4AudioSpecificConfig mp4ASC;

        mp4ff_get_decoder_config(infile, i, &buff, (unsigned int *)&buff_size);

        if (buff)
        {
            rc = NeAACDecAudioSpecificConfig(buff, buff_size, &mp4ASC);
            free(buff);

            if (rc < 0)
                continue;
            return i;
        }
    }

    /* can't decode this */
    return -1;
}

int AacDecode::decodeMP4file(char *mp4file, char *sndfile, char *adts_fn, int to_stdout,
                  int outputFormat, int fileType, int downMatrix, int noGapless,
                  int infoOnly, int adts_out, float *song_length)
{
    int track;
    unsigned long samplerate;
    unsigned char channels;
    void *sample_buffer;

    mp4ff_t *infile;
    long sampleId, numSamples;

    audio_file *aufile;

    FILE *mp4File;
    FILE *adtsFile;
    unsigned char *adtsData;
    int adtsDataSize;

    NeAACDecHandle hDecoder;
    NeAACDecConfigurationPtr config;
    NeAACDecFrameInfo frameInfo;
    mp4AudioSpecificConfig mp4ASC;

    unsigned char *buffer;
    int buffer_size;

    char percents[200];
    int percent, old_percent = -1;

    int first_time = 1;

    /* for gapless decoding */
    unsigned int useAacLength = 1;
    unsigned int initial = 1;
    unsigned int framesize;
    unsigned long timescale;


    /* initialise the callback structure */
    mp4ff_callback_t *mp4cb = (mp4ff_callback_t *)malloc(sizeof(mp4ff_callback_t));

    mp4File = fopen(mp4file, "rb");
    mp4cb->read = read_callback;
    mp4cb->seek = seek_callback;
    mp4cb->user_data = mp4File;


    hDecoder = NeAACDecOpen();

    /* Set configuration */
    config = NeAACDecGetCurrentConfiguration(hDecoder);
    config->outputFormat = outputFormat;
    config->downMatrix = downMatrix;
    //config->dontUpSampleImplicitSBR = 1;
    NeAACDecSetConfiguration(hDecoder, config);

    if (adts_out)
    {
        adtsFile = fopen(adts_fn, "wb");
        if (adtsFile == NULL)
        {
            faad_fprintf("%s: Error opening file: %s\n", __FUNCTION__,adts_fn);
            return 1;
        }
    }

    infile = mp4ff_open_read(mp4cb);
    if (!infile)
    {
        /* unable to open file */
        faad_fprintf("%s: Error opening file: %s\n", __FUNCTION__,mp4file);
        return 1;
    }

    if ((track = GetAACTrack(infile)) < 0)
    {
        faad_fprintf("%s: Unable to find correct AAC sound track in the MP4 file.\n",__FUNCTION__);
        NeAACDecClose(hDecoder);
        mp4ff_close(infile);
        free(mp4cb);
        fclose(mp4File);
        return 1;
    }

    buffer = NULL;
    buffer_size = 0;
    mp4ff_get_decoder_config(infile, track, &buffer, (unsigned int *)&buffer_size);

    if(NeAACDecInit2(hDecoder, buffer, buffer_size,
                    &samplerate, &channels) < 0)
    {
        /* If some error initializing occured, skip the file */
        faad_fprintf("%s: Error initializing decoder library.\n",__FUNCTION__);
        NeAACDecClose(hDecoder);
        mp4ff_close(infile);
        free(mp4cb);
        fclose(mp4File);
        return 1;
    }

    timescale = mp4ff_time_scale(infile, track);
    framesize = 1024;
    useAacLength = 0;

    if (buffer)
    {
        if (NeAACDecAudioSpecificConfig(buffer, buffer_size, &mp4ASC) >= 0)
        {
            if (mp4ASC.frameLengthFlag == 1) framesize = 960;
            if (mp4ASC.sbr_present_flag == 1) framesize *= 2;
        }
        free(buffer);
    }

    /* print some mp4 file info */
    faad_fprintf("%s: %s file info:\n\n", __FUNCTION__,mp4file);
    {
        char *tag = NULL, *item = NULL;
        int k, j;
        const char *ot[6] = { "NULL", "MAIN AAC", "LC AAC", "SSR AAC", "LTP AAC", "HE AAC" };
        long samples = mp4ff_num_samples(infile, track);
        float f = 1024.0;
        float seconds;
        if (mp4ASC.sbr_present_flag == 1)
        {
            f = f * 2.0;
        }
        seconds = (float)samples*(float)(f-1.0)/(float)mp4ASC.samplingFrequency;

        *song_length = seconds;

        faad_fprintf("%s\t%.3f secs, %d ch, %lu Hz\n\n", ot[(mp4ASC.objectTypeIndex > 5)?0:mp4ASC.objectTypeIndex],
            seconds, mp4ASC.channelsConfiguration, mp4ASC.samplingFrequency);

#define PRINT_MP4_METADATA
#ifdef PRINT_MP4_METADATA
        j = mp4ff_meta_get_num_items(infile);
        for (k = 0; k < j; k++)
        {
            if (mp4ff_meta_get_by_index(infile, k, &item, &tag))
            {
                if (item != NULL && tag != NULL)
                {
                    faad_fprintf("%s: %s\n", item, tag);
                    free(item); item = NULL;
                    free(tag); tag = NULL;
                }
            }
        }
        if (j > 0) faad_fprintf("\n");
#endif
    }

    if (infoOnly)
    {
        NeAACDecClose(hDecoder);
        mp4ff_close(infile);
        free(mp4cb);
        fclose(mp4File);
        return 0;
    }

    numSamples = mp4ff_num_samples(infile, track);

    //输出MP4信息
    //-------------
    char tempstr[256] = {0};
    //objectTypeIndex
    sprintf(tempstr, "%d", mp4ASC.objectTypeIndex);
    containerInfo.insert(make_pair("objectTypeIndex", tempstr));
    //samplingFrequencyIndex
    sprintf(tempstr, "%d", mp4ASC.samplingFrequencyIndex);
    containerInfo.insert(make_pair("samplingFrequencyIndex", tempstr));
    //samplingFrequency
    sprintf(tempstr, "%d", mp4ASC.samplingFrequency);
    containerInfo.insert(make_pair("samplingFrequency", tempstr));
    //channelsConfiguration
    sprintf(tempstr, "%d",mp4ASC.channelsConfiguration);
    containerInfo.insert(make_pair("channelsConfiguration", tempstr));
    //-----------

    for (sampleId = 0; sampleId < numSamples; sampleId++)
    {
        int rc;
        long dur;
        unsigned int sample_count;
        unsigned int delay = 0;

        /* get acces unit from MP4 file */
        buffer = NULL;
        buffer_size = 0;

        dur = mp4ff_get_sample_duration(infile, track, sampleId);
        rc = mp4ff_read_sample(infile, track, sampleId, &buffer,  (unsigned int *)&buffer_size);
        if (rc == 0)
        {
            faad_fprintf("%s: Reading from MP4 file failed.\n",__FUNCTION__);
            NeAACDecClose(hDecoder);
            mp4ff_close(infile);
            free(mp4cb);
            fclose(mp4File);
            return 1;
        }

        sample_buffer = NeAACDecDecode(hDecoder, &frameInfo, buffer, buffer_size);

        if (adts_out == 1)
        {
            adtsData = MakeAdtsHeader(&adtsDataSize, &frameInfo, 0);

            /* write the adts header */
            fwrite(adtsData, 1, adtsDataSize, adtsFile);

            fwrite(buffer, 1, frameInfo.bytesconsumed, adtsFile);
        }

        if (buffer) free(buffer);

        if (!noGapless)
        {
            if (sampleId == 0) dur = 0;

            if (useAacLength || (timescale != samplerate)) {
                sample_count = frameInfo.samples;
            } else {
                sample_count = (unsigned int)(dur * frameInfo.channels);
                if (sample_count > frameInfo.samples)
                    sample_count = frameInfo.samples;

                if (!useAacLength && !initial && (sampleId < numSamples/2) && (sample_count != frameInfo.samples))
                {
                    faad_fprintf("MP4 seems to have incorrect frame duration, using values from AAC data.\n");
                    useAacLength = 1;
                    sample_count = frameInfo.samples;
                }
            }

            if (initial && (sample_count < framesize*frameInfo.channels) && (frameInfo.samples > sample_count))
                delay = frameInfo.samples - sample_count;
        } else {
            sample_count = frameInfo.samples;
        }

        /* open the sound file now that the number of channels are known */
        if (first_time && !frameInfo.error)
        {
            /* print some channel info */
            print_channel_info(&frameInfo);
            metaDataInfo.clear();
            //输出信息
            //-------------
            int tempnum = 0;
            char tempstr[256] = {0};
            //声道数
            sprintf(tempstr, "%d", frameInfo.channels);
            metaDataInfo.insert(make_pair("Channels", tempstr));
            //采样率
            sprintf(tempstr, "%d", frameInfo.samplerate);
            metaDataInfo.insert(make_pair("Sample Rate", tempstr));
            //AAC类型-------
            tempnum = frameInfo.object_type;
            switch (tempnum) {
                case MAIN:
                    sprintf(tempstr,"%s","MAIN");
                    break;
                case LC:
                    sprintf(tempstr,"%s","LC");
                    break;
                case SSR:
                    sprintf(tempstr,"%s","SSR");
                    break;
                case LTP:
                    sprintf(tempstr,"%s","LTP");
                    break;
                case HE_AAC:
                    sprintf(tempstr,"%s","HE_AAC");
                    break;
                case ER_LC:
                    sprintf(tempstr,"%s","ER_LC");
                    break;
                case ER_LTP:
                    sprintf(tempstr,"%s","ER_LTP");
                    break;
                case LD:
                    sprintf(tempstr,"%s","LD");
                    break;
                case DRM_ER_LC:
                    sprintf(tempstr,"%s","DRM_ER_LC");
                    break;
            }
            metaDataInfo.insert(make_pair("Object Type", tempstr));
            //Header Type
            tempnum = frameInfo.header_type;
            switch (tempnum) {
                case RAW:
                    sprintf(tempstr,"%s","RAW");
                    break;
                case ADIF:
                    sprintf(tempstr,"%s","ADIF");
                    break;
                case ADTS:
                    sprintf(tempstr,"%s","ADTS");
                    break;
                case LATM:
                    sprintf(tempstr,"%s","LATM");
                    break;
            }
            metaDataInfo.insert(make_pair("Header Type", tempstr));

            //SBR
            tempnum = frameInfo.sbr;
            switch (tempnum) {
                case NO_SBR:
                    sprintf(tempstr,"%s","off");
                    break;
                case SBR_UPSAMPLED:
                    sprintf(tempstr,"%s","on;upsample");
                    break;
                case SBR_DOWNSAMPLED:
                    sprintf(tempstr,"%s","on;downsampled");
                    break;
                case NO_SBR_UPSAMPLED:
                    sprintf(tempstr,"%s","off;upsampled");
                    break;
            }
            metaDataInfo.insert(make_pair("SBR", tempstr));

            //PS
            tempnum = frameInfo.ps;
            switch (tempnum) {
                case 0:
                    sprintf(tempstr, "%s", "off");
                    break;
                case 1:
                    sprintf(tempstr, "%s", "on");
                    break;
            }
            metaDataInfo.insert(make_pair("PS", tempstr));
            (*pBasicAudioInfoFunc)(containerInfo,metaDataInfo);
            //-------------

            if (!adts_out)
            {
                /* open output file */
                if(!to_stdout)
                {
                    aufile = open_audio_file(sndfile, frameInfo.samplerate, frameInfo.channels,
                        outputFormat, fileType, aacChannelConfig2wavexChannelMask(&frameInfo));
                } else {
#ifdef _WIN32
                    setmode(fileno(stdout), O_BINARY);
#endif
                    aufile = open_audio_file((char*)"-", frameInfo.samplerate, frameInfo.channels,
                        outputFormat, fileType, aacChannelConfig2wavexChannelMask(&frameInfo));
                }
                if (aufile == NULL)
                {
                    NeAACDecClose(hDecoder);
                    mp4ff_close(infile);
                    free(mp4cb);
                    fclose(mp4File);
                    return 0;
                }
            }
            first_time = 0;
        }

        pAacFrame->size = frameInfo.bytesconsumed;
        pAacFrame->sampleNum = frameInfo.samples;
        //AAC每帧数据对应的采样数为1024，对应的每帧时间间隔(ms)为 1024*1000 / 采样率。
        //如48K采样每帧递增21；44.1K采样每帧递增23
        pAacFrame->duration = 1024*1000/frameInfo.samplerate;

        if (sample_count > 0) initial = 0;

        percent = min((int)(sampleId*100)/numSamples, 100);

        if(checkBoxNal && countIndex++ < showAdtsCount)
            (*pAacFrameFunc)(pAacFrame);
        if(!checkBoxNal)
            (*pAacFrameFunc)(pAacFrame);

        if (percent > old_percent)
        {
            old_percent = percent;
            sprintf(percents, "%d%% decoding %s.", percent, mp4file);
            faad_fprintf("%s\r", percents);
            pDecodeProgressFunc(percent);
#ifdef _WIN32
            SetConsoleTitle(percents);
#endif
        }

        if ((frameInfo.error == 0) && (sample_count > 0) && (!adts_out))
        {
            if (write_audio_file(aufile, sample_buffer, sample_count, delay) == 0)
                break;
        }

        if (frameInfo.error > 0)
        {
            faad_fprintf("%s: Warning: %s\n",__FUNCTION__,
                NeAACDecGetErrorMessage(frameInfo.error));
        }
    }

    NeAACDecClose(hDecoder);

    if (adts_out == 1)
    {
        fclose(adtsFile);
    }

    mp4ff_close(infile);

    if (!first_time && !adts_out)
        close_audio_file(aufile);

    free(mp4cb);
    fclose(mp4File);

    return frameInfo.error;
}

int AacDecode::decodeAACfile(const char *aacfile, const char *outfile, const char *adts_fn, int to_stdout,
                             int def_srate, int object_type, int outputFormat, int fileType,
                             int downMatrix, int infoOnly, int adts_out, int old_format)
{
    int result;
    int showHelp = 0;
    unsigned char header[2];
    float length = 0;
    FILE *hAacFile;

/* System dependant types */
#ifdef _WIN32
    long begin;
#else
    clock_t begin;
#endif

#ifdef _WIN32
    begin = GetTickCount();
#else
    begin = clock();
#endif

    if (((fileType < 1) || (fileType > 2)) ||
        (outputFormat < 1 || outputFormat >5) ||
        ((object_type != LC) && (object_type != MAIN) && (object_type != LTP) && (object_type != LD)))
        showHelp = 1;
    else
        showHelp = 0;

    if (showHelp) {
        usage();
        return 1;
    }

    /* check for aac file */
    hAacFile = fopen(aacfile, "r");
    if (!hAacFile)
    {
        faad_fprintf("Error opening file: %s", aacfile);
        return 1;
    }
    fread(header, 1, 2, hAacFile);
    fclose(hAacFile);
    containerInfo.clear();
    countIndex = 0;
    if (header[0] == 0xFF && (header[1] & 0xF0 == 0xF0)){
        containerInfo.insert(make_pair("Container", "RAW Format"));
        result = decodeAACfile(const_cast<char*>(aacfile), const_cast<char*>(outfile), const_cast<char*>(adts_fn), to_stdout,
                               def_srate, object_type, outputFormat, fileType, downMatrix, infoOnly, adts_out,
                               old_format, &length);
    }

    if (!result && !infoOnly)
    {
#ifdef _WIN32
        float dec_length = (float)(GetTickCount()-begin)/1000.0;
        SetConsoleTitle("FAAD");
#else
        /* clock() grabs time since the start of the app but when we decode
           multiple files, each file has its own starttime (begin).
         */
        float dec_length = (float)(clock() - begin)/(float)CLOCKS_PER_SEC;
#endif
        faad_fprintf("Decoding %s took: %5.2f sec. %5.2fx real-time.\n", aacfile,
            dec_length, length/dec_length);
    }

    return 0;
}

int AacDecode::decodeMP4file(const char *mp4file, const char *outfile, const char *adts_fn, int to_stdout,
                             int outputFormat, int fileType, int downMatrix, int noGapless,
                             int infoOnly, int adts_out)
{
    int result;
    int showHelp = 0;
    unsigned char header[8];
    float length = 0;
    FILE *hMP4File;

/* System dependant types */
#ifdef _WIN32
    long begin;
#else
    clock_t begin;
#endif

#ifdef _WIN32
    begin = GetTickCount();
#else
    begin = clock();
#endif

    if (((fileType < 1) || (fileType > 2)) || (outputFormat < 1 || outputFormat >5))
        showHelp = 1;
    else
        showHelp = 0;

    if (showHelp) {
        usage();
        return 1;
    }

    /* check for mp4 file */
    hMP4File = fopen(mp4file, "r");
    if (!hMP4File)
    {
        faad_fprintf("Error opening file: %s", mp4file);
        return 1;
    }
    fread(header, 1, 8, hMP4File);
    fclose(hMP4File);
    containerInfo.clear();
    countIndex = 0;
    if (header[4] == 'f' && header[5] == 't' && header[6] == 'y' && header[7] == 'p'){
        containerInfo.insert(make_pair("Container", "MP4 Container Format"));
        result = decodeMP4file(const_cast<char*>(mp4file), const_cast<char*>(outfile),const_cast<char*>(adts_fn), to_stdout,
                               outputFormat, fileType, downMatrix, noGapless, infoOnly, adts_out, &length);
    }

    if (!result && !infoOnly)
    {
#ifdef _WIN32
        float dec_length = (float)(GetTickCount()-begin)/1000.0;
        SetConsoleTitle("FAAD");
#else
        /* clock() grabs time since the start of the app but when we decode
           multiple files, each file has its own starttime (begin).
         */
        float dec_length = (float)(clock() - begin)/(float)CLOCKS_PER_SEC;
#endif
        faad_fprintf("Decoding %s took: %5.2f sec. %5.2fx real-time.\n", mp4file,
                     dec_length, length/dec_length);
    }

    return 0;
}
