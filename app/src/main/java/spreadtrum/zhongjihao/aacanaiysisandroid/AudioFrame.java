package spreadtrum.zhongjihao.aacanaiysisandroid;

/**
 * Created by zhongjihao on 18-6-16.
 */

public class AudioFrame {
    private int size;
    private int sampleNum;
    private int duration;

    public AudioFrame() {

    }

    public AudioFrame(int size,int sampleNum,int duration) {
        this.size = size;
        this.sampleNum = sampleNum;
        this.duration = duration;
    }

    public int getSize() {
        return size;
    }

    public void setSize(int size) {
        this.size = size;
    }

    public int getSampleNum() {
        return sampleNum;
    }

    public void setSampleNum(int sampleNum) {
        this.sampleNum = sampleNum;
    }

    public int getDuration() {
        return duration;
    }

    public void setDuration(int duration) {
        this.duration = duration;
    }
}
