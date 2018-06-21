package spreadtrum.zhongjihao.aacanaiysisandroid;

import java.io.Serializable;
import java.util.HashMap;

/**
 * Created by zhongjihao on 18-6-20.
 */

public class SerializableMap implements Serializable {
    private HashMap<String, String> map;

    public HashMap<String, String> getMap() {
        return map;
    }

    public void setMap(HashMap<String, String> map) {
        this.map = map;
    }

    public int size(){
        return map.size();
    }
}
