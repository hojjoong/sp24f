#include "kvs.h"

int main()
{
	kvs_t* kvs = open();

	if(!kvs) {
		printf("Failed to open kvs\n");
		return -1;
	}

	// workload execution  
	
	// 1) 	file read 
	// 2) 	if put, insert (key, value) into kvs.
	// 		if get, seach the key in kvs and return the value. 
	//		Return -1 if the key is not found  
	FILE *queryFile = fopen("query.dat", "r");  //읽기 모드로 열기
	FILE *answerFile = fopen("answer.dat", "w");//결과를 저장할 파일
	if(!queryFile || !answerFile){
		printf("Failed to open query or answer file\n");
		close(kvs); //메모리 해제
		return -1;
	}
	char line[256];
	while(fgets(line, sizeof(line), queryFile){     //한 줄씩 읽어서처리
		char operation[10], key[100], value[100];
		int result;
	//sscanf가 line에서 세 개 이상의 값을 제대로 못읽으면 오류 출력
		if(sscanf(line, "%[^,],%[^,],%s", operation, key, value)<3{
			printf("Error reading line:%s", line);
			continue;
		}

	

		if(strcmp(operation, "put")==0{
			result = put(kvs, key, value);
			if(result == -1){
				fprintf(answerFile, "put failed for key %s\n", key);
			}
		}
		else if (strcmp(operation, "get") == 0{
			char *retrieved_value = get(kvs, key);
			if(retrieved_value){
				fprintf(answerFile, "get,%s,%s\n", key, retrieved_value);
				free(retrieved_value);
			}
			else{
				fprintf(answerFile, "get,%s,-1\n", key); //-1 if not found
			}

		}
	}
	fclose(queryFile);
	fclose(answerFile);
	close(kvs);
	
	return 0;
}
