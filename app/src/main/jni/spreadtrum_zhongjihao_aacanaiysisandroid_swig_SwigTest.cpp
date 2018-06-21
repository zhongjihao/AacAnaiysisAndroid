//
// Created by SPREADTRUM\jihao.zhong on 18-5-3.
//

#include "spreadtrum_zhongjihao_aacanaiysisandroid_swig_SwigTest.h"

JNIEXPORT jstring JNICALL Java_spreadtrum_zhongjihao_aacanaiysisandroid_swig_SwigTest_SayHello
  (JNIEnv *env, jclass jcls)
{
    return env ->NewStringUTF("SWIG测试");
}
