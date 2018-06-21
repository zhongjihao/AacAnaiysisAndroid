package spreadtrum.zhongjihao.aacanaiysisandroid;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.PopupWindow;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

/**
 * Created by zhongjihao on 18-6-12.
 */

public class FileUtils {
    private static final String TAG = "FileUtils";
    public static final int FILE_SELECT_CODE = 1;

    public static void showFileChooser(Activity act) {
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("*/*");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        try {
            act.startActivityForResult(Intent.createChooser(intent, act.getString(R.string.open_file_tips)),
                    FILE_SELECT_CODE);
        } catch (android.content.ActivityNotFoundException ex) {
            // Potentially direct the user to the Market with a Dialog
            Toast.makeText(act, act.getText(R.string.file_select_tool_tips), Toast.LENGTH_SHORT)
                    .show();
        }
    }

    //检测是否是二进制文件
    public static boolean isBinderFile(File file) {
        boolean isBinary = false;
        FileInputStream fin = null;
        try {
            fin = new FileInputStream(file);
            long len = file.length();
            for (int j = 0; j < (int) len; j++) {
                int t = fin.read();
                if (t < 32 && t != 9 && t != 10 && t != 13) {
                    isBinary = true;
                    break;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (fin != null)
                    fin.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return isBinary;
    }

    //检测是否是MP4文件
    public static boolean isValidMp4File(File file){
        try {
            FileInputStream in = new FileInputStream(file);
            byte[] aacFlag = new byte[8];
            in.read(aacFlag, 0, 8);
            if (aacFlag[4] == 'f' && aacFlag[5] == 't' && aacFlag[6] == 'y' && aacFlag[7] == 'p') {
                in.close();
                Log.d(TAG, "===zhongjihao===isValidMp4File===[4]: "+aacFlag[4]+"   [5]: "+aacFlag[5]+"  [6]: "+aacFlag[6]+"  [7]: "+aacFlag[7]);
                return true;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    //检测是否是AAC文件
    public static boolean isValidAacFile(File file){
        try {
            FileInputStream in = new FileInputStream(file);
            byte[] aacFlag = new byte[2];
            in.read(aacFlag, 0, 2);
            if (((aacFlag[0] & 0xFF) == 0xFF) && ((aacFlag[1] & 0xF0) == 0xF0)) {
                in.close();
                Log.d(TAG, "===zhongjihao===isValidAacFile===[0]: "+aacFlag[0]+"  [1]: "+aacFlag[1]);
                return true;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    public static void showSoftInfo(View Anchors,Context context) {
        // 用于PopupWindow的View
        View contentView = LayoutInflater.from(context).inflate(R.layout.about_info_view, null, false);
        // 创建PopupWindow对象，其中：
        // 第一个参数是用于PopupWindow中的View，第二个参数是PopupWindow的宽度，
        // 第三个参数是PopupWindow的高度，第四个参数指定PopupWindow能否获得焦点
        PopupWindow window = new PopupWindow(contentView, context.getResources().getDisplayMetrics().widthPixels/2, context.getResources().getDisplayMetrics().heightPixels/3, true);
        // 设置PopupWindow的背景
        window.setBackgroundDrawable(new ColorDrawable(Color.GREEN));
        // 设置PopupWindow是否能响应外部点击事件
        window.setOutsideTouchable(true);
        // 设置PopupWindow是否能响应点击事件
        window.setTouchable(true);
        window.showAtLocation(Anchors,  Gravity.BOTTOM, 10, 90);
    }

    public static String getPathFromURI(Context context, Uri contentURI) {
        String result = null;
        Cursor cursor = null;
        try {
            Log.d(TAG, "===zhongjihao====1==contentURI: "+contentURI.toString());
            cursor = context.getContentResolver().query(contentURI,
                    new String[]{ MediaStore.MediaColumns.DATA},
                    null, null, null);
            Log.d(TAG, "===zhongjihao=====2===contentURI: "+contentURI.toString()+"  cursor: "+(cursor !=null));
            if (cursor != null && cursor.moveToFirst()) {
                int index = cursor.getColumnIndexOrThrow( MediaStore.MediaColumns.DATA);
                result = cursor.getString(index);
            }
        } catch (Exception e) {
            Log.e(TAG,"=====zhongjihao======Except: "+new Exception(e));
            e.printStackTrace();
        } finally {
            if (cursor != null)
                cursor.close();
        }
        return result;
    }

    /**
     * 获取应用程序名称
     */
    public synchronized static String getAppName(Context context) {
        try {
            PackageManager packageManager = context.getPackageManager();
            PackageInfo packageInfo = packageManager.getPackageInfo(
                    context.getPackageName(), 0);
            int labelRes = packageInfo.applicationInfo.labelRes;
            return context.getResources().getString(labelRes);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }
}
