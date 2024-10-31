#include "kvs.h"

//새 노드 생성
node_t *create_node(int level, const char *key, const char *value) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->forward = (node_t **)malloc(sizeof(node_t *) * (level + 1));
    strcpy(new_node->key, key);
    new_node->value = (char *)malloc(strlen(value) + 1);
    strcpy(new_node->value, value);
    return new_node;
}

int random_level() {
    int level = 0;
    while (rand() < RAND_MAX / 2 && level < MAX_LEVEL) {
        level++;
    }
    return level;
}

int put(kvs_t *kvs, const char *key, const char *value) {
    node_t *current = kvs->db;
    node_t *update[MAX_LEVEL + 1];
    memset(update, 0, sizeof(node_t *) * (MAX_LEVEL + 1));

    for (int i = kvs->level; i >= 0; i--) {
        while (current->forward[i] != NULL && strcmp(current->forward[i]->key, key) < 0) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    current = current->forward[0];

    if (current != NULL && strcmp(current->key, key) == 0) {
        free(current->value);
        current->value = (char *)malloc(strlen(value) + 1);
        strcpy(current->value, value);
        return 0;
    } else {
        int new_level = random_level();
        if (new_level > kvs->level) {
            for (int i = kvs->level + 1; i <= new_level; i++) {
                update[i] = kvs->db;
            }
            kvs->level = new_level;
        }

        node_t *new_node = create_node(new_level, key, value);
        for (int i = 0; i <= new_level; i++) {
            new_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = new_node;
        }
        kvs->items++;
        return 0;
    }
}
