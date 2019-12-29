#include <jni.h>
#include <string>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "RecordBuffer.h"
#include "AndroidLog.h"


SL_API SLresult SLAPIENTRY slCreateEngine(
        SLObjectItf             *pEngine,           //对象地址，用于传出对象
        SLuint32                numOptions,         //配置参数数量
        const SLEngineOption    *pEngineOptions,    //配置参数，为枚举数组
        SLuint32                numInterfaces,      //支持的接口数量
        const SLInterfaceID     *pInterfaceIds,     //具体的要支持的接口，是枚举的数组
        const SLboolean         *pInterfaceRequired //具体的要支持的接口是开放的还是关闭的，也是一个数组，这三个参数长度是一致的
);

SLObjectItf  slObjectEngine = NULL;//用SLObjectltf声明引擎接口对象
SLEngineItf engineItf = NULL;//声明具体的引擎对象实例
SLObjectItf  recordObj = NULL;//用SLObjectltf声明引擎接口对象
SLRecordItf recordItf = NULL;


SLAndroidSimpleBufferQueueItf  recorderBufferQueue = NULL;//Buffer接口

RecordBuffer *recordBuffer;

FILE *pcmFile = NULL;

bool finish = false;
//取出缓冲数据回调
void bgRecorderCallback(SLAndroidSimpleBufferQueueItf bg, void *context){
    if (NULL != recordBuffer){
        fwrite(recordBuffer->getNowBuffer(),1,4096,pcmFile);
    }

    if (finish)
    {
        (*recordItf)->SetRecordState(recordItf,SL_RECORDSTATE_STOPPED);
        //刷新缓冲区后，关闭流
        fclose(pcmFile);
        //释放内存
        delete  recordBuffer;
        recordBuffer = NULL;

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
    //第二步：实现(Realize)engineObject,SL_BOOLEAN_FALSE);实例化这个对象
    (*slObjectEngine)->Realize(slObjectEngine,SL_BOOLEAN_FALSE);
    //第三步：通过engineObject的GetInterface方法初始化enngineEngine,从这个对象里面获取引擎接口
    (*slObjectEngine)->GetInterface(slObjectEngine,SL_IID_ENGINE,&engineItf);



    //4. 设置IO设备(麦克风)
    SLDataLocator_IODevice loc_dev = {
            SL_DATALOCATOR_IODEVICE,//类型
            SL_IODEVICE_AUDIOINPUT,//device类型 选择了音频输入类型
            SL_DEFAULTDEVICEID_AUDIOINPUT,//deviceID
            NULL            //device实例
    };

    SLDataSource audioStr = {
            &loc_dev, //SLDataLocator_IODevice配置输入
            NULL      //输入格式,采集的并不需要
    };
    //5. 设置输出buffer队列
    SLDataLocator_AndroidSimpleBufferQueue loc_bq = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,//类型 这里只能是这个常量
            2,//buffer的数量

    };
    //6. 设置输出数据的格式
    SLDataFormat_PCM format_pcm = {
            SL_DATAFORMAT_PCM,////输出PCM格式的数据
            (SLuint32)2,//输出的声道数量
            SL_SAMPLINGRATE_44_1,//输出的采样频率，这里是44100Hz
            SL_PCMSAMPLEFORMAT_FIXED_16,//输出的采样格式，这里是16bit
            SL_PCMSAMPLEFORMAT_FIXED_16,//一般来说，跟随上一个参数
            SL_SPEAKER_FRONT_LEFT |
            SL_SPEAKER_FRONT_RIGHT,//双声道配置，如果单声道可以用 SL_SPEAKER_FRONT_CENTER
            SL_BYTEORDER_LITTLEENDIAN//PCM数据的大小端排列
    };

    SLDataSink audioSink = {
            &loc_bq,     //SLDataFormat_PCM配置输出
            &format_pcm  //输出数据格式
    };


    //7. 创建录制的对象
    const SLInterfaceID  id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};

    //创建录音器
    (*engineItf)->CreateAudioRecorder(
            engineItf,  //引擎接口
            &recordObj, //录制对象地址，用于传出对象
            &audioStr,  //输入配置
            &audioSink,  //输出配置
            1,            //支持的接口数量
            id,          //具体的要支持的接口
            req          //具体的要支持的接口是开放的还是关闭的
    );
    ////8. 实例化这个录制对象
    (*recordObj)->Realize(recordObj,SL_BOOLEAN_FALSE);
    //9. 获取录制接口
    (*recordObj)->GetInterface(recordObj,SL_IID_RECORD,&recordItf);
    //10. 获取Buffer接口
    (*recordObj)->GetInterface(recordObj,SL_IID_ANDROIDSIMPLEBUFFERQUEUE,&recorderBufferQueue);

    (*recorderBufferQueue)->Enqueue(recorderBufferQueue,recordBuffer->getRecordBuffer(),4096);

    (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue,bgRecorderCallback,NULL);
    //11. 开始录音
    (*recordItf)->SetRecordState(recordItf,SL_RECORDSTATE_RECORDING);

    env->ReleaseStringUTFChars(path_, path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_will_openslesrecord_MainActivity_stopRecord(JNIEnv *env, jobject instance) {

    if (NULL != recordItf) {
        finish = true;
    }

}