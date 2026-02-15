#include <jni.h>
#include <stdbool.h>
#include <stdlib.h>
#include "n2n.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

typedef struct {
    n2n_sn_t sn;
    bool keep_running;
} n2n_jni_ctx_t;

JNIEXPORT jint JNICALL Java_org_ntop_n2n_Supernode_init(JNIEnv *env, jobject obj, jint port, jint mgmtPort) {
    n2n_jni_ctx_t *ctx = (n2n_jni_ctx_t *)calloc(1, sizeof(n2n_jni_ctx_t));
    if (!ctx) return -1;

    sn_init_defaults(&ctx->sn);
    ctx->sn.daemon = 0;
    ctx->sn.lport = (uint16_t)port;

    ctx->sn.sock = open_socket(ctx->sn.lport, INADDR_ANY, 0 /* UDP */);
    if (ctx->sn.sock == INVALID_SOCKET) {
        free(ctx);
        return -2;
    }

    ctx->sn.mgmt_sock = open_socket((uint16_t)mgmtPort, INADDR_LOOPBACK, 0 /* UDP */);
    if (ctx->sn.mgmt_sock == INVALID_SOCKET) {
        closesocket(ctx->sn.sock);
        free(ctx);
        return -3;
    }

    sn_init(&ctx->sn);
    ctx->keep_running = true;
    ctx->sn.keep_running = &ctx->keep_running;

    // 将指针存储在 Java 对象中
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "nativePtr", "J");
    (*env)->SetLongField(env, obj, fid, (jlong)ctx);

    return 0;
}

JNIEXPORT jint JNICALL Java_org_ntop_n2n_Supernode_run(JNIEnv *env, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "nativePtr", "J");
    n2n_jni_ctx_t *ctx = (n2n_jni_ctx_t *)(*env)->GetLongField(env, obj, fid);

    if (!ctx) return -1;

    return run_sn_loop(&ctx->sn);
}

JNIEXPORT void JNICALL Java_org_ntop_n2n_Supernode_stop(JNIEnv *env, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "nativePtr", "J");
    n2n_jni_ctx_t *ctx = (n2n_jni_ctx_t *)(*env)->GetLongField(env, obj, fid);

    if (ctx) {
        ctx->keep_running = false;
    }
}

JNIEXPORT void JNICALL Java_org_ntop_n2n_Supernode_destroy(JNIEnv *env, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "nativePtr", "J");
    n2n_jni_ctx_t *ctx = (n2n_jni_ctx_t *)(*env)->GetLongField(env, obj, fid);

    if (ctx) {
        sn_term(&ctx->sn);
        if (ctx->sn.sock != INVALID_SOCKET) closesocket(ctx->sn.sock);
        if (ctx->sn.mgmt_sock != INVALID_SOCKET) closesocket(ctx->sn.mgmt_sock);
        free(ctx);
        (*env)->SetLongField(env, obj, fid, 0);
    }
}
