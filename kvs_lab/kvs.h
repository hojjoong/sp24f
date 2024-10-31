#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

#define MAX_LEVEL 10

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


kvs_t* open();
int close(kvs_t* kvs); // free all memory space 
int put(kvs_t* kvs, const char* key, const char* value); // return -1 if failed.
char* get(kvs_t* kvs, const char* key); // return NULL if not found. 
node_t* create_node(int level, const char *key, const char *value);
int random_level();
