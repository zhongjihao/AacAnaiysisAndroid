package spreadtrum.zhongjihao.aacanaiysisandroid;

import java.util.ArrayList;

/**
 * Created by zhongjihao on 18-6-20.
 */

public class BasicinfoGroup {
    //分组名
    private String groupName;
    //子项列表
    private ArrayList<BasicinfoChild> list;

    public BasicinfoGroup(String groupName) {
        this.groupName = groupName;
        list = new ArrayList<BasicinfoChild>();
    }

    //添加子项
    public void addChild(ArrayList<BasicinfoChild> child) {
        if(list.size() > 0)
            list.clear();
        list.addAll(child);
    }

    //获取某个分组中子项的数量
    public int getChildCount() {
        return list.size();
    }

    //获取分组中某个子项
    public BasicinfoChild getChild(int childPosition) {
        return list.get(childPosition);
    }

    public String getGroupName(){
        return groupName;
    }
}
