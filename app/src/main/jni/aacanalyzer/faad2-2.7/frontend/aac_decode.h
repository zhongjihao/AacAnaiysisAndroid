/*************************************************************************
    > File Name: aac_decode.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com
    > Created Time: 2018年06月8日 星期五 14时46分38秒
 ************************************************************************/

#ifndef AACANAIYSISANDROID_AAC_DECODE_H
#define AACANAIYSISANDROID_AAC_DECODE_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define off_t __int64
#else
#include <time.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>

#include "../include/neaacdec.h"
#include "../common/mp4ff/mp4ff.h"

#include "audio.h"

#include "../../../log.h"

using namespace std;

/* MicroSoft channel definitions */
#define SPEAKER_FRONT_LEFT             0x1
#define SPEAKER_FRONT_RIGHT            0x2
#define SPEAKER_FRONT_CENTER           0x4
#define SPEAKER_LOW_FREQUENCY          0x8
#define SPEAKER_BACK_LEFT              0x10
#define SPEAKER_BACK_RIGHT             0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER   0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER  0x80
#define SPEAKER_BACK_CENTER            0x100
#define SPEAKER_SIDE_LEFT              0x200
#define SPEAKER_SIDE_RIGHT             0x400
#define SPEAKER_TOP_CENTER             0x800
#define SPEAKER_TOP_FRONT_LEFT         0x1000
#define SPEAKER_TOP_FRONT_CENTER       0x2000
#define SPEAKER_TOP_FRONT_RIGHT        0x4000
#define SPEAKER_TOP_BACK_LEFT          0x8000
#define SPEAKER_TOP_BACK_CENTER        0x10000
#define SPEAKER_TOP_BACK_RIGHT         0x20000
#define SPEAKER_RESERVED               0x80000000

/* FAAD file buffering routines */
typedef struct {
    long bytes_into_buffer;
    long bytes_consumed;
    long file_offset;
    unsigned char *buffer;
    int at_eof;
    FILE *infile;
} aac_buffer;

struct AudioFrame{
    long size;
    long sampleNum;
    long duration;
};

class AacDecode
{
private:
    char progName[256];    //程序名
    map<string, string> containerInfo;
    map<string, string> metaDataInfo;
    AudioFrame* pAacFrame;
    void (*pBasicAudioInfoFunc)(map<string, string>& containerInfo,map<string, string>& metaDataInfo);
    void (*pAacFrameFunc)(AudioFrame* aacFrame);
    void (*pDecodeProgressFunc)(int progressPercent);
//    static const char *file_ext[7];
//    static const unsigned long srates[12];
private:
    AacDecode(const AacDecode&);
    AacDecode& operator=(const AacDecode&);
    int fill_buffer(aac_buffer *b);
    void advance_buffer(aac_buffer *b, int bytes);
    int adts_parse(aac_buffer *b, int *bitrate, float *length);
    long aacChannelConfig2wavexChannelMask(NeAACDecFrameInfo *hInfo);
    const char *position2string(int position);
    void print_channel_info(NeAACDecFrameInfo *frameInfo);
    int FindAdtsSRIndex(int sr);
    unsigned char *MakeAdtsHeader(int *dataSize, NeAACDecFrameInfo *hInfo, int old_format);
    void usage(void);
    int decodeAACfile(char *aacfile, char *sndfile, char *adts_fn, int to_stdout,
                             int def_srate, int object_type, int outputFormat, int fileType,
                             int downMatrix, int infoOnly, int adts_out, int old_format,
                             float *song_length);
    int GetAACTrack(mp4ff_t *infile);
    int decodeMP4file(char *mp4file, char *sndfile, char *adts_fn, int to_stdout,
                             int outputFormat, int fileType, int downMatrix, int noGapless,
                             int infoOnly, int adts_out, float *song_length);
    static uint32_t read_callback(void *user_data, void *buffer, uint32_t length);
    static uint32_t seek_callback(void *user_data, uint64_t position);

public:
    AacDecode();
    AacDecode(const char* proName);
    ~AacDecode();
    void setBasicInfoCallback(void (*func)(map<string, string>& containerInfo,map<string, string>& metaDataInfo));
    void setAacFrameCallback(void (*func)(AudioFrame* aacFrame));
    void setProgressCallback(void (*func)(int progressPercent));
    /* aacfile : 要解码的AAC源文件
     * outfile : 解码输出的音频文件
     * adts_fn : 存放ADTS帧文件，adts_out为1则需要指定adts_fn，adts_out为0则adts_fn为NULL
     * to_stdout : 是否输出终端,0为输出，1为不输出，默认为0
     * def_srate : 采样率
     * object_type : Set object type. Supported object types:
     *              1:  Main object type
     *              2:  LC (Low Complexity) object type
     *              4:  LTP (Long Term Prediction) object type
     *              23: LD (Low Delay) object type
     * outputFormat : 采样精度, Set output sample format.Valid values are:
     *              1:  16 bit PCM data (default)
     *              2:  24 bit PCM data
     *              3:  32 bit PCM data
     *              4:  32 bit floating point data
     *              5:  64 bit floating point data
     * fileType :  Set output format. Valid values are:
     *              1:  Microsoft WAV format (default)
     *              2:  RAW PCM data
     * downMatrix: 默认为0
     * infoOnly : 默认为0
     * adts_out : 是否输出解码的adts,默认为1
     * old_format : 默认为0
     */
    int decodeAACfile(const char *aacfile, const char *outfile, const char *adts_fn, int to_stdout,
                      int def_srate, int object_type, int outputFormat, int fileType,
                      int downMatrix, int infoOnly, int adts_out, int old_format);

    /* mp4file : 要解码的mp4源文件
     * outfile : 解码输出的文件
     * adts_fn : 存放ADTS帧文件,adts_out为1则需要指定adts_fn，adts_out为0则adts_fn为NULL
     * to_stdout : 是否输出终端,0为输出，1为不输出，默认为0
     * outputFormat : 采样精度, Set output sample format.Valid values are:
     *              1:  16 bit PCM data (default)
     *              2:  24 bit PCM data
     *              3:  32 bit PCM data
     *              4:  32 bit floating point data
     *              5:  64 bit floating point data
     *
     * fileType : Set output format. Valid values are:
     *              1:  Microsoft WAV format (default)
     *              2:  RAW PCM data
     * downMatrix: 默认为0
     * noGapless : 默认为0
     * infoOnly : 默认为0
     * adts_out : 是否输出解码的adts,默认为1
     */
    int decodeMP4file(const char *mp4file, const char *outfile, const char *adts_fn, int to_stdout,
                                 int outputFormat, int fileType, int downMatrix, int noGapless,
                                 int infoOnly, int adts_out);
};

#endif //AACANAIYSISANDROID_AAC_DECODE_H
