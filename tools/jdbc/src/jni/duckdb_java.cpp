#include "nl_cwi_da_duckdb_DuckDBNative.h"
#include "duckdb.hpp"
#include "duckdb/main/client_context.hpp"

using namespace duckdb;
using namespace std;

JNIEXPORT jobject JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1startup(JNIEnv *env, jclass,
                                                                                   jstring database_j,
                                                                                   jboolean read_only) {

	auto *database_c = env->GetStringUTFChars(database_j, 0);
	auto database = string(database_c);
	env->ReleaseStringUTFChars(database_j, database_c);

	auto db = new DuckDB(database);
	return env->NewDirectByteBuffer(db, 0);
}

JNIEXPORT void JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1shutdown(JNIEnv *env, jclass,
                                                                                 jobject db_ref_buf) {
	auto db_ref = (DuckDB *)env->GetDirectBufferAddress(db_ref_buf);
	if (db_ref) {
		delete db_ref;
	}
}

JNIEXPORT jobject JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1connect(JNIEnv *env, jclass,
                                                                                   jobject db_ref_buf) {
	auto db_ref = (DuckDB *)env->GetDirectBufferAddress(db_ref_buf);
	auto conn = new Connection(*db_ref);
	return env->NewDirectByteBuffer(conn, 0);
}

JNIEXPORT void JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1set_1auto_1commit(JNIEnv *env, jclass,
                                                                                          jobject conn_ref_buf,
                                                                                          jboolean auto_commit) {
	auto conn_ref = (Connection *)env->GetDirectBufferAddress(conn_ref_buf);
	if (!conn_ref) {
		jclass Exception = env->FindClass("java/sql/SQLException");
		env->ThrowNew(Exception, "Invalid connection");
	}
	conn_ref->context->transaction.SetAutoCommit(auto_commit);
}

JNIEXPORT jboolean JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1get_1auto_1commit(JNIEnv *env, jclass,
                                                                                              jobject conn_ref_buf) {
	auto conn_ref = (Connection *)env->GetDirectBufferAddress(conn_ref_buf);
	if (!conn_ref) {
		jclass Exception = env->FindClass("java/sql/SQLException");
		env->ThrowNew(Exception, "Invalid connection");
	}
	return conn_ref->context->transaction.IsAutoCommit();
}

JNIEXPORT void JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1disconnect(JNIEnv *env, jclass,
                                                                                   jobject conn_ref_buf) {
	auto conn_ref = (Connection *)env->GetDirectBufferAddress(conn_ref_buf);
	if (conn_ref) {
		delete conn_ref;
	}
}

struct StatementHolder {
	unique_ptr<PreparedStatement> stmt;
};

JNIEXPORT jobject JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1prepare(JNIEnv *env, jclass,
                                                                                   jobject conn_ref_buf,
                                                                                   jstring query_j) {
	auto conn_ref = (Connection *)env->GetDirectBufferAddress(conn_ref_buf);
	if (!conn_ref) {
		jclass Exception = env->FindClass("java/sql/SQLException");
		env->ThrowNew(Exception, "Invalid connection");
	}

	auto *query_c = env->GetStringUTFChars(query_j, 0);
	auto query = string(query_c);
	env->ReleaseStringUTFChars(query_j, query_c);

	auto stmt_ref = new StatementHolder();
	stmt_ref->stmt = conn_ref->Prepare(query);
	if (!stmt_ref->stmt->success) {
		jclass Exception = env->FindClass("java/sql/SQLException");
		string error_msg = string(stmt_ref->stmt->error);
		stmt_ref->stmt = nullptr;
		env->ThrowNew(Exception, error_msg.c_str());
	}
	return env->NewDirectByteBuffer(stmt_ref, 0);
}

struct ResultHolder {
	unique_ptr<QueryResult> res;
	unique_ptr<DataChunk> chunk;
};

JNIEXPORT jobject JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1execute(JNIEnv *env, jclass,
                                                                                   jobject stmt_ref_buf,
                                                                                   jobjectArray params) {
	auto stmt_ref = (StatementHolder *)env->GetDirectBufferAddress(stmt_ref_buf);
	if (!stmt_ref) {
		jclass Exception = env->FindClass("java/sql/SQLException");
		env->ThrowNew(Exception, "Invalid statement");
	}
	auto res_ref = new ResultHolder();
	res_ref->res = stmt_ref->stmt->Execute();
	if (!res_ref->res->success) {
		jclass Exception = env->FindClass("java/sql/SQLException");
		string error_msg = string(res_ref->res->error);
		res_ref->res = nullptr;
		env->ThrowNew(Exception, error_msg.c_str());
	}
	return env->NewDirectByteBuffer(res_ref, 0);
}

JNIEXPORT void JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1release(JNIEnv *env, jclass,
                                                                                jobject stmt_ref_buf) {
	auto stmt_ref = (StatementHolder *)env->GetDirectBufferAddress(stmt_ref_buf);
	if (stmt_ref) {
		delete stmt_ref;
	}
}

JNIEXPORT void JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1free_1result(JNIEnv *env, jclass,
                                                                                     jobject res_ref_buf) {
	auto res_ref = (ResultHolder *)env->GetDirectBufferAddress(res_ref_buf);
	if (res_ref) {
		delete res_ref;
	}
}

