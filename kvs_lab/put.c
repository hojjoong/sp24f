#include "kvs.h"

//새 노드 생성
node_t* create_node(int level, const char* key, const char* value) {
    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    strcpy(new_node->key, key);   //문자열 복사 내장함수
    new_node->value = strdup(value);
    new_node->next = (node_t**)malloc(sizeof(node_t*) * (level + 1));
    for (int i = 0; i <= level; i++) {
        new_node->next[i] = NULL;
    }
    return new_node;
}

// 랜덤 레벨 생성 (Skip List의 높이 결정)
int random_level() {
    int level = 0;
    while (rand() % 2 && level < MAX_LEVEL) {
        level++;
    }
    return level;
}
int put(kvs_t* kvs, const char* key, const char* value){
	printf("put: %s, %s\n", key, value);
	/* do program here */
	node_t* update[MAX_LEVEL + 1];
   	node_t* current = kvs->db;

    	// 각 레벨에서 삽입 위치 찾기
    	for (int i = kvs->level; i >= 0; i--) {
        	while (current->next[i] != NULL && strcmp(current->next[i]->key, key) < 0) {
            	current = current->next[i];
        	}
        	update[i] = current;
    	}

    	// key가 존재하면 value 업데이트
    	current = current->next[0];
    	if (current != NULL && strcmp(current->key, key) == 0) { //current의 key가 찾는 key와 동일한지 검사
        	free(current->value);           //이전 할당된value메모리 해제하고 strdup으로 새로운 값 업데이
        	current->value = strdup(value);
        	return 0;
    	}

    	// 새로운 노드를 삽입할 레벨 설정
    	int new_level = random_level();
    	if (new_level > kvs->level) {
        	for (int i = kvs->level + 1; i <= new_level; i++) {
            	update[i] = kvs->db;
        	}
        	kvs->level = new_level;
    	}

    	// 새 노드 생성 및 연결
    	node_t* new_node = create_node(new_level, key, value);
    	for (int i = 0; i <= new_level; i++) {
        	new_node->next[i] = update[i]->next[i];
        	update[i]->next[i] = new_node;
    	}

    	kvs->items++;  // 데이터 수 증가
	return 0;
