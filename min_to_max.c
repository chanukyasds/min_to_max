#include <postgres.h>
#include <fmgr.h>
#include <utils/array.h>
#include <catalog/pg_type.h>
#include <utils/lsyscache.h>
#include <math.h>
#include <string.h>
#include <common/int.h>
#include <utils/builtins.h>
#include <utils/typcache.h>


PG_MODULE_MAGIC;

Datum min_to_max_sfunc(PG_FUNCTION_ARGS);
Datum min_to_max_ffunc(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(min_to_max_sfunc);
PG_FUNCTION_INFO_V1(min_to_max_ffunc);

typedef union pgnum {
	  int16 i16;
	  int32 i32;
	  int64 i64;
	  float4 f4;
	  float8 f8;
	} pgnum; 
	

Datum
min_to_max_sfunc(PG_FUNCTION_ARGS)
	{
		Oid   arg_type = get_fn_expr_argtype(fcinfo->flinfo, 1);
		MemoryContext aggcontext;
		ArrayBuildState *state;
		Datum   data;

		if (arg_type == InvalidOid)
		 ereport(ERROR,
			 (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			  errmsg("Invalid Parameter Value")));

		if (!AggCheckCallContext(fcinfo, &aggcontext))
			elog(ERROR, "min_to_max_sfunc called in non-aggregate context");
		
		if (PG_ARGISNULL(0))
		 state = initArrayResult(arg_type, aggcontext, false);
		else
		 state = (ArrayBuildState *) PG_GETARG_POINTER(0);

		data = PG_ARGISNULL(1) ? (Datum) 0 : PG_GETARG_DATUM(1);

		state = accumArrayResult(state,
						  data,
						  PG_ARGISNULL(1),
						  arg_type,
						  aggcontext);
						  
		PG_RETURN_POINTER(state);
	}


Datum
min_to_max_ffunc(PG_FUNCTION_ARGS)
	{

		ArrayBuildState *state;
		int  dims[1],lbs[1],valsLength,i,len;
		ArrayType *vals;
		Oid valsType;
		int16 valsTypeWidth,retTypeWidth;
		bool valsTypeByValue,retTypeByValue,resultIsNull = true,*valsNullFlags;
		char valsTypeAlignmentCode,retTypeAlignmentCode,*res;
		Datum *valsContent,retContent[2];
		pgnum minV, maxV;
		

		Assert(AggCheckCallContext(fcinfo, NULL));

		state = PG_ARGISNULL(0) ? NULL : (ArrayBuildState *) PG_GETARG_POINTER(0);

		if (state == NULL) 
				PG_RETURN_NULL();       
			
		dims[0] = state->nelems;
		lbs[0] = 1;
		
		vals = (ArrayType *)makeMdArrayResult(state, 1, dims, lbs,
						CurrentMemoryContext,
						false);

		if (ARR_NDIM(vals) > 1) 
			ereport(ERROR,(errmsg("Not received one dimensional array ")));
		

		valsType = ARR_ELEMTYPE(vals);

		if (valsType != INT2OID &&
			valsType != INT4OID &&
			valsType != INT8OID &&
			valsType != FLOAT4OID &&
			valsType != FLOAT8OID) {
				ereport(ERROR, (errmsg("Supported Datatypes are SMALLINT, INTEGER, BIGINT, REAL, or DOUBLE PRECISION.")));
			}

		valsLength = (ARR_DIMS(vals))[0];

		get_typlenbyvalalign(valsType, &valsTypeWidth, &valsTypeByValue, &valsTypeAlignmentCode);

		deconstruct_array(vals, valsType, valsTypeWidth, valsTypeByValue, valsTypeAlignmentCode,
			&valsContent, &valsNullFlags, &valsLength);


		switch (valsType) {
		case INT2OID:
			for (i = 0; i < valsLength; i++) {
				if (valsNullFlags[i]) {
				  continue;
				} else if (resultIsNull) {
				  minV.i16 = DatumGetInt16(valsContent[i]);
				  maxV.i16 = DatumGetInt16(valsContent[i]);
				  resultIsNull = false;
				} else {
				  if (DatumGetInt16(valsContent[i]) < minV.i16) minV.i16 = DatumGetInt16(valsContent[i]);
				  if (DatumGetInt16(valsContent[i]) > maxV.i16) maxV.i16 = DatumGetInt16(valsContent[i]);
				}
			}
			retContent[0] = Int16GetDatum(minV.i16);
			retContent[1] = Int16GetDatum(maxV.i16);
			get_typlenbyvalalign(INT2OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
			break;
		case INT4OID:
			for (i = 0; i < valsLength; i++) {
				if (valsNullFlags[i]) {
				  continue;
				} else if (resultIsNull) {
				  minV.i32 = DatumGetInt32(valsContent[i]);
				  maxV.i32 = DatumGetInt32(valsContent[i]);
				  resultIsNull = false;
				} else {
				  if (DatumGetInt32(valsContent[i]) < minV.i32) minV.i32 = DatumGetInt32(valsContent[i]);
				  if (DatumGetInt32(valsContent[i]) > maxV.i32) maxV.i32 = DatumGetInt32(valsContent[i]);
				}
			}
			retContent[0] = Int32GetDatum(minV.i32);
			retContent[1] = Int32GetDatum(maxV.i32);
			get_typlenbyvalalign(INT4OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
			break;
		case INT8OID:
			for (i = 0; i < valsLength; i++) {
				if (valsNullFlags[i]) {
				  continue;
				} else if (resultIsNull) {
				  minV.i64 = DatumGetInt64(valsContent[i]);
				  maxV.i64 = DatumGetInt64(valsContent[i]);
				  resultIsNull = false;
				} else {
				  if (DatumGetInt64(valsContent[i]) < minV.i64) minV.i64 = DatumGetInt64(valsContent[i]);
				  if (DatumGetInt64(valsContent[i]) > maxV.i64) maxV.i64 = DatumGetInt64(valsContent[i]);
				}
			}
			retContent[0] = Int64GetDatum(minV.i64);
			retContent[1] = Int64GetDatum(maxV.i64);
			get_typlenbyvalalign(INT8OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
			break;
		case FLOAT4OID:
			for (i = 0; i < valsLength; i++) {
				if (valsNullFlags[i]) {
				  continue;
				} else if (resultIsNull) {
				  minV.f4 = DatumGetFloat4(valsContent[i]);
				  maxV.f4 = DatumGetFloat4(valsContent[i]);
				  resultIsNull = false;
				} else {
				  if (DatumGetFloat4(valsContent[i]) < minV.f4) minV.f4 = DatumGetFloat4(valsContent[i]);
				  if (DatumGetFloat4(valsContent[i]) > maxV.f4) maxV.f4 = DatumGetFloat4(valsContent[i]);
				}
			}
			retContent[0] = Float4GetDatum(minV.f4);
			retContent[1] = Float4GetDatum(maxV.f4);
			get_typlenbyvalalign(FLOAT4OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
			break;
		case FLOAT8OID:
			for (i = 0; i < valsLength; i++) {
				if (valsNullFlags[i]) {
				  continue;
				} else if (resultIsNull) {
				  minV.f8 = DatumGetFloat8(valsContent[i]);
				  maxV.f8 = DatumGetFloat8(valsContent[i]);
				  resultIsNull = false;
				} else {
				  if (DatumGetFloat8(valsContent[i]) < minV.f8) minV.f8 = DatumGetFloat8(valsContent[i]);
				  if (DatumGetFloat8(valsContent[i]) > maxV.f8) maxV.f8 = DatumGetFloat8(valsContent[i]);
				}
			}
			retContent[0] = Float8GetDatum(minV.f8);
			retContent[1] = Float8GetDatum(maxV.f8);
			get_typlenbyvalalign(FLOAT8OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
			break;
		default:
			ereport(ERROR, (errmsg("Supported Datatypes are SMALLINT, INTEGER, BIGINT, REAL, or DOUBLE PRECISION.")));
		}

		
		len =((retContent[0]==0)?1:log10(retContent[0])+1)+((retContent[1]==0)?1:log10(retContent[1])+1)+2;

		res = malloc(len);

		sprintf(res, "%ld%s%ld", retContent[0],"->",retContent[1]);

		PG_RETURN_TEXT_P(cstring_to_text(res));

		free(res);

		return 0;
	
	}
