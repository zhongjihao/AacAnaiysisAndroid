package spreadtrum.zhongjihao.aacanaiysisandroid;

import android.app.IntentService;
import android.content.Intent;

/**
 * Created by zhongjihao on 18-6-19.
 */

public class DecodeAacService extends IntentService {
    public DecodeAacService() {
        super("DecodeAacService");
    }

    /**
     * 实现异步任务的方法
     *
     * @param intent Activity传递过来的Intent,数据封装在intent中
     */
    @Override
    protected void onHandleIntent(Intent intent) {
        String srcFile = intent.getExtras().getString("infile");
        String outfile = intent.getExtras().getString("outfile");
        String adtsfile = intent.getExtras().getString("adts-outfile",null);
        int srcfileType = intent.getExtras().getInt("infileType");
        int to_stdout = intent.getExtras().getInt("to_stdout",0);
        int SampleAccuracy = intent.getExtras().getInt("outputFormat",1);
        int outFormat = intent.getExtras().getInt("fileType",1);
        int downMatrix = intent.getExtras().getInt("downMatrix",0);
        int infoOnly = intent.getExtras().getInt("infoOnly",0);
        int adts_out = intent.getExtras().getInt("adts_out",0);

        if (srcfileType == 1){
            int noGapless = intent.getExtras().getInt("noGapless",0);
            AacDecodeWrap.newInstance().decodeMp4File(srcFile, outfile, adtsfile, to_stdout, SampleAccuracy, outFormat, downMatrix, noGapless, infoOnly, adts_out);
        } else if(srcfileType == 2) {
            int sampleRate = intent.getExtras().getInt("def_srate",44100);
            int objectType = intent.getExtras().getInt("object_type",2);
            int old_format = intent.getExtras().getInt("old_format",0);
            AacDecodeWrap.newInstance().decodeAacFile(srcFile, outfile, adtsfile, to_stdout, sampleRate, objectType, SampleAccuracy, outFormat, downMatrix, infoOnly, adts_out, old_format);
        }
    }
}
