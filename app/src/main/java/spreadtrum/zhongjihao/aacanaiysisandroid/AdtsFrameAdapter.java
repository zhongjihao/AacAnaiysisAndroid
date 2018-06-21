package spreadtrum.zhongjihao.aacanaiysisandroid;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.ArrayList;

/**
 * Created by zhongjihao on 18-6-21.
 */

public class AdtsFrameAdapter extends BaseAdapter {
    private ArrayList<AudioFrame> dataList;
    private Context context;
    private int resource;

    public AdtsFrameAdapter(Context context, ArrayList<AudioFrame> list, int resource) {
        this.context = context;
        this.dataList = list;
        this.resource = resource;
    }

    @Override
    public int getCount() {
        if (dataList != null)
            return dataList.size();
        return 0;
    }

    @Override
    public Object getItem(int position) {
        if (dataList != null)
            return dataList.get(position);
        return null;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHoder viewHoder = null;
        if (convertView == null) {
            viewHoder = new ViewHoder();
            // 给xml布局文件创建java对象
            LayoutInflater inflater = LayoutInflater.from(context);
            convertView = inflater.inflate(resource, null);
            // 指向布局文件内部组件
            viewHoder.number_tv = (TextView) convertView
                    .findViewById(R.id.number_tv);
            viewHoder.size_tv = (TextView) convertView.findViewById(R.id.size_tv);
            viewHoder.sampleNum_tv = (TextView) convertView.findViewById(R.id.sampleNum_tv);
            viewHoder.duration_tv = (TextView) convertView.findViewById(R.id.duration_tv);
            // 增加额外变量
            convertView.setTag(viewHoder);
        } else {
            viewHoder = (ViewHoder) convertView.getTag();
        }
        // 获取数据显示在各组件
        viewHoder.number_tv.setText(String.valueOf(position));
        viewHoder.size_tv.setText(String.valueOf(dataList.get(position).getSize()));
        viewHoder.sampleNum_tv.setText(String.valueOf(dataList.get(position).getSampleNum()));
        viewHoder.duration_tv.setText(dataList.get(position).getDuration()+"ms");

        return convertView;
    }

    static class ViewHoder {
        TextView number_tv;
        TextView size_tv;
        TextView sampleNum_tv;
        TextView duration_tv;
    }
}
