#include "kvs.h"

kvs_t* open()
{
	kvs_t* kvs = (kvs_t*) malloc (sizeof(kvs_t));
	if(kvs){
		kvs->items = 0;
		kvs->level = 0;
		kvs->db = create_node(MAX_LEVEL, "", "");
		for(int i=0; i<=MAX_LEVEL; i++){
			kvs->db->forward[i] = NULL;
		}
	}	
	printf("Open: kvs has %d items\n", kvs->items);

	return kvs;
}
