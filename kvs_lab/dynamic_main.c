#include "kvs.h"
#include <dlfcn.h>

int main()
{
	void *handle;
	kvs_t* (*open)();
	int (*put)(kvs_t*, const char*, const char*);
	char* (*get)(kvs_t*, const char*);
	int (*close)(kvs_t*);

	handle = dlopen("libkvs.so", RTLD_LAZY);
	if(!handle){
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	//로드한 open함수의 포인터 선언
	open = dlsym(handle, "open");
	if ((dlerror()) != NULL) {
		fprintf(stderr, "%s\n", dlerror());
		dlclose(handle);
		return -1;
	}

	//로드한 set함수의 포인터 선언
	put = dlsym(handle, "put");
	if((dlerror())!=NULL){
		fprintf(stderr, "%s\n", dlerror());
		dlclose(handle);
		return -1;
	}

	//로드한 get함수의 포인터 선언
	get = dlsym(handle, "get");
	if((dlerror()) != NULL){
		fprintf(stderr, "%s\n", dlerror());
		dlclose(handle);
		return -1;
	}

	//로드한 close함수의 포인터 선언
	close = dlsym(handle, "close");                                                
	if((dlerror())!=NULL){
		fprintf(stderr, "%s\n", dlerror());
		dlclose(handle);
		return -1;
	}

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
    	//%24[,]:,를 만나기 전까지 최대 24자를 읽어 key에 저장
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
                		fprintf(answerFile, "%s\n", get_value); //결과파일에 기록
                		free(get_value);
            		}
            		else{  //값을 찾을 수 없는 경우
                		printf("Value not found\n");
                		fprintf(answerFile, "-1\n");
            		}
        	}
    	}

    	fclose(queryFile);
    	fclose(answerFile);
    	close(kvs);


	if(dlclose(handle) < 0){
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

    	return 0;
}















