//
// Created by SPREADTRUM\jihao.zhong on 18-5-3.
//

package spreadtrum.zhongjihao.aacanaiysisandroid;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.os.Process;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ExpandableListView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemSelectedListener;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.TimeoutException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


public class MainActivity extends AppCompatActivity implements View.OnClickListener,OnItemSelectedListener,BasicAudioInfoCb.AudioInfoListener{
    private EditText file_edit;
    private Button openfile_btn;
    private CheckBox totalAdts_cb;
    private ExpandableListView basicInfo_lv;
    private ListView adts_lv;
    private TextView empty_tv;
    private Button exit_btn;
    private Button start_btn;
    private Button about_btn;
    private ProgressBar hor_pbar;
    private TextView progress_val_tv;
    private Spinner output_format_spDwon;
    private Spinner output_sample_spDwon;

    private ArrayAdapter<String> outformatDropDownAdapter;
    private ArrayList<String> outformat_list;
    private ArrayAdapter<String> outsampleDropDownAdapter;
    private ArrayList<String> outsample_list;

    /*
       采样精度, Set output sample format.Valid values are:
     * 1:  16 bit PCM data (default)
     * 2:  24 bit PCM data
     * 3:  32 bit PCM data
     * 4:  32 bit floating point data
     * 5:  64 bit floating point data
     */
    private int sampleAccuracy = 1;
    /*
       Set output format. Valid values are:
     * 1:  Microsoft WAV format (default)
     * 2:  RAW PCM data
     */
    private int outfileFormat = 1;

    private final String[] outfileFormatStr = new String[]{"Microsoft WAV format","RAW PCM data"};
    private final String[] sampleAccuracyStr = new String[]{"16 bit PCM data","24 bit PCM data","32 bit PCM data","32 bit floating point data","64 bit floating point data"};

    // 群组名称（一级条目内容）
    private String[] aacInfoGroup = new String[]{"MetaData Information","Container Information"};
    private WorkerHandler msgHandler;
    private BasicInfoAdapter metaDataAdapter;
    private ArrayList<BasicinfoGroup> metaDataList;

    private AdtsFrameAdapter adtsFrameAdapter;
    private ArrayList<AudioFrame> mAacFrameList;
    private ArrayList<AudioFrame> mAacTempFrameList;

    private static final int TARGET_READ_PERMISSION_REQUEST = 100;
    private static final int AAC_BASIC_INFO = 1;
    private static final int AAC_FRAME_INFO = 2;

    // 要申请的权限
    private String[] permissions = {Manifest.permission.READ_EXTERNAL_STORAGE,Manifest.permission.WRITE_EXTERNAL_STORAGE};

    public static class WorkerHandler extends Handler {
        WeakReference<MainActivity> sr;

        public WorkerHandler(MainActivity stateReceiver) {
            sr = new WeakReference<MainActivity>(stateReceiver);
        }

