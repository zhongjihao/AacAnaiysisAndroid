package spreadtrum.zhongjihao.aacanaiysisandroid;

/**
 * Created by zhongjihao on 18-6-8.
 */

public class AacAnalyzeNativeJni {
    static {
        System.loadLibrary("aacAnalyzer");
    }

    public native static long startFaad2Engine(String proName);
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
    * 返回 audio frame总数
    */
    public native static int decodeAACfile(long cPtr,String aacfile,String outfile,String adts_fn,int to_stdout,int def_srate,int object_type,int outputFormat, int fileType,
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
     * 返回 audio frame总数
     */
    public native static int decodeMP4file(long cPtr,String mp4file,String outfile,String adts_fn,int to_stdout,int outputFormat, int fileType,
                                           int downMatrix, int noGapless,int infoOnly,int adts_out);

    public native static void stopFaad2Engine(long cPtr);

}
