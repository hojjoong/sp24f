#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<limits.h>

//#define MAX_NUM 10
#define MAX_NUM 1000000000

typedef double data_t;

struct vec {
	int len;
	data_t *data;
};
typedef struct vec vec_t;

int get_vec_element(vec_t* v, int idx, data_t *val)
{
	assert(v);

	if(idx >= v->len)
		return 0;

	*val = v->data[idx];
	return 1;
}

int vec_length(vec_t* v)
{
	assert(v);
	return v->len;
}

void combine(vec_t* v, data_t *dest)
{
	assert(dest);
	*dest = 0;
	for(int i = 0; i < vec_length(v); i++){
		data_t val; 
		get_vec_element(v, i, &val);
		*dest = *dest + val; 
	}
}

void init(vec_t* v)
{
	assert(v);
	v->len = MAX_NUM;
	v->data = (data_t*) malloc(sizeof(data_t) * MAX_NUM);

	// init 
	for(int i = 0; i < MAX_NUM; i++)
		v->data[i] = i;
}

int main()
{
	printf("This is the naive version .. \n");

	vec_t info; 
	data_t result; 
	// init 
	init(&info);

	// combine 
	combine(&info, &result);
	//printf("combined val = %d\n", result);

	return 0;
}
