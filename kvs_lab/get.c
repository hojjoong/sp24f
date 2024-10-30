#include "kvs.h"

char* get(kvs_t* kvs, const char* key)
{
	/* do program here */
	node_t* current = kvs->db; //kvs의 첫 번째 노드부터 시작

	for (int i=kvs->level; i>=0; i--){
		while(current->next[i]!=NULL && strcmp(current->next[i]->key, key) <0){
			current = current->next[i];
		}
	}
	
	//마지막 레벨에서 key 확인
	current = current->next[0];
	if(current != NULL && strcmp(current->key, key) == 0){
		return strdup(current->value); //key 맞으면 해당 value 반환
	}
	
	//key가 없을 경우 기본 값 할당 및 반환
	char* value = (char*)malloc(sizeof(char)*100);

	if(!value){
		printf("Failed to malloc\n");
		return NULL;
	}

	strcpy(value, "deadbeaf");
	return value;

}
