/*
  2023.06.01. 프로젝트 시작 : 스켈레톤 코드 및 전체 흐름 이해
  2023.06.02. 프로젝트 readme 문서 작성
  2023.06.05. skeleton code 참고하여 cnuled 작성 : Page구조체와 전역 변수 구현 
  2023.06.08. appendLine, insertLine, removeLine 구현 (시작)
  2023.06.09. insertLine, removeLine 구현 (완)
  2023.06.10. 프롬프트 입력 구현 (완)
  2023.06.11. 우분투 내 파일 권한 조정
  2023.06.12. Page 구조체 최적화 : append할때 한 페이지 당 5개 씩 넣기
  2023.06.13. 예외 처리 점검
  2023.06.18. 보고서 작성 (시작)
  2023.06.19. 테스트케이스 테스트 실행 (완)
  2023.06.20. 보고서 작성 (완)
*/

//컴퓨터융합학부 202002546 임우진
//컴퓨터프로그래밍3 00분반  

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 10 //10줄
#define MAX_CHARS 80 //최대 80자의 문자열
#define MAX_READ_LINES 5 //읽을 때 5줄씩

int total = 0; // 전체 존재하는 문자열 개수

typedef struct Page{ //페이지 구조체 : 문자열 10개, 페이지에 저장되어있는 문자열 수, 전후 참조
    char line[MAX_LINES][MAX_CHARS];
    int count;
    struct Page *nextPage;
    struct Page *prevPage;
}Page;

Page *firstPage = NULL; //첫페이지 포인터
Page *currentPage = NULL; //현재페이지 포인터

void appendLine(char *str){
    if(currentPage == NULL){ //아무것도 없으면 새페이지 생성해서 첫페이지로 지정
        currentPage = (Page *)malloc(sizeof(Page));
        currentPage->count = 0;
        currentPage->nextPage = NULL;
        currentPage->prevPage = NULL;
        firstPage = currentPage;
    }

    strcpy(currentPage->line[currentPage->count], str); //현재페이지 마지막줄에 추가
    currentPage->count++; //페이지의 카운트 1 증가

    if(currentPage->count == MAX_READ_LINES){ //페이지에 문자열이 5개가 되면
        Page *newPage = (Page *)malloc(sizeof(Page)); //새페이지 만들기
        if(newPage == NULL){ //동적할당 실패시 예외처리
            printf("Error: Failed to allocate memory for a new page.\n");
            return;
        }
        newPage->count = 0;
        newPage->nextPage = NULL;
        newPage->prevPage = currentPage;
        currentPage->nextPage = newPage;
        currentPage = newPage;
    }
    total++; //전체 문자열 개수 증가
}

void insertLine(int i, char *str) { //인자는 인덱스와 문자열
    if(i > total){ //범위를 벗어남
        printf("Error: Index out of range.\n");
        return;
    }

    Page *page = firstPage; //첫페이지부터 순회
    

    while(page != NULL && i >= page->count){ //인덱스 세는 반복문 : i - (그 페이지의 카운트)
        i -= page->count;
        
        page = page->nextPage; //페이지 넘기기
    }

    if(page == NULL){
        printf("Error: Index out of range.\n");
        return;
    }

    int lineIdx = i; //위의 반복문을 마치고 나온 결과물 줄 위치

    if(page->count == MAX_LINES){ //꽉 찼으면 새 페이지 생성하고 연결하고 
        Page *newPage = (Page *)malloc(sizeof(Page));
        if(newPage == NULL){
            printf("Error: Failed to allocate memory for a new page.\n");
            return;
        }
        newPage->count = 0;
        newPage->prevPage = page; //연결 알고리즘 : (원)page1->page2 => page1->newpage->page2 
        newPage->nextPage = page->nextPage;
        page->nextPage = newPage;

        if(newPage->nextPage != NULL){
            newPage->nextPage->prevPage = newPage;
        }

        if(lineIdx == 0){ //lineIdx가 0이면 페이지 넘기기
            page = newPage;
        }else{ 
            strcpy(newPage->line[newPage->count++], page->line[MAX_LINES - 1]);
            page->count--;
        }
    }

    for(int j = page->count; j > lineIdx; j--){ //한칸씩 뒤로 밀기
        strcpy(page->line[j], page->line[j - 1]);
    }

    strcpy(page->line[lineIdx], str); //밀고 남은 빈자리(삽입하고픈 위치)에 삽입
    page->count++;
    total++;
}


