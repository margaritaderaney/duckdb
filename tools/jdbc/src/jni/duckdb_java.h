/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class nl_cwi_da_duckdb_DuckDBNative */

#ifndef _Included_nl_cwi_da_duckdb_DuckDBNative
#define _Included_nl_cwi_da_duckdb_DuckDBNative
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     nl_cwi_da_duckdb_DuckDBNative
 * Method:    duckdb_jdbc_startup
 * Signature: (Ljava/lang/String;Z)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1startup
  (JNIEnv *, jclass, jstring, jboolean);

/*
 * Class:     nl_cwi_da_duckdb_DuckDBNative
 * Method:    duckdb_jdbc_shutdown
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1shutdown
  (JNIEnv *, jclass, jobject);

/*
 * Class:     nl_cwi_da_duckdb_DuckDBNative
 * Method:    duckdb_jdbc_connect
 * Signature: (Ljava/nio/ByteBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1connect
  (JNIEnv *, jclass, jobject);

/*
 * Class:     nl_cwi_da_duckdb_DuckDBNative
 * Method:    duckdb_jdbc_disconnect
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1disconnect
  (JNIEnv *, jclass, jobject);

/*
 * Class:     nl_cwi_da_duckdb_DuckDBNative
 * Method:    duckdb_jdbc_prepare
 * Signature: (Ljava/nio/ByteBuffer;Ljava/lang/String;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1prepare
  (JNIEnv *, jclass, jobject, jstring);

/*
 * Class:     nl_cwi_da_duckdb_DuckDBNative
 * Method:    duckdb_jdbc_execute
 * Signature: (Ljava/nio/ByteBuffer;[Ljava/lang/Object;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1execute
  (JNIEnv *, jclass, jobject, jobjectArray);

/*
 * Class:     nl_cwi_da_duckdb_DuckDBNative
 * Method:    duckdb_jdbc_release
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1release
  (JNIEnv *, jclass, jobject);

/*
 * Class:     nl_cwi_da_duckdb_DuckDBNative
 * Method:    duckdb_jdbc_free_result
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1free_1result
  (JNIEnv *, jclass, jobject);

/*
 * Class:     nl_cwi_da_duckdb_DuckDBNative
 * Method:    duckdb_jdbc_meta
 * Signature: (Ljava/nio/ByteBuffer;)Lnl/cwi/da/duckdb/DuckDBResultSetMetaData;
 */
JNIEXPORT jobject JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1meta
  (JNIEnv *, jclass, jobject);

#ifdef __cplusplus
}
#endif
#endif
