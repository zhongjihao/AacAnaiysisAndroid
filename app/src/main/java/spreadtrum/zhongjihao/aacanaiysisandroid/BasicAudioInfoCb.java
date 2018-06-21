package spreadtrum.zhongjihao.aacanaiysisandroid;

import android.util.Log;

import java.util.HashMap;

/**
 * Created by zhongjihao on 18-6-15.
 */

/*
 Fadd2库回调java类,该类不允许被上层修改
 */
public class BasicAudioInfoCb {
    private static final String TAG = "BasicAudioInfoCb";
    private static AudioInfoListener listener;

    public interface AudioInfoListener{
        void audioInfo(HashMap<String, String> containerInfo,HashMap<String, String> metaDataInfo);
        void aacFrameList(AudioFrame aacFrame);
        void decodeFrameProgress(int progressPercent);
    }

    public static void setListener(AudioInfoListener l){
        listener = l;
    }

    public static void basicAudioInfo(HashMap<String, String> containerInfo, HashMap<String, String> metaDataInfo) {
        Log.d(TAG,"===zhongjihao====container count: "+containerInfo.size()+"   meta count: "+metaDataInfo.size());
        listener.audioInfo(containerInfo,metaDataInfo);
    }

    static int i = 0;
    public static void aacFrameInfo(AudioFrame aacFrame) {
        //if(progressPercent<=5)
         //   Log.d(TAG,"===aacFrameInfo=====zhongjihao===size: "+aacFrame.getSize()+"   sampleNum: "+aacFrame.getSampleNum()+"  duration: "+aacFrame.getDuration()+"   progressPercent: "+progressPercent);

       // if(progressPercent >96)

     //   Log.d(TAG,"===aacFrameInfo=====zhongjihao===size: "+aacFrame.getSize()+"   sampleNum: "+aacFrame.getSampleNum()+"  duration: "+aacFrame.getDuration()+"   index: "+(i++));
        listener.aacFrameList(aacFrame);
    }

    public static void decodeFrameProgress(int progressPercent) {
        listener.decodeFrameProgress(progressPercent);
    }
}
