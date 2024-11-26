#include "kvs.h"

int kvs_close(kvs_t* kvs)
{
	/* do program */
	node_t* current = kvs->db->forward[0];
    	while (current != NULL) {
        	node_t *temp = current;
        	current = current->forward[0];
        	free(temp->value);
        	free(temp->forward);
        	free(temp);
	}
	//skip list 헤더 노드와 kvs 해제
	free(kvs->db->forward);
	free(kvs->db);
	free(kvs);
	
	return 0;
}
