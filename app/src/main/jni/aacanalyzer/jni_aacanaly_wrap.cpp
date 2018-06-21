/*************************************************************************
    > File Name: jni_aacanaly_wrap.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: 2018年06月08日 星期五 15时52分03秒
 ************************************************************************/

#define LOG_TAG "AACAnalyze-Jni"

#include <jni.h>
#include <stdio.h>
#include <vector>
#include "./faad2-2.7/frontend/aac_decode.h"

#include "../log.h"

//定义java中类全名
static const char* classPathName = "spreadtrum/zhongjihao/aacanaiysisandroid/AacAnalyzeNativeJni";

static JavaVM* g_vm = NULL;

struct AacFrameListNativeJNI{
    jclass     aacitem_cls;
    jmethodID  constructionID;
    jmethodID  setSizeID;
    jmethodID  setSampleNumID;
    jmethodID  setDurationID;
};

static JNIEnv* getJNIEnv(bool* needsDetach)
{
    JNIEnv* env = NULL;
    if(g_vm ->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        int status = g_vm ->AttachCurrentThread(&env, NULL);
        if(status < 0)
        {
            LOGE("%s: ====zhongjihao=====failed to attach current thread=======",__FUNCTION__);
            return NULL;
        }
        *needsDetach = true;
    }
    return env;
}

