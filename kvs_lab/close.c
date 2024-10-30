#include "kvs.h"

int close(kvs_t* kvs)
{
	/* do program */
	if(!kvs) return -1;

	node_t* current = kvs->db->next[0];
	while(current!=NULL){
		node_t* next_node = current -> next[0]; //다음 노드 저장
		free(current->value); //값 메모리 해제
		free(current->next);  //다음 노드 포인터 배열 해제
		free(current);	      //노드 자체 해제
		current = next_node;  //다음 노드로 이동
	}
	
	//skip list 헤더 노드와 kvs 해제
	free(kvs->db->next);
	free(kvs->db);
	free(kvs);
	
	return 0;
}
