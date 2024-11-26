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
    while (fscanf(file, "%255[^,],%5000[^\n]", key, value) == 2) {
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

// Custom 버전: do_recovery 함수 - system call 사용
void do_recovery_custom(const char *filename, kvs_t *kvs) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    //복구 sementation fault... buffer 늘리기? buffer에서 내용이 잘리나?-> \n으로 구분?  
    //strtok()을 사용하여 줄을 나누지만, 마지막 줄이 완전하지 않을 경우 
    //leftover에 저장하여 다음에 읽을 때 처리하도록 수정
    char buffer[6000];
    ssize_t bytes_read;
    char key[256], value[5001];
    char leftover[6000] = ""; // 이전에 읽고 남은 데이터 저장용

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        // 이전에 읽고 남은 데이터와 현재 읽은 데이터를 합쳐서 처리
        char combined[12000];
        snprintf(combined, sizeof(combined), "%s%s", leftover, buffer);

        // 줄 단위("\n")로 나누어 처리하기 위해 strtok 사용
        char *line = strtok(combined, "\n");  
        while (line != NULL) {
            //마지막 줄에 '\n'이 없는 경우, 이 줄을 leftover에 저장
            char *next_line = strtok(NULL, "\n");
            if (next_line == NULL) {
                // 완전하지 않은 줄은 leftover에 저장
                snprintf(leftover, sizeof(leftover), "%s", line);
            } else {
                // 완전한 줄은 처리
                if (sscanf(line, "%255[^ ] %5000[^\n]", key, value) == 2) {
                    if (put(kvs, key, value) == -1) {
                        fprintf(stderr, "key-value 삽입 실패: %s, %s\n", key, value);
                    }
                }
            }
            line = next_line;
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

    // Baseline 버전
    // printf("\n--Baseline 버전--\n");
    // start = clock();
    // do_snapshot_baseline("kvs_baseline.img", kvs);
    // end = clock();
    // cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    // printf("Baseline 스냅샷 시간: %f 초\n", cpu_time_used);
    // measure_memory_usage();

    // kvs_close(kvs);

    // kvs = kvs_open("kvs_baseline.img", 0); 
    // start = clock();
    // do_recovery_baseline("kvs_baseline.img", kvs);
    // end = clock();
    // cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    // printf("Baseline 복구 시간: %f 초\n", cpu_time_used);
    // measure_memory_usage();

    // kvs_close(kvs);

    // Custom 버전
    if (!kvs) {
        printf("kvs 열기 실패\n");
        return -1;
    }

    printf("\n--Custom 버전--\n");
    start = clock();
    do_snapshot_custom("kvs_custom.img", kvs);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Custom 스냅샷 시간: %f 초\n", cpu_time_used);
    measure_memory_usage();

    kvs_close(kvs);

    kvs = kvs_open("kvs_custom.img", 1);  // Custom 복구
    start = clock();
    do_recovery_custom("kvs_custom.img", kvs);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Custom 복구 시간: %f 초\n", cpu_time_used);
    measure_memory_usage();

    kvs_close(kvs);
    return 0;
}