void removeLine(int i) { //index를 받아서 삭제하는 연산
    i--;
    if(i >= total){ //범위 벗어남
        printf("Error: Index out of range.\n");
        return;
    }

    Page *page = firstPage;
    
    //페이지 넘기기
    while(page != NULL && i >= page->count){
        i -= page->count;
        
        page = page->nextPage;
    }

    if(page == NULL){
        printf("Error: Index out of range.\n");
        return;
    }

    // 페이지 내의 요소들만 이동시킵니다 - 다음 페이지의 요소는 건드리지 않고!
    for(int j = i; j < page->count - 1; j++){
        strcpy(page->line[j], page->line[j + 1]);
    }

    page->count--;
    total--;

    // 페이지가 비어있다면 메모리를 해제하고 연결을 조정합니다.
    if(page->count == 0){
        if(page->prevPage){
            page->prevPage->nextPage = page->nextPage;
        }else{
            firstPage = page->nextPage;
        }

        if(page->nextPage){
            page->nextPage->prevPage = page->prevPage;
        }
        
        free(page);
    }
}



void printPages(){ //페이지 출력
    Page *page = firstPage;
    int pageNum = 1;

    while(page != NULL){
        printf("<Page %d>\n", pageNum);
        for(int i = 0; i < page->count; i++){
            printf("%s\n", page->line[i]);
        }
        printf("---------------------\n");
        page = page->nextPage;
        pageNum++;
    }
}

int main(int argc, char *argv[]){
    if(argc == 3 && strcmp(argv[1], "-f") == 0){
        // 파일 열기
        FILE *file = fopen(argv[2], "r");
        if(file == NULL){
            printf("Error: Failed to open the file.\n");
            return 1;
        }

        char line[MAX_CHARS];
        while(fgets(line, sizeof(line), file)){
            // 파일의 문자열 삽입
            line[strcspn(line, "\n")] = '\0';
            appendLine(line);
        }

        fclose(file);
    }

    // exit 명령어가 나올 때까지 명령어를 계속 입력받고 처리
    char command[10];
    while(strcmp(command, "exit") != 0){
        
        scanf("%s", command);

        if(strcmp(command, "-a") == 0){
            char str[MAX_CHARS];
            scanf(" %[^\n]s", str);
            appendLine(str);
        }else if (strcmp(command, "-i") == 0){
            int index;
            char str[MAX_CHARS];
            scanf("%d", &index);
            scanf(" %[^\n]s", str);
            insertLine(index-1, str);
        }else if(strcmp(command, "-r") == 0){
            int index;
            scanf("%d", &index);
            removeLine(index);
        }else if(strcmp(command, "exit") != 0){
            printf("Invalid command.\n");
        }

        // 현재 상태 출력
        printPages();
    }
    // 프로그램 종료 시 페이지 내용을 testfile.txt 파일로 출력
    FILE *outputFile = fopen("testfile.txt", "w"); //쓰기 옵션
    if(outputFile == NULL){
        printf("Error: Failed to open the output file.\n");
        return 1;
    }

    Page *page = firstPage; //첫 페이지부터
    while(page != NULL){
        for(int i = 0; i < page->count; i++){
            fprintf(outputFile, "%s\n", page->line[i]); //outputfile에 문자열개행 형식으로 페이지의 내용을 삽입
        }
        page = page->nextPage;
    }

    fflush(outputFile);
    fclose(outputFile);

    return 0;
}
