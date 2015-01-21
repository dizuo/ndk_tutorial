#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jni.h>

#include "native.h"

int global_var = 5;

#ifndef NULL
#define NULL (0)
#endif

/*
 * callback的jni上下文
 */
typedef struct _cbcontext {
	JavaVM *jvm; // java虚拟机
	jobject thiz; // jni对象
} CBContext;

/*
 * 引擎的运行上下文
 */
typedef struct _context {
	void *handle;
	CBContext cbContext;
} Context;

static JNIEnv *getEnv(CBContext *context) {
	JavaVM *jvm = context->jvm;
	JNIEnv *env;
	int status = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);

	switch (status) {
	case JNI_OK:
		return env;
	case JNI_EDETACHED:
		status = jvm->AttachCurrentThread(&env, NULL);
		if (status < 0) {
			return NULL;
		} else {
			return env;
		}
	default:
		return NULL;
	}
}

/*
 * unicode chars 2 jstring
 */
static jstring getJString(JNIEnv *env, unsigned short *cstr, int strLen) {
	if (cstr == NULL || strLen == 0) {
		return NULL;
	}

	return (jstring) env->NewString((const jchar*) cstr, strLen);
}


static jobject callback(void* context, int what, int arg1, jstring arg2)
{
	CBContext* pContext = (CBContext*)context;

	JNIEnv *env = getEnv(pContext);
	jobject obj = pContext->thiz;

	jclass cls = env->GetObjectClass(obj);
	jmethodID javaCallback = env->GetMethodID(cls, "callback", "(IILjava/lang/String;)Ljava/lang/Object;");

	jobject ret = env->CallObjectMethod(obj, javaCallback, what, arg1, arg2);

	if (arg2 != NULL) {
		env->DeleteLocalRef(arg2);
	}

	return ret;
}


// com.dizuo.jni_demo/nativeInit
JNIEXPORT jlong JNICALL Java_com_dizuo_jnidemo_JNI_nativeInit(JNIEnv *env, jobject thiz)
{
	int* engine_handle = &global_var;

	Context* context = new Context();
	context->handle = engine_handle;

	JavaVM *pjvm;
	env->GetJavaVM(&pjvm);
	context->cbContext.jvm = pjvm;
	context->cbContext.thiz = env->NewGlobalRef(thiz);

	return (jlong)context;
}

JNIEXPORT jlong JNICALL Java_com_dizuo_jnidemo_JNI_nativeTestCallback(JNIEnv* env, jobject thiz, jlong handle)
{
	unsigned short nameBuf[] = {35140, 38451, 24066};

	jstring jname = getJString(env, nameBuf, sizeof(nameBuf) / sizeof(nameBuf[0]));
	int type = 1;
	int arg = 888;
	Context* context = (Context*)handle;
	jobject ret = callback(&context->cbContext, type, arg, jname);

	// python2.7.8 us = u'我叫任亚飞' print(us)
	unsigned short nameBuf1[] = {0x6211, 0x53eb, 0x4efb, 0x4e9a, 0x98de};
	jname = getJString(env, nameBuf1, sizeof(nameBuf1) / sizeof(nameBuf1[0]));
	type = 0;
	arg = 111;
	ret = callback(&context->cbContext, type, arg, jname);

	return (jlong)ret;
}
