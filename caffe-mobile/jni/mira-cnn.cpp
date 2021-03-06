#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <string>

#include "caffe/caffe.hpp"
#include "imagenet_test.hpp"

#define  LOG_TAG    "MiRA-CNN"
#define  LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG, __VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

caffe::ImageNet *image_net;

int getTimeSec();

void JNIEXPORT JNICALL
Java_com_sh1r0_caffe_1android_1demo_ImageNet_enableLog(JNIEnv* env, jobject thiz, jboolean enabled)
{
    caffe::LogMessage::Enable(enabled != JNI_FALSE);
}

jint JNIEXPORT JNICALL
Java_com_sh1r0_caffe_1android_1demo_ImageNet_initTest(JNIEnv* env, jobject thiz, jstring modelPath, jstring weightsPath)
{
    const char *model_path = env->GetStringUTFChars(modelPath, 0);
    const char *weights_path = env->GetStringUTFChars(weightsPath, 0);
    image_net = new caffe::ImageNet(string(model_path), string(weights_path));
    env->ReleaseStringUTFChars(modelPath, model_path);
    env->ReleaseStringUTFChars(weightsPath, weights_path);
    return 0;
}

jint JNIEXPORT JNICALL
Java_com_sh1r0_caffe_1android_1demo_ImageNet_runTest(JNIEnv* env, jobject thiz, jstring imgPath)
{
    const char *img_path = env->GetStringUTFChars(imgPath, 0);
    caffe::vector<int> top_k = image_net->predict_top_k(string(img_path), 3);
    LOGD("top-1 result: %d", top_k[0]);

    env->ReleaseStringUTFChars(imgPath, img_path);

    return top_k[0];
}

int getTimeSec() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (int) now.tv_sec;
}
/*
JavaVM *g_jvm = NULL;
jobject g_obj = NULL;

void JNIEXPORT JNICALL
Java_com_sh1r0_caffe_1android_1demo_MainActivity_MainActivity_setJNIEnv(JNIEnv* env, jobject obj)
{
    env->GetJavaVM(&g_jvm);
    g_obj = env->NewGlobalRef(obj);
}
*/
jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        LOGE("GetEnv failed!");
        return result;
    }

    return JNI_VERSION_1_6;
}

int main(int argc, char const *argv[])
{
    string usage("usage: main <model> <weights> <img>");
    if (argc < 4) {
        std::cerr << usage << std::endl;
        return 1;
    }

    caffe::LogMessage::Enable(true); // enable logging
    image_net = new caffe::ImageNet(string(argv[1]), string(argv[2]));
    caffe::vector<int> top_3 = image_net->predict_top_k(string(argv[3]));
    for (auto k : top_3) {
        std::cout << k << std::endl;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif
