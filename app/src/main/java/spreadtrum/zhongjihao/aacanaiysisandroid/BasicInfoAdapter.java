package spreadtrum.zhongjihao.aacanaiysisandroid;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;


/**
 * Created by zhongjihao on 18-6-14.
 */

public class BasicInfoAdapter extends BaseExpandableListAdapter {
    private final static String TAG = "BasicInfoAdapter";
    private ArrayList<BasicinfoGroup> list;
    private Context context;
    private LayoutInflater inflater;

    public BasicInfoAdapter(ArrayList<BasicinfoGroup> list, Context context) {
        this.list = list;
        this.context = context;
        Log.d(TAG,"==BasicInfoAdapter===zhongjihao===group1 len: "+list.get(0).getChildCount()+"     group2 len: "+list.get(1).getChildCount());
        inflater = LayoutInflater.from(context);
    }

    @Override
    public int getGroupCount() {
        return list.size();
    }

    @Override
    public int getChildrenCount(int groupPosition) {
        return list.get(groupPosition).getChildCount();
    }

    @Override
    public BasicinfoGroup getGroup(int groupPosition) {
        return list.get(groupPosition);
    }

    @Override
    public Object getChild(int groupPosition, int childPosition) {
        return list.get(groupPosition).getChild(childPosition);
    }

    @Override
    public long getGroupId(int groupPosition) {
        return groupPosition;
    }

    @Override
    public long getChildId(int groupPosition, int childPosition) {
        return childPosition;
    }

    @Override
    public boolean hasStableIds() {
        return true;
    }

    @Override
    public View getGroupView(int groupPosition, boolean isExpanded,
                             View convertView, ViewGroup parent) {
        GroupViewHolder viewHoder = null;
        if(convertView == null){
            viewHoder = new GroupViewHolder();
            convertView = inflater.inflate(R.layout.basic_info_group, null);
            viewHoder.groupName_tv = (TextView) convertView
                    .findViewById(R.id.tv_group);
            viewHoder.groupIcon_iv = (ImageView) convertView
                    .findViewById(R.id.iv_selector);
            convertView.setTag(viewHoder);
        }else{
            viewHoder = (GroupViewHolder) convertView.getTag();
        }

        viewHoder.groupName_tv.setText(getGroup(groupPosition).getGroupName());
        viewHoder.groupIcon_iv.setImageResource(R.drawable.w2);

        // 更换展开分组图片
        if (!isExpanded) {
            viewHoder.groupIcon_iv.setImageResource(R.drawable.w1);
        }
        return convertView;
    }

    @Override
    public View getChildView(int groupPosition, int childPosition,
                             boolean isLastChild, View convertView, ViewGroup parent) {
        ChildViewHolder viewHoder = null;
        if (convertView == null) {
            viewHoder = new ChildViewHolder();
            convertView = inflater.inflate(R.layout.basic_info_child, null);
            viewHoder.metaDataKey_tv = (TextView) convertView
                    .findViewById(R.id.key_tv);
            viewHoder.metaDataVal_tv = (TextView) convertView
                    .findViewById(R.id.value_tv);
            convertView.setTag(viewHoder);
        } else {
            viewHoder = (ChildViewHolder) convertView.getTag();
        }

        viewHoder.metaDataKey_tv.setText(((BasicinfoChild)getChild(groupPosition, childPosition)).getName());
        viewHoder.metaDataVal_tv.setText(((BasicinfoChild)getChild(groupPosition, childPosition)).getValue());
        return convertView;
    }

    // 子选项是否可以选择
    @Override
    public boolean isChildSelectable(int groupPosition, int childPosition) {
        return true;
    }

    static class GroupViewHolder {
        TextView groupName_tv;
        ImageView groupIcon_iv;
    }

    static class ChildViewHolder {
        TextView metaDataKey_tv;
        TextView metaDataVal_tv;
    }
}
