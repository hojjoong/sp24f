#include "kvs.h"

kvs_t* kvs_open(const char *filename, int recovery_type) {
    kvs_t* kvs = (kvs_t*) malloc(sizeof(kvs_t));
    if (kvs) {
        kvs->items = 0;
        kvs->level = 0;
        kvs->db = create_node(MAX_LEVEL, "", "");
        //오류 확인
        if (kvs->db == NULL) {
            fprintf(stderr, "노드 생성 실패\n");
            free(kvs);
            return NULL;
        }
        for (int i = 0; i <= MAX_LEVEL; i++) {
            kvs->db->forward[i] = NULL;
        }
        if (filename != NULL) {
            if (recovery_type == 0) {
                printf("Baseline 복구 시작\n");
                do_recovery_baseline(filename, kvs);
            } else if (recovery_type == 1) {
                printf("Custom 복구 시작\n");
                do_recovery_custom(filename, kvs);
            } else {
                printf("잘못된 복구 유형입니다.\n");
            }
        }
    }
    return kvs;
}