        @Override
        public void handleMessage(Message msg) {
            MainActivity act = sr.get();
            if (act == null) {
                return;
            }
            switch (msg.what) {
                case AAC_BASIC_INFO:
                    SerializableMap containerInfo = (SerializableMap) msg.getData().get("containerInfo");
                    SerializableMap metaDataInfo = (SerializableMap) msg.getData().get("metaDataInfo");
                    if (act.metaDataAdapter == null) {
                        Log.d("MainActivity","====zhongjihao======containerInfo: "+containerInfo.size()+"   metaDataInfo: "+metaDataInfo.size());
                        for(int i=0;i<act.aacInfoGroup.length;i++){
                            BasicinfoGroup group = new BasicinfoGroup(act.aacInfoGroup[i]);
                            act.metaDataList.add(group);
                        }
                        ArrayList<BasicinfoChild> containerChild = new ArrayList<BasicinfoChild>();
                        ArrayList<BasicinfoChild> metaDataChild = new ArrayList<BasicinfoChild>();
                        for(HashMap.Entry<String,String> item : containerInfo.getMap().entrySet()){
                          //  Log.d("MainActivity","==container=====zhongjihao==="+item.getKey()+" : "+item.getValue());
                            containerChild.add(new BasicinfoChild(item.getKey(),item.getValue()));
                        }
                        for(HashMap.Entry<String,String> item : metaDataInfo.getMap().entrySet()){
                            metaDataChild.add(new BasicinfoChild(item.getKey(),item.getValue()));
                        }
                        act.metaDataList.get(0).addChild(metaDataChild);
                        act.metaDataList.get(1).addChild(containerChild);
                        act.metaDataAdapter = new BasicInfoAdapter(act.metaDataList, act);
                        act.basicInfo_lv.setAdapter(act.metaDataAdapter);
                    } else {
                        for(int i=0;i<act.aacInfoGroup.length;i++){
                            BasicinfoGroup group = new BasicinfoGroup(act.aacInfoGroup[i]);
                            act.metaDataList.add(group);
                        }
                        ArrayList<BasicinfoChild> containerChild = new ArrayList<BasicinfoChild>();
                        ArrayList<BasicinfoChild> metaDataChild = new ArrayList<BasicinfoChild>();
                        for(HashMap.Entry<String,String> item : containerInfo.getMap().entrySet()){
                            containerChild.add(new BasicinfoChild(item.getKey(),item.getValue()));
                        }
                        for(HashMap.Entry<String,String> item : metaDataInfo.getMap().entrySet()){
                            metaDataChild.add(new BasicinfoChild(item.getKey(),item.getValue()));
                        }
                        act.metaDataList.get(0).addChild(metaDataChild);
                        act.metaDataList.get(1).addChild(containerChild);
                        act.metaDataAdapter.notifyDataSetChanged();
                    }
                    act.msgHandler.removeMessages(AAC_BASIC_INFO);
                    break;
                case AAC_FRAME_INFO:
//                    act.hor_pbar.setProgress(msg.arg1);
//                    act.progress_val_tv.setText(msg.arg1+"%");
                    act.mAacFrameList.addAll(act.mAacTempFrameList);
                    if (act.adtsFrameAdapter == null) {
                        act.adtsFrameAdapter = new AdtsFrameAdapter(act, act.mAacFrameList, R.layout.adts_frame_item);
                        act.adts_lv.setAdapter(act.adtsFrameAdapter);
                    } else {
                        act.adtsFrameAdapter.notifyDataSetChanged();
                    }
                    act.mAacTempFrameList.clear();
                    act.msgHandler.removeMessages(AAC_FRAME_INFO);
                    break;
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        file_edit = (EditText)findViewById(R.id.file_edit);
        openfile_btn = (Button)findViewById(R.id.openfile_btn);
        totalAdts_cb = (CheckBox)findViewById(R.id.totalnal_cb);
        basicInfo_lv = (ExpandableListView)findViewById(R.id.basicinfo_list);
        output_format_spDwon = (Spinner)findViewById(R.id.output_format_spDwon);
        output_sample_spDwon = (Spinner)findViewById(R.id.output_sample_spDwon);
        adts_lv = (ListView)findViewById(R.id.aac_adts_lv);
        empty_tv = (TextView)findViewById(R.id.empty_tv);
        hor_pbar = (ProgressBar)findViewById(R.id.hor_pbar);
        progress_val_tv = (TextView)findViewById(R.id.progress_val_tv);
        totalAdts_cb = (CheckBox)findViewById(R.id.totalnal_cb);

        exit_btn = (Button)findViewById(R.id.exit_btn);
        start_btn = (Button)findViewById(R.id.start_btn);
        about_btn = (Button)findViewById(R.id.about_btn);
        openfile_btn.setOnClickListener(this);
        exit_btn.setOnClickListener(this);
        start_btn.setOnClickListener(this);
        about_btn.setOnClickListener(this);

        LayoutInflater inflater = LayoutInflater.from(this);
        adts_lv.addHeaderView(inflater.inflate(R.layout.adts_frame_item, null),null,false);
        adts_lv.setEmptyView(empty_tv);
        outputOptionInit();
        mAacFrameList = new ArrayList<>();
        mAacTempFrameList = new ArrayList<>();
        metaDataList = new ArrayList<BasicinfoGroup>();
        msgHandler = new WorkerHandler(this);

        totalAdts_cb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                String txt = totalAdts_cb.getText().toString();
                Pattern pattern = Pattern.compile("\\d+");
                Matcher matcher = pattern.matcher(txt);
                String showNaluNumber = "";
                while (matcher.find()) {
                    showNaluNumber = matcher.group(0);
                }
                Log.d("MainActivity","====zhongjihao=======showNaluNumber: "+showNaluNumber);
                AacDecodeWrap.newInstance().setCheckBox(isChecked,Integer.valueOf(showNaluNumber));
            }
        });

        AacDecodeWrap.newInstance().startFaad2Engine(FileUtils.getAppName(this));
        BasicAudioInfoCb.setListener(this);

