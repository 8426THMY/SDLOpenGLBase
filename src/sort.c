#include "sort.h"


/*
** Return -1 if x is less than y, 0 if x is equal to y and 1 if x is greater than y.
** We generally use this for sorting objects whose ordering depends on a float.
*/
sort_t compareFloat(const float x, const float y){
	if(x < y){
		return(SORT_COMPARE_LESSER);
	}
	if(x > y){
		return(SORT_COMPARE_GREATER);
	}
	return(SORT_COMPARE_EQUAL);
}

// Useful if we want to sort floats in reversed order.
sort_t compareFloatReversed(const float x, const float y){
	if(x > y){
		return(SORT_COMPARE_LESSER);
	}
	if(x < y){
		return(SORT_COMPARE_GREATER);
	}
	return(SORT_COMPARE_EQUAL);
}

sort_t compareKeyValue(const keyValue *const restrict kv1, const keyValue *const restrict kv2){
	return(compareFloat(kv1->key, kv2->key));
}

sort_t compareKeyValueReversed(const keyValue *const restrict kv1, const keyValue *const restrict kv2){
	return(compareFloatReversed(kv1->key, kv2->key));
}