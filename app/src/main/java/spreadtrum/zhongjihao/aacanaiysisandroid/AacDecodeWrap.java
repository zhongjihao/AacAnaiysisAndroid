package spreadtrum.zhongjihao.aacanaiysisandroid;


/**
 * Created by zhongjihao on 18-6-11.
 */

public class AacDecodeWrap {
    private long cPtr;
    private static AacDecodeWrap mInstance;
    private static Object lockobj = new Object();

    private AacDecodeWrap() {
        cPtr = 0;
    }

    public static AacDecodeWrap newInstance() {
        synchronized (lockobj) {
            if (mInstance == null) {
                mInstance = new AacDecodeWrap();
            }
        }
        return mInstance;
    }

    //启动faad2解码器
    public void startFaad2Engine(String proName) {
        cPtr = AacAnalyzeNativeJni.startFaad2Engine(proName);
    }

    //解码AAC文件
    public int decodeAacFile(String aacfile,String outfile,String adts_fn,int to_stdout,int def_srate,int object_type,int outputFormat, int fileType,
                             int downMatrix, int infoOnly, int adts_out, int old_format) {
        if (cPtr != 0) {
            return AacAnalyzeNativeJni.decodeAACfile(cPtr,aacfile,outfile,adts_fn,to_stdout,def_srate,object_type,outputFormat,fileType,downMatrix,infoOnly,adts_out,old_format);
        }
        return -1;
    }

    //解码MP4文件
    public int decodeMp4File(String mp4file,String outfile,String adts_fn,int to_stdout,int outputFormat, int fileType,
                             int downMatrix, int noGapless,int infoOnly,int adts_out) {
        if (cPtr != 0) {
            return AacAnalyzeNativeJni.decodeMP4file(cPtr,mp4file,outfile,adts_fn,to_stdout,outputFormat,fileType,downMatrix,noGapless,infoOnly,adts_out);
        }
        return -1;
    }

    //停止faad2解码器
    public void stopFaad2Engine() {
        if (cPtr != 0) {
            AacAnalyzeNativeJni.stopFaad2Engine(cPtr);
        }
        mInstance = null;
    }

}
