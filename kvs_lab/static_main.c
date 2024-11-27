#include "kvs.h"

clock_t start, end;
double cpu_time_used = 0.0;
// Baseline 버전: do_snapshot 함수 - fprintf 사용
void do_snapshot_baseline(const char *filename, kvs_t *kvs) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    node_t *current = kvs->db->forward[0];
    while (current != NULL) {
        fprintf(file, "%s %s\n", current->key, current->value);
        current = current->forward[0];
    }
    fflush(file);
    fsync(fileno(file));
    fclose(file);
}
// Baseline 버전: do_recovery 함수 - fscanf 사용
void do_recovery_baseline(const char *filename, kvs_t *kvs) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    char key[256], value[5001];
    while (fscanf(file, "%255s %5000[^\n]", key, value) == 2) {  //kvs_baseline.img에는 tweet숫자 문자열(중간에 공백)으로 저장되어 있어서 공백을 기준으로 읽었어야...
        if (put(kvs, key, value) == -1) {
            fprintf(stderr, "키-값 쌍 삽입 실패: %s, %s\n", key, value);
        }
    }
    fclose(file);
}
// Custom 버전: do_snapshot 함수 - system call 사용
void do_snapshot_custom(const char *filename, kvs_t *kvs) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return;
    }
    node_t *current = kvs->db->forward[0];
    char buffer[520];
    ssize_t written; //오류처리 하기 위해
    while (current != NULL) {
        snprintf(buffer, sizeof(buffer), "%s %s\n", current->key, current->value);
        written = write(fd, buffer, strlen(buffer));
        if(written == -1){
            perror("failed write");
            close(fd);
            return;
        }
        current = current->forward[0];
    }
    fsync(fd);
    close(fd);
}
void do_recovery_custom(const char *filename, kvs_t *kvs) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }
    char buffer[6000];
    ssize_t bytes_read;
    char key[256], value[5001];
    char *current_pos;
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';  // 널 종료 문자 추가하여 C 문자열로 만듦
        current_pos = buffer;
        while (current_pos != NULL) {
            // "tweet" 문자열 찾기
            char *tweet_pos = strstr(current_pos, "tweet");
            if (tweet_pos == NULL) {
                break;  // 더 이상 "tweet" 문자열이 없으면 반복 종료
            }
            // "tweet" 이후의 숫자 (키) 읽기
            int key_length = 0;
            while (tweet_pos[key_length] != ' ' && tweet_pos[key_length] != '\t' && tweet_pos[key_length] != '\0' && key_length < 255) {
                key[key_length] = tweet_pos[key_length];
                key_length++;
            }
            key[key_length] = '\0';  // 키 문자열 종료
            // 공백을 건너뛰고 값 읽기
            current_pos = tweet_pos + key_length;
            while (*current_pos == ' ' || *current_pos == '\t') {
                current_pos++;  // 공백 또는 탭 문자를 건너뜀
            }
            // 값 읽기
            int value_length = 0;
            while (current_pos[value_length] != '\0' && strncmp(current_pos + value_length, "tweet", 5) != 0 && value_length < 5000) {
                value[value_length] = current_pos[value_length];
                value_length++;
            }
            value[value_length] = '\0';  // 값 문자열 종료
            // 키-값 쌍을 kvs에 삽입
            if (put(kvs, key, value) == -1) {
                fprintf(stderr, "키-값 쌍 삽입 실패: %s, %s\n", key, value);
            }
            // 다음 "tweet" 문자열 탐색
            current_pos += value_length;
        }
    }
    if (bytes_read == -1) {
        perror("failed read");
    }
    close(fd);
}
void measure_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("메모리 사용량: %ld 킬로바이트\n", usage.ru_maxrss);
}
// 특정 키들에 대한 get 요청을 보내어 값을 출력하는 함수 추가
void test_get_requests(kvs_t *kvs) {
    const char *keys[] = {"tweet55", "tweet13843", "tweet3482"};
    char *value;

    for (int i = 0; i < 3; i++) {
        value = get(kvs, keys[i]);
        if (value != NULL) {
            printf("Key: %s is in KVS\n", keys[i]);
        } else {
            printf("Key: %s not found in KVS\n", keys[i]);
        }
    }
}
int main() {
    // cluster004.trc 파일에서 데이터를 읽어와 dataset에 입력
    const char *data_path = "/home/ubuntu/sp24f/cluster004.trc";

    FILE *data_file = fopen(data_path, "r");
    if (data_file == NULL) {
        perror("fopen");
        return -1;
    }
    
    kvs_t *kvs = kvs_open(NULL, 0);  // 복구 없이 초기화
    if (!kvs) {
        printf("kvs 열기 실패\n");
        fclose(data_file);
        return -1;
    }
    
    FILE *answer_file = fopen("answer.dat", "w");
    if(!answer_file){
        printf("Failed to create answer.dat\n");
        fclose(data_file);
        kvs_close(kvs);
        return -1;
    }

    //cluster004.trc파일의 데이터를 읽고 answer.dat에 저장
    char opt[5];
    char key[256], value[5001];
    while(fscanf(data_file, "%3s,%99[^,],%5000s\n", opt, key, value) == 3){
        if(strcmp(opt, "set") == 0){
            if(put(kvs, key, value)==-1){
                printf("Failed to set key: %s, value: %s\n", key, value);
            }
            else{
                fprintf(answer_file, "%s,%s\n", key, value);
            }
        }
    }
    fclose(data_file);

    printf("\n--Get 요청 테스트--\n");
    test_get_requests(kvs);


    // Baseline 버전
    printf("\n--Baseline 버전--\n");
    start = clock();
    do_snapshot_baseline("kvs_baseline.img", kvs);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Baseline 스냅샷 시간: %f 초\n", cpu_time_used);
    measure_memory_usage();

    kvs_close(kvs);

    kvs = kvs_open("kvs_baseline.img", 0); 
    start = clock();
    do_recovery_baseline("kvs_baseline.img", kvs);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Baseline 복구 시간: %f 초\n", cpu_time_used);
    measure_memory_usage();

    // 스냅샷 후 특정 키들에 대해 get 요청
    printf("\n--Get 요청 테스트--\n");
    test_get_requests(kvs);

    kvs_close(kvs);

    // Custom 버전
    // if (!kvs) {
    //     printf("kvs 열기 실패\n");
    //     return -1;
    // }

    // printf("\n--Custom 버전--\n");
    // start = clock();
    // do_snapshot_custom("kvs_custom.img", kvs);
    // end = clock();
    // cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    // printf("Custom 스냅샷 시간: %f 초\n", cpu_time_used);
    // measure_memory_usage();

    // kvs_close(kvs);

    // kvs = kvs_open("kvs_custom.img", 1);  // Custom 복구
    // start = clock();
    // do_recovery_custom("kvs_custom.img", kvs);
    // end = clock();
    // cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    // printf("Custom 복구 시간: %f 초\n", cpu_time_used);
    // measure_memory_usage();

    // // 스냅샷 후 특정 키들에 대해 get 요청
    // printf("\n--Get 요청 테스트--\n");
    // test_get_requests(kvs);

    // kvs_close(kvs);
    return 0;
}
