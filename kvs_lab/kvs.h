#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<sys/resource.h>
#include<stdbool.h>

#define MAX_LEVEL 10

extern clock_t start, end;
extern double cpu_time_used;

struct node {
	char key[100];   
	char* value;
	struct node** forward;  //다음 노드들
};	
typedef struct node node_t;


struct kvs{
	struct node* db; // database   //노드들의 시작지점?
	int level;
	int items; // number of data 
};
typedef struct kvs kvs_t; //struct kvs를 kvs_t로 정의하겠다


kvs_t* kvs_open(const char *filenamem, int recovery_type);
int kvs_close(kvs_t* kvs); // free all memory space 
int put(kvs_t* kvs, const char* key, const char* value); // return -1 if failed.
char* get(kvs_t* kvs, const char* key); // return NULL if not found. 
node_t* create_node(int level, const char *key, const char *value);
int random_level();

void do_snapshot(kvs_t* kvs);
void do_recovery_baseline(const char *filename, kvs_t *kvs);
void do_recovery_custom(const char *filename, kvs_t *kvs);

void test_get_requests(kvs_t *kvs);