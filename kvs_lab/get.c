#include "kvs.h"

char* get(kvs_t* kvs, const char* key)
{
	/* do program here */
    node_t *current = kvs->db;

    for (int i = kvs->level; i >= 0; i--) {
        while (current->forward[i] != NULL && strcmp(current->forward[i]->key, key) < 0) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];

    if (current != NULL && strcmp(current->key, key) == 0) {
        char *value = (char *)malloc(strlen(current->value) + 1);
        strcpy(value, current->value);
        return value;
    }
    return NULL;

}