static void basicAudioInfoCallback(map<string, string>& containerInfo,map<string, string>& metaDataInfo)
{
    bool needsDetach = false;
    JNIEnv * env = getJNIEnv(&needsDetach);
    int containerCount = containerInfo.size();
    int metaDataCount = metaDataInfo.size();
    if(env != NULL){
        jclass map_cls = env->FindClass("java/util/HashMap");//获得HashMap类引用
        LOGD("%s: ===zhongjihao===1===containerCount: %d   metaDataCount: %d",__FUNCTION__,containerCount,metaDataCount);
        jmethodID map_construct = env->GetMethodID(map_cls,"<init>","()V");
        LOGD("%s: ===zhongjihao===2===containerCount: %d   metaDataCount: %d",__FUNCTION__,containerCount,metaDataCount);
        jobject map_containerObj = env->NewObject(map_cls,map_construct,"");
        jobject map_metaDataObj = env->NewObject(map_cls,map_construct,"");
        LOGD("%s: ===zhongjihao===3===containerCount: %d   metaDataCount: %d",__FUNCTION__,containerCount,metaDataCount);
        jmethodID map_put = env->GetMethodID(map_cls,"put","(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
        LOGD("%s: ===zhongjihao===4===containerCount: %d   metaDataCount: %d",__FUNCTION__,containerCount,metaDataCount);
        map<string, string>::iterator iter = containerInfo.begin();
        while (iter != containerInfo.end()) {
            jstring jkey = env->NewStringUTF((iter->first).c_str());
            jstring jval = env->NewStringUTF((iter->second).c_str());
            LOGD("%s: ===zhongjihao======container===key: %s   val: %s",__FUNCTION__,(iter->first).c_str(),(iter->second).c_str());
            env->CallObjectMethod(map_containerObj, map_put, jkey, jval);
            env->DeleteLocalRef(jkey);
            env->DeleteLocalRef(jval);

            iter++;
        }

        iter = metaDataInfo.begin();
        while (iter != metaDataInfo.end()) {
            jstring jkey = env->NewStringUTF((iter->first).c_str());
            jstring jval = env->NewStringUTF((iter->second).c_str());
            LOGD("%s: ===zhongjihao======metaData===key: %s   val: %s",__FUNCTION__,(iter->first).c_str(),(iter->second).c_str());
            env->CallObjectMethod(map_metaDataObj, map_put, jkey, jval);
            env->DeleteLocalRef(jkey);
            env->DeleteLocalRef(jval);

            iter++;
        }
        LOGD("%s: ===zhongjihao===5===containerCount: %d   metaDataCount: %d",__FUNCTION__,containerCount,metaDataCount);
        jclass cb_cls = env->FindClass("spreadtrum/zhongjihao/aacanaiysisandroid/BasicAudioInfoCb");
        LOGD("%s: ===zhongjihao===6===cb_cls: %d",__FUNCTION__,(cb_cls!=NULL));
        //获取java类BasicAudioInfoCb的静态函数basicAudioInfo的方法ID
        jmethodID cb_mid = env->GetStaticMethodID(cb_cls,"basicAudioInfo","(Ljava/util/HashMap;Ljava/util/HashMap;)V");
        LOGD("%s: ===zhongjihao===7===cb_mid: %d",__FUNCTION__,(cb_mid!=NULL));
        env->CallStaticVoidMethod(cb_cls,cb_mid,map_containerObj,map_metaDataObj);
     //   env->CallStaticVoidMethod(g_BasicInfoCb.basicInfoCb_cls,g_BasicInfoCb.basicAudioInfoID,map_containerObj,map_metaDataObj);
        env->DeleteLocalRef(cb_cls);
        LOGD("%s: ===zhongjihao===end=====",__FUNCTION__);
    }
    if (needsDetach) {
        g_vm->DetachCurrentThread();
    }
}

static void initAacFrameListNativeJNI(JNIEnv* env,struct AacFrameListNativeJNI& aacFrameNativeJNI ,const char* className)
{
    //根据java类NaluItem获取在jni中的类表示形式
    aacFrameNativeJNI.aacitem_cls = env->FindClass(className);//FindClass获取的是局部引用,离开作用域会被系统自动回收
    if(aacFrameNativeJNI.aacitem_cls == NULL){
        LOGE("%s: ======not find java class AudioFrame======",__FUNCTION__);
        return;
    }

    //获取java类AudioFrame的构造函数ID
    if(aacFrameNativeJNI.constructionID == NULL){
        aacFrameNativeJNI.constructionID = env->GetMethodID(aacFrameNativeJNI.aacitem_cls, "<init>", "()V");
        if(aacFrameNativeJNI.constructionID == NULL){
            LOGE("%s: ==not get java class AudioFrame constructionID======",__FUNCTION__);
            return;
        }
    }

    //获取JAVA类AudioFrame中setSize方法ID
    if(aacFrameNativeJNI.setSizeID == NULL){
        aacFrameNativeJNI.setSizeID = env->GetMethodID(aacFrameNativeJNI.aacitem_cls,"setSize","(I)V");
        if(aacFrameNativeJNI.setSizeID == NULL){
            LOGE("%s: =======not get java class AudioFrame setSize======",__FUNCTION__);
            return;
        }
    }

    //获取JAVA类AudioFrame中setSampleNum方法ID
    if(aacFrameNativeJNI.setSampleNumID == NULL){
        aacFrameNativeJNI.setSampleNumID = env->GetMethodID(aacFrameNativeJNI.aacitem_cls,"setSampleNum","(I)V");
        if(aacFrameNativeJNI.setSampleNumID == NULL){
            LOGE("%s: =======not get java class AudioFrame setSampleNum======",__FUNCTION__);
            return;
        }
    }

    //获取JAVA类AudioFrame中setDuration方法ID
    if(aacFrameNativeJNI.setDurationID == NULL){
        aacFrameNativeJNI.setDurationID = env->GetMethodID(aacFrameNativeJNI.aacitem_cls,"setDuration","(I)V");
        if(aacFrameNativeJNI.setDurationID == NULL){
            LOGE("%s: =======not get java class AudioFrame setDuration======",__FUNCTION__);
            return;
        }
    }
}

static void decodeFrameProgress(int progressPercent)
{
    bool needsDetach = false;
    JNIEnv * env = getJNIEnv(&needsDetach);
    if(env != NULL){
        LOGD("%s: ===zhongjihao======progressPercent: %d",__FUNCTION__,progressPercent);
        jclass cb_cls = env->FindClass("spreadtrum/zhongjihao/aacanaiysisandroid/BasicAudioInfoCb");
        // LOGD("%s: ===zhongjihao======cb_cls: %d",__FUNCTION__,(cb_cls!=NULL));
        //获取java类BasicAudioInfoCb的静态函数decodeFrameProgress的方法ID
        jmethodID cb_mid = env->GetStaticMethodID(cb_cls,"decodeFrameProgress","(I)V");
        // LOGD("%s: ===zhongjihao======cb_mid: %d",__FUNCTION__,(cb_mid!=NULL));
        env->CallStaticVoidMethod(cb_cls,cb_mid,(jint)progressPercent);
        env->DeleteLocalRef(cb_cls);
    }
    if (needsDetach) {
        g_vm->DetachCurrentThread();
    }
}

static void aacFrameInfo(AudioFrame* pAacFrame)
{
    bool needsDetach = false;
    JNIEnv * env = getJNIEnv(&needsDetach);
    if(env != NULL){
        struct AacFrameListNativeJNI aacFramelistNativeJNI;
        initAacFrameListNativeJNI(env,aacFramelistNativeJNI,"spreadtrum/zhongjihao/aacanaiysisandroid/AudioFrame");
        //创建java对象AudioFrame
        jobject aacFrame = env->NewObject(aacFramelistNativeJNI.aacitem_cls,aacFramelistNativeJNI.constructionID);
        //给java对象AudioFrame成员size赋值
        env->CallVoidMethod(aacFrame,aacFramelistNativeJNI.setSizeID,(jint)pAacFrame->size);
        //给java对象AudioFrame成员sampleNum赋值
        env->CallVoidMethod(aacFrame,aacFramelistNativeJNI.setSampleNumID,(jint)pAacFrame->sampleNum);
        //给java对象AudioFrame成员duration赋值
        env->CallVoidMethod(aacFrame,aacFramelistNativeJNI.setDurationID,(jint)pAacFrame->duration);

        jclass cb_cls = env->FindClass("spreadtrum/zhongjihao/aacanaiysisandroid/BasicAudioInfoCb");
       // LOGD("%s: ===zhongjihao===2===cb_cls: %d",__FUNCTION__,(cb_cls!=NULL));
        //获取java类BasicAudioInfoCb的静态函数aacFrameInfo的方法ID
        jmethodID cb_mid = env->GetStaticMethodID(cb_cls,"aacFrameInfo","(Lspreadtrum/zhongjihao/aacanaiysisandroid/AudioFrame;)V");
       // LOGD("%s: ===zhongjihao===3===cb_mid: %d",__FUNCTION__,(cb_mid!=NULL));
        env->CallStaticVoidMethod(cb_cls,cb_mid,aacFrame);
        env->DeleteLocalRef(cb_cls);
      //  LOGD("%s: ===zhongjihao===end=====",__FUNCTION__);

        env->DeleteLocalRef(aacFrame);
        env->DeleteLocalRef(aacFramelistNativeJNI.aacitem_cls);
    }
    if (needsDetach) {
        g_vm->DetachCurrentThread();
    }
}

jlong JNI_startFaad2Engine(JNIEnv *env,jclass jcls __unused,jstring jproName)
{
    const char* proName = env ->GetStringUTFChars(jproName, NULL);
    AacDecode* pAacDec =  new AacDecode(proName);
    pAacDec->setBasicInfoCallback(basicAudioInfoCallback);
    pAacDec->setAacFrameCallback(aacFrameInfo);
    pAacDec->setProgressCallback(decodeFrameProgress);
    env ->ReleaseStringUTFChars(jproName, proName);
    return reinterpret_cast<long> (pAacDec);
}

jint JNI_decodeAacfile(JNIEnv *env,jclass jcls __unused,jlong jcPtr,jstring jaacfile,jstring joutfile,jstring jadts_fn,jint to_stdout,jint def_srate,jint object_type,jint outputFormat, jint fileType,
                       jint downMatrix, jint infoOnly, jint adts_out, jint old_format)
{
    AacDecode* pAacDec = reinterpret_cast<AacDecode *> (jcPtr);
    const char* aacfile = env ->GetStringUTFChars(jaacfile, NULL);
    const char* outfile = NULL;
    const char* adts_fn = NULL;
    if(joutfile != NULL)
        outfile = env ->GetStringUTFChars(joutfile, NULL);
    if(jadts_fn != NULL)
        adts_fn = env ->GetStringUTFChars(jadts_fn, NULL);
    pAacDec->decodeAACfile(aacfile,outfile,adts_fn,(int)to_stdout,(int)def_srate,(int)object_type,(int)outputFormat,(int)fileType,(int)downMatrix,(int)infoOnly,(int)adts_out,(int)old_format);
    env ->ReleaseStringUTFChars(jaacfile, aacfile);
    if(joutfile != NULL)
        env ->ReleaseStringUTFChars(joutfile, outfile);
    if(jadts_fn)
        env ->ReleaseStringUTFChars(jadts_fn, adts_fn);
    return 0;
}

jint JNI_decodeMp4file(JNIEnv *env,jclass jcls __unused,jlong jcPtr,jstring jmp4file,jstring joutfile,jstring jadts_fn,jint to_stdout,jint outputFormat, jint fileType,
                       jint downMatrix, jint noGapless,jint infoOnly,jint adts_out)
{
    AacDecode* pAacDec = reinterpret_cast<AacDecode *> (jcPtr);
    const char* mp4File = env ->GetStringUTFChars(jmp4file, NULL);
    const char* outfile = NULL;
    const char* adts_fn = NULL;
    if(joutfile != NULL)
        outfile = env ->GetStringUTFChars(joutfile, NULL);
    if(jadts_fn != NULL)
        adts_fn = env ->GetStringUTFChars(jadts_fn, NULL);
    pAacDec->decodeMP4file(mp4File,outfile,adts_fn,(int)to_stdout,(int)outputFormat, (int)fileType,(int)downMatrix, (int)noGapless,(int)infoOnly,(int)adts_out);
    env ->ReleaseStringUTFChars(jmp4file, mp4File);
    if(joutfile != NULL)
        env ->ReleaseStringUTFChars(joutfile, outfile);
    if(jadts_fn != NULL)
        env ->ReleaseStringUTFChars(jadts_fn, adts_fn);
    return 0;
}

void JNI_setCheckBox(JNIEnv *env,jclass jcls __unused,jlong jcPtr,jboolean jisCheck,jint jshowNalCount)
{
    AacDecode* pAacDec = reinterpret_cast<AacDecode *> (jcPtr);
    pAacDec->setCheckBox((bool)jisCheck,(int)jshowNalCount);
}

void JNI_stopFaad2Engine(JNIEnv *env,jclass jcls __unused,jlong jcPtr)
{
    AacDecode* pAacDec = reinterpret_cast<AacDecode *> (jcPtr);
    delete pAacDec;
}

//JAVA函数和C++函数映射关系表
static JNINativeMethod gMethods[] = {
        { "startFaad2Engine","(Ljava/lang/String;)J",(void*)JNI_startFaad2Engine},
        { "decodeAACfile","(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;IIIIIIIII)I",(void*)JNI_decodeAacfile},
        { "decodeMP4file","(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;IIIIIII)I",(void*)JNI_decodeMp4file},
        { "setCheckBox","(JZI)V",(void*)JNI_setCheckBox},
        { "stopFaad2Engine","(J)V",(void*)JNI_stopFaad2Engine},
};

typedef union{
    JNIEnv* env;
    void* venv;
}UnionJNIEnvToVoid;

static jint registerNativeMethods(JNIEnv* env,const char* className,JNINativeMethod* gMethods,int numMethods)
{
    jclass clazz;
    clazz = env ->FindClass(className);

    if(clazz == NULL){
        return JNI_FALSE;
    }

    if(env->RegisterNatives(clazz,gMethods,numMethods) < 0){
        env ->DeleteLocalRef(clazz);
        return JNI_FALSE;
    }

    env ->DeleteLocalRef(clazz);
    return JNI_TRUE;
}


static jint registerNatives(JNIEnv* env,const char* classPathName,JNINativeMethod *gMethods,int methodsNum)
{
    if(!registerNativeMethods(env,classPathName,gMethods,methodsNum)){
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

//JNI库加载时自动调用该函数
jint JNI_OnLoad(JavaVM* vm, void* reserved __unused)
{
    g_vm = vm;
    UnionJNIEnvToVoid uenv;
    JNIEnv* env = NULL;

    LOGD("%s: ==zhongjihao========1===",__FUNCTION__);
    //获得JNI版本
    if(vm->GetEnv((void**)&uenv.venv,JNI_VERSION_1_4) != JNI_OK){
        return -1;
    }

    LOGD("%s: ===zhongjihao======2====",__FUNCTION__);
    env = uenv.env;
    //注册java函数
    if(registerNatives(env,classPathName,gMethods,sizeof(gMethods)/sizeof(gMethods[0])) != JNI_TRUE){
        return -1;
    }

    LOGD("%s: ===zhongjihao======3===",__FUNCTION__);
    return JNI_VERSION_1_4;
}