JNIEXPORT jobject JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1meta(JNIEnv *env, jclass,
                                                                                jobject res_ref_buf) {
	auto res_ref = (ResultHolder *)env->GetDirectBufferAddress(res_ref_buf);
	if (!res_ref || !res_ref->res) {
		jclass Exception = env->FindClass("java/sql/SQLException");
		env->ThrowNew(Exception, "Invalid result set");
	}

	jclass meta = env->FindClass("nl/cwi/da/duckdb/DuckDBResultSetMetaData");
	jmethodID meta_construct = env->GetMethodID(meta, "<init>", "(I[Ljava/lang/String;[Ljava/lang/String;)V");

	auto column_count = res_ref->res->names.size();

	auto name_array = env->NewObjectArray(column_count, env->FindClass("java/lang/String"), nullptr);
	auto type_array = env->NewObjectArray(column_count, env->FindClass("java/lang/String"), nullptr);

	for (idx_t col_idx = 0; col_idx < column_count; col_idx++) {
		env->SetObjectArrayElement(name_array, col_idx, env->NewStringUTF(res_ref->res->names[col_idx].c_str()));
		env->SetObjectArrayElement(type_array, col_idx,
		                           env->NewStringUTF(SQLTypeToString(res_ref->res->sql_types[col_idx]).c_str()));
	}

	return env->NewObject(meta, meta_construct, column_count, name_array, type_array);
}

JNIEXPORT jobjectArray JNICALL Java_nl_cwi_da_duckdb_DuckDBNative_duckdb_1jdbc_1fetch(JNIEnv *env, jclass,
                                                                                      jobject res_ref_buf) {
	auto res_ref = (ResultHolder *)env->GetDirectBufferAddress(res_ref_buf);
	if (!res_ref) {
		jclass Exception = env->FindClass("java/sql/SQLException");
		env->ThrowNew(Exception, "Invalid result set");
	}

	res_ref->chunk = res_ref->res->Fetch();
	auto row_count = res_ref->chunk->size();

	auto vec_array = (jobjectArray)env->NewObjectArray(res_ref->chunk->column_count(),
	                                                   env->FindClass("nl/cwi/da/duckdb/DuckDBVector"), nullptr);
	for (idx_t col_idx = 0; col_idx < res_ref->chunk->column_count(); col_idx++) {
		auto &vec = res_ref->chunk->data[col_idx];
		auto type_str = env->NewStringUTF(TypeIdToString(vec.type).c_str());
		// construct nullmask
		auto null_array = env->NewBooleanArray(row_count);
		jboolean *null_array_ptr = env->GetBooleanArrayElements(null_array, nullptr);
		for (idx_t row_idx = 0; row_idx < row_count; row_idx++) {
			null_array_ptr[row_idx] = FlatVector::Nullmask(vec)[row_idx];
		}
		env->ReleaseBooleanArrayElements(null_array, null_array_ptr, 0);

		jclass vec_class = env->FindClass("nl/cwi/da/duckdb/DuckDBVector");
		jmethodID vec_construct = env->GetMethodID(vec_class, "<init>", "(Ljava/lang/String;I[Z)V");
		auto jvec = env->NewObject(vec_class, vec_construct, type_str, (int)row_count, null_array);

		jobject constlen_data = nullptr;
		jobjectArray varlen_data = nullptr;

		switch (vec.type) {
		case TypeId::BOOL:
			constlen_data = env->NewDirectByteBuffer(FlatVector::GetData(vec), row_count * sizeof(bool));
			break;
		case TypeId::INT8:
			constlen_data = env->NewDirectByteBuffer(FlatVector::GetData(vec), row_count * sizeof(int8_t));
			break;
		case TypeId::INT16:
			constlen_data = env->NewDirectByteBuffer(FlatVector::GetData(vec), row_count * sizeof(int16_t));
			break;
		case TypeId::INT32:
			constlen_data = env->NewDirectByteBuffer(FlatVector::GetData(vec), row_count * sizeof(int32_t));
			break;
		case TypeId::INT64:
			constlen_data = env->NewDirectByteBuffer(FlatVector::GetData(vec), row_count * sizeof(int64_t));
			break;
		case TypeId::FLOAT:
			constlen_data = env->NewDirectByteBuffer(FlatVector::GetData(vec), row_count * sizeof(float));
			break;
		case TypeId::DOUBLE:
			constlen_data = env->NewDirectByteBuffer(FlatVector::GetData(vec), row_count * sizeof(double));
			break;
		case TypeId::VARCHAR:
			varlen_data = env->NewObjectArray(row_count, env->FindClass("java/lang/String"), nullptr);
			for (idx_t row_idx = 0; row_idx < row_count; row_idx++) {
				env->SetObjectArrayElement(
				    varlen_data, row_idx, env->NewStringUTF(((string_t *)FlatVector::GetData(vec))[row_idx].GetData()));
			}
			break;
		default:
			jclass Exception = env->FindClass("java/sql/SQLException");
			env->ThrowNew(Exception, ("Unsupported result column type " + TypeIdToString(vec.type)).c_str());
		}

		jfieldID constlen_data_field = env->GetFieldID(vec_class, "constlen_data", "Ljava/nio/ByteBuffer;");
		jfieldID varlen_data_field = env->GetFieldID(vec_class, "varlen_data", "[Ljava/lang/Object;");

		env->SetObjectField(jvec, constlen_data_field, constlen_data);
		env->SetObjectField(jvec, varlen_data_field, varlen_data);

		env->SetObjectArrayElement(vec_array, col_idx, jvec);
	}

	return vec_array;
}
