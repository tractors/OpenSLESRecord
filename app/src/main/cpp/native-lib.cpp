#include <jni.h>
#include <string>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "RecordBuffer.h"
#include "AndroidLog.h"

SLObjectItf  slObjectEngine = NULL;//用SLObjectltf声明引擎接口对象
SLEngineItf engineItf = NULL;//声明具体的引擎对象实例
SLObjectItf  recordObj = NULL;//用SLObjectltf声明引擎接口对象
SLRecordItf recordItf = NULL;
//SLEngineltf engineEngine = NULL;//声明具体的引擎对象实例

SLAndroidSimpleBufferQueueItf  recorderBufferQueue = NULL;

RecordBuffer *recordBuffer;

FILE *pcmFile = NULL;

bool finish = false;
//取出缓冲数据回调
void bgRecorderCallback(SLAndroidSimpleBufferQueueItf bg, void *context){
    fwrite(recordBuffer->getNowBuffer(),1,4096,pcmFile);
    if (finish)
    {
        (*recordItf)->SetRecordState(recordItf,SL_RECORDSTATE_STOPPED);
        //资源的释放
        //销毁
        (*slObjectEngine)->Destroy(slObjectEngine);
        LOGE("录制完成");
    } else {
        (*recorderBufferQueue)->Enqueue(recorderBufferQueue,recordBuffer->getRecordBuffer(),4096);
        LOGE("正在录制");
    };
}

extern "C"
JNIEXPORT void JNICALL
Java_com_will_openslesrecord_MainActivity_startRecord(JNIEnv *env, jobject instance,
                                                      jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    finish = false;
    pcmFile = fopen(path,"w");
    recordBuffer = new RecordBuffer(4096);
    //第一步：创建引擎
    slCreateEngine(&slObjectEngine,0,NULL,0,NULL,NULL);
    //第二步：实现(Realize)engineObject,SL_BOOLEAN_FALSE);
    (*slObjectEngine)->Realize(slObjectEngine,SL_BOOLEAN_FALSE);
    //第三步：通过engineObject的GetInterface方法初始化enngineEngine
    (*slObjectEngine)->GetInterface(slObjectEngine,SL_IID_ENGINE,&engineItf);


    //设置混音器；
//    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
//    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
//    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
//    (void)result;
//    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
//    (void)result;
//    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
//    if (SL_RESULT_SUCCESS == result) {
//        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
//                outputMixEnvironmentalReverb, &reverbSettings);
//        (void)result;
//    }
//    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};


    SLDataLocator_IODevice loc_dev = {
            SL_DATALOCATOR_IODEVICE,
            SL_IODEVICE_AUDIOINPUT,
            SL_DEFAULTDEVICEID_AUDIOINPUT,
            NULL};

    SLDataSource audioStr = {&loc_dev,NULL};
    // 创建播放器
    SLDataLocator_AndroidSimpleBufferQueue loc_bq = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            2,

    };

    SLDataFormat_PCM format_pcm = {
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            2,//2个声道（立体声）
            SL_SAMPLINGRATE_44_1,//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };

    SLDataSink audioSink = {&loc_bq,&format_pcm};



    const SLInterfaceID  id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};

    //创建录音器
    (*engineItf)->CreateAudioRecorder(engineItf,&recordObj,&audioStr,&audioSink,1,id,req);
    //实现
    (*recordObj)->Realize(recordObj,SL_BOOLEAN_FALSE);
    (*recordObj)->GetInterface(recordObj,SL_IID_RECORD,&recordItf);

    (*recordObj)->GetInterface(recordObj,SL_IID_ANDROIDSIMPLEBUFFERQUEUE,&recorderBufferQueue);

    (*recorderBufferQueue)->Enqueue(recorderBufferQueue,recordBuffer->getRecordBuffer(),4096);

    (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue,bgRecorderCallback,NULL);

    (*recordItf)->SetRecordState(recordItf,SL_RECORDSTATE_RECORDING);

    env->ReleaseStringUTFChars(path_, path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_will_openslesrecord_MainActivity_stopRecord(JNIEnv *env, jobject instance) {

    finish = true;

}