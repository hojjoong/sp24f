#include "kvs.h"

int main() {
    kvs_t* kvs = open();
    if (!kvs) {
        printf("Failed to open kvs\n");
        return -1;
    }

    FILE *queryFile = fopen("query.dat", "r");
    FILE *answerFile = fopen("answer.dat", "w");
    if (!queryFile || !answerFile) {
        printf("Failed to open query or answer file\n");
        close(kvs);
        return -1;
    }

    char opt[5];
    char key[24];
    char value[24];

    while(fscanf(queryFile, "%3s, %24[^,],%24s\n", opt, key, value)==3){
        if (strcmp(opt, "set") == 0){
            if(put(kvs, key, value)==-1){
                printf("Failed to set key: %s, value: %s\n", key, value);
            }
        }
        else if(strcmp(opt, "get")==0){
            char* get_value = get(kvs, key);
            if(get_value){
                printf("%s\n", get_value);
                fprintf(answerFile, "%s\n", get_value);
                free(get_value);
            }
            else{
                printf("-1\n");
                fprintf(answerFile, "-1\n");
            }
        }
    }

    fclose(queryFile);
    fclose(answerFile);
    close(kvs);
    return 0;
}