        // 版本判断。当手机系统大于 23 时，才有必要去判断权限是否获取
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 检查该权限是否已经获取
            boolean permission = false;
            for (int i = 0; i < permissions.length; i++) {
                int result = ContextCompat.checkSelfPermission(this, permissions[i]);
                // 权限是否已经 授权 GRANTED---授权  DINIED---拒绝
                if (result != PackageManager.PERMISSION_GRANTED) {
                    permission = false;
                    break;
                } else
                    permission = true;
            }
            if(!permission){
                // 如果没有授予权限，就去提示用户请求
                ActivityCompat.requestPermissions(this,
                        permissions, TARGET_READ_PERMISSION_REQUEST);
            }
        }

    }

    private void outputOptionInit() {
        outformat_list = new ArrayList<>();
        outsample_list = new ArrayList<>();
        outformat_list.add(outfileFormatStr[0]);
        outformat_list.add(outfileFormatStr[1]);

        outsample_list.add(sampleAccuracyStr[0]);
        outsample_list.add(sampleAccuracyStr[1]);
        outsample_list.add(sampleAccuracyStr[2]);
        outsample_list.add(sampleAccuracyStr[3]);
        outsample_list.add(sampleAccuracyStr[4]);

        outformatDropDownAdapter = new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item,outformat_list);
        /*adapter设置一个下拉列表样式，参数为系统子布局*/
        outformatDropDownAdapter.setDropDownViewResource(android.R.layout.simple_spinner_item);
        output_format_spDwon.setAdapter(outformatDropDownAdapter);
        output_format_spDwon.setOnItemSelectedListener(this);

        outsampleDropDownAdapter = new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item,outsample_list);
        /*adapter设置一个下拉列表样式，参数为系统子布局*/
        outsampleDropDownAdapter.setDropDownViewResource(android.R.layout.simple_spinner_item);
        output_sample_spDwon.setAdapter(outsampleDropDownAdapter);
        output_sample_spDwon.setOnItemSelectedListener(this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        msgHandler.removeMessages(AAC_BASIC_INFO);
        msgHandler.removeMessages(AAC_FRAME_INFO);
        mAacFrameList.clear();
        metaDataList.clear();
        mAacTempFrameList.clear();
        mAacTempFrameList = null;
        metaDataList = null;
        mAacFrameList = null;
        metaDataAdapter = null;
        android.os.Process.killProcess(Process.myPid());
    }

    @Override
    public void onClick(View v) {
        int vid = v.getId();
        switch (vid){
            case R.id.about_btn:
                FileUtils.showSoftInfo(findViewById(R.id.bottomRlayout),this);
                break;
            case R.id.start_btn:
                if (TextUtils.isEmpty(file_edit.getText().toString())) {
                    Toast.makeText(this, getText(R.string.open_file_tips), Toast.LENGTH_SHORT)
                            .show();
                    return;
                }
                File infile = new File(file_edit.getText().toString());
                if(!(FileUtils.isBinderFile(infile) &&
                        (FileUtils.isValidMp4File(infile) || FileUtils.isValidAacFile(infile)))){
                    Toast.makeText(this, getText(R.string.parse_file_tips), Toast.LENGTH_SHORT)
                            .show();
                    return;
                }
                mAacFrameList.clear();
                metaDataList.clear();
                mAacTempFrameList.clear();
                msgHandler.removeMessages(AAC_BASIC_INFO);
                msgHandler.removeMessages(AAC_FRAME_INFO);
                Intent intent = new Intent(this,DecodeAacService.class);
                String path = infile.getParent();
                String outfile = path + "/" + "mp4_outfile.wav";
                String adtsfile = null;// path + "/"+"outfile.aac";
                intent.putExtra("infile",infile.getAbsolutePath());
                intent.putExtra("outfile",outfile);
                intent.putExtra("adts-outfile",adtsfile);
                int infileType = 0;
                if (FileUtils.isValidMp4File(infile)){
                    infileType = 1;
                    intent.putExtra("noGapless",0);
                } else if(FileUtils.isValidAacFile(infile)) {
                    infileType = 2;
                    intent.putExtra("def_srate",44100);
                    intent.putExtra("object_type",2);
                    intent.putExtra("old_format",0);
                }
                intent.putExtra("infileType",infileType);
                intent.putExtra("to_stdout",0);
                intent.putExtra("outputFormat",sampleAccuracy);
                intent.putExtra("fileType",outfileFormat);
                intent.putExtra("downMatrix",0);
                intent.putExtra("infoOnly",0);
                intent.putExtra("adts_out",0);
                Log.d("MainActivity", "===zhongjihao======dir: " + path + "  path: " + infile.getAbsolutePath());
                startService(intent);
                break;
            case R.id.exit_btn:
                AacDecodeWrap.newInstance().stopFaad2Engine();
                finish();
                break;
            case R.id.openfile_btn:
                FileUtils.showFileChooser(this);
                break;
        }
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position,
                               long id) {
        int vid = parent.getId();
        if (vid == R.id.output_format_spDwon) {
            String value = outformatDropDownAdapter.getItem(position);   //获取选中的那一项
            Log.d("MainActivity", "===zhongjihao====1===onItemSelected===value: " + value);
            if (outfileFormatStr[0].equalsIgnoreCase(value))
                outfileFormat = 1;
            else if (outfileFormatStr[1].equalsIgnoreCase(value))
                outfileFormat = 2;
        } else if (vid == R.id.output_sample_spDwon) {
            String value = outsampleDropDownAdapter.getItem(position);   //获取选中的那一项
            Log.d("MainActivity", "===zhongjihao===2===onItemSelected===value: " + value);
            if (sampleAccuracyStr[0].equalsIgnoreCase(value))
                sampleAccuracy = 1;
            else if (sampleAccuracyStr[1].equalsIgnoreCase(value))
                sampleAccuracy = 2;
            else if (sampleAccuracyStr[2].equalsIgnoreCase(value))
                sampleAccuracy = 3;
            else if (sampleAccuracyStr[3].equalsIgnoreCase(value))
                sampleAccuracy = 4;
            else if (sampleAccuracyStr[4].equalsIgnoreCase(value))
                sampleAccuracy = 5;
        }

        TextView tv = (TextView) view;
        tv.setTextColor(getResources().getColor(R.color.black));    //设置颜色

        tv.setTextSize(10.0f);    //设置大小
        Log.d("MainActivity", "===zhongjihao===onItemSelected===outfileFormat: " + outfileFormat+"  sampleAccuracy: "+sampleAccuracy);
    }

    @Override
    public void onNothingSelected(AdapterView<?> arg0) {

    }

    @Override
    public void audioInfo(HashMap<String, String> containerInfo,HashMap<String, String> metaDataInfo){
        Message msg = Message.obtain();
        msg.what = AAC_BASIC_INFO;
        //传递数据
        SerializableMap containerMap = new SerializableMap();
        containerMap.setMap(containerInfo);
        SerializableMap metaDataMap = new SerializableMap();
        metaDataMap.setMap(metaDataInfo);
        msg.getData().putSerializable("containerInfo",containerMap);
        msg.getData().putSerializable("metaDataInfo",metaDataMap);
        msgHandler.sendMessage(msg);
    }

    @Override
    public void aacFrameList(AudioFrame aacFrame){
        mAacTempFrameList.add(new AudioFrame(aacFrame.getSize(),aacFrame.getSampleNum(),aacFrame.getDuration()));
    }

    @Override
    public void decodeFrameProgress(int progressPercent){
        Log.d("MainActivity", "===zhongjihao======decodeFrameProgress===progressPercent: " + progressPercent);
        final int pro = progressPercent;
        Message msg = Message.obtain();
        msg.what = AAC_FRAME_INFO;
        msg.arg1 = progressPercent;
        msgHandler.post(new Runnable() {
            @Override
            public void run() {
                hor_pbar.setProgress(pro);
                progress_val_tv.setText(pro+"%");
            }
        });
        msgHandler.sendMessage(msg);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == Activity.RESULT_OK && requestCode == FileUtils.FILE_SELECT_CODE) {
            // Get the Uri of the selected file
            Uri uri = data.getData();
            String url = FileUtils.getPathFromURI(this, uri);
            Log.d("MainActivity", "===zhongjihao======aac path: " + url+"  uri: "+uri.toString());
            // String fileName = url.substring(url.lastIndexOf("/") + 1);
            file_edit.setText(url);
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (ActivityCompat.checkSelfPermission(this, android.Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED
                && (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED)) {
            if(requestCode == TARGET_READ_PERMISSION_REQUEST){
                openfile_btn.setEnabled(true);
            }
        }else{
            openfile_btn.setEnabled(false);
            Toast.makeText(this, getText(R.string.file_read_permission_tips), Toast.LENGTH_SHORT)
                    .show();
        }
    }

}
