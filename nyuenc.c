#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#define TASK_NUM 999999999

int cur_task_index;//to track which file we are fetching. 

struct queue_task{
	char filename[1000];
	int fileindex;
}mytask[1000];

struct result{
	char head;
	int headcount;
	char body[TASK_NUM];
	char tail;
	int tailcount;
};

int thread_num;
int taskcount=0;
int comm_len;
int flag=0; 
int completed=0;
struct result* myresult[1000];

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

//the rle_encode function accepts the current character and count as arguments, allowing the program to maintain the count across multiple files.
void rle_encode(FILE *input_file, int *current_char, uint8_t *count) {
    int next_char;

    while ((next_char = fgetc(input_file)) != EOF) {
        if (*current_char == next_char && *count < 255) {
            //检查新读取的字符next_char是否与当前字符current_char相同，且计数器值小于255。
            (*count)++;
        } else {
            if (*current_char != -1) {
                putchar(*current_char);
                putchar(*count);
            }
            *current_char = next_char;
            *count = 1;
        }
    }
}


void execute(struct queue_task task) {
    // Open file
    int fd = open(task.filename, O_RDONLY);
    if (fd == -1) {
    fprintf(stderr, "Error opening file '%s': %s\n", task.filename, strerror(errno));
    exit(EXIT_FAILURE);
    }

    // Get file size
    struct stat sb;
    fstat(fd, &sb);

    // Map file into memory
    char *str = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    int len = sb.st_size; 
    // printf("%d",sb.st_size);  
    if (len == 0) {
        // *start = '\0';
        // *middle = '\0';
        // *end = '\0';
        //printf("2222\n");
        exit(1);
        return;
    }
    if (len == 1) {
        myresult[task.fileindex]->head=*str;
        myresult[task.fileindex]->headcount=1;

    // printf("this is result number %d\n",task.fileindex);
    // printf("%c\n",myresult[task.fileindex]->head);
    // printf("%d\n",myresult[task.fileindex]->headcount);
    // printf("%s\n",myresult[task.fileindex]->body);
    // printf("%c\n",myresult[task.fileindex]->tail);
    // printf("%d\n\n",myresult[task.fileindex]->tailcount);
        return;
    }
    int i = 0, j, k;
    char last = *str;
    int freq=1;
    int count = 1;
    int trace=0;
    while (i < len - 1) {
        
        j = i + 1;
        while (j < len && str[j] == last) {
            count++;
            j++;
        }
        if (myresult[task.fileindex]->headcount==0) {//"aaa"or"aaabb"
            myresult[task.fileindex]->head=last;
            myresult[task.fileindex]->headcount=count;
        } else if ( myresult[task.fileindex]->tailcount==0&& j == len) {//"aaabb"
            //sprintf(end, "%c%d", last, count);
            myresult[task.fileindex]->tail=last;
            myresult[task.fileindex]->tailcount=count;
        } else {
            myresult[task.fileindex]->body[trace]=last;
            trace++;
            myresult[task.fileindex]->body[trace]=(char)count;
            trace++;
        }
        if(j==len){
            break;
            
        }
        last = str[j];
        freq++;
        count = 1;
        i = j;
    }
    //give terminator to body string
    //myresult[task.fileindex]->body[trace]='\0';

    if (myresult[task.fileindex]->tailcount == 0 && freq!=1) {
        // sprintf(end, "%c%d", last, count);
        myresult[task.fileindex]->tail=last;
        myresult[task.fileindex]->tailcount=count;
    }
    
    // printf("freq= %d\n",freq);
    // printf("this is result number %d\n",task.fileindex);
    // printf("head = %c\n",myresult[task.fileindex]->head);
    // printf("%d\n",myresult[task.fileindex]->headcount);
    // printf("%s\n",myresult[task.fileindex]->body);
    // printf("tail = %c\n",myresult[task.fileindex]->tail);
    // printf("%d\n\n",myresult[task.fileindex]->tailcount);
}








void mergeresult(){
    // int tempcount=0;
    //  for (int i=0; i<1000;i++){
    //     if(myresult[i]->headcount!=0){
    //         // printf("%c\n", myresult[i]->head);
    //         // printf("%d\n", myresult[i]->headcount);
    //         // printf("%c\n", myresult[i]->tail);
    //         // printf("%d\n", myresult[i]->tailcount);
    //         tempcount++;
    //     }
    // }
    // printf("%d\n", taskcount);
    // for (int i=0; i<taskcount;i++){
    //         if(myresult[i]->headcount==0){
    //             for(int j=i; j<taskcount;j++){
    //                 myresult[j]=myresult[j+1];
    //             }   
    //         }
    //     }
    // for (int i=0; i<1000;i++){
    //     if(myresult[i]->headcount!=0){
    //         // printf("%c\n", myresult[i]->head);
    //         // printf("%d\n", myresult[i]->headcount);
    //         // printf("%c\n", myresult[i]->tail);
    //         // printf("%d\n", myresult[i]->tailcount);
    //         tempcount++;
    //     }
    // }
    int i=0; 
    int count;
    char cmp_char;

    if(myresult[0]->tailcount!=0){
        printf("%c",myresult[0]->head);
        printf("%c",(char)myresult[0]->headcount);
        printf("%s",myresult[0]->body);
        count=myresult[0]->tailcount;
        cmp_char=myresult[0]->tail;
    }else{
        count=myresult[0]->headcount;
        cmp_char=myresult[0]->head;
    }
    if(taskcount==1){
        printf("%c",cmp_char);
        printf("%c",(char)count);       
        return;
    }


    while(i+1<taskcount){
        while(i+1<taskcount && myresult[i]->tailcount==0 && myresult[i+1]->tailcount==0 && myresult[i]->head==myresult[i+1]->head){
            count+=myresult[i+1]->headcount;
            i++;
        }

        if(myresult[i]->tailcount==0 && myresult[i+1]->tailcount==0){
            printf("%c",cmp_char);
            printf("%c",(char)count);   
            cmp_char=myresult[i+1]->head;
            count=myresult[i+1]->headcount;  
        }

        if(myresult[i]->tailcount!=0 && myresult[i+1]->tailcount==0){
            if(cmp_char==myresult[i+1]->head){
                count+=myresult[i+1]->headcount;
            }else{
                printf("%c",cmp_char);
                printf("%c",(char)count); 
                cmp_char=myresult[i+1]->head;
                count=myresult[i+1]->headcount;   
            }
        }

        if(myresult[i+1]->tailcount!=0){   //if(myresult[i]->tailcount==0 && myresult[i+1]->tailcount!=0) + if(myresult[i]->tailcount!=0 && myresult[i+1]->tailcount!=0) 
            printf("%c",cmp_char);
            if(cmp_char==myresult[i+1]->head){ //cmp_char = myresult[i]->head in this case :) 
                printf("%c",(char)(count+myresult[i+1]->headcount));
                printf("%s",myresult[i+1]->body); 
            }else{
                printf("%c",(char)count); 
                printf("%c",myresult[i+1]->head); 
                printf("%c",(char)myresult[i+1]->headcount); 
                printf("%s",myresult[i+1]->body); 
            }
            cmp_char=myresult[i+1]->tail;
            count=myresult[i+1]->tailcount;
        }

        if(i+1==taskcount-1){
            printf("%c",cmp_char);
            printf("%c",(char)count);  
        }

        i++;
    }
}

//startThread函数是工作线程的入口。每个工作线程都会循环等待条件变量信号，然后从任务队列中取出一个任务并执行。
void* startThread(void* args) {
    while (1) {
        //struct queue_task task;
        pthread_mutex_lock(&mutexQueue);
        //when queue is empty we just wait
        //or when all the tasks are being processed completely we return 
        while(taskcount==cur_task_index){
            if((taskcount==comm_len-3)&&(flag==1)){
                pthread_mutex_unlock(&mutexQueue);
                return NULL;
                
            }

            pthread_cond_wait(&condQueue, &mutexQueue);
        }
        struct queue_task task = mytask[cur_task_index];
        //printf("%s\n%d\n",mytask[cur_task_index].filename,mytask[cur_task_index].fileindex);
        //update cur_task_index to keep track of the current processing thread.
        int i;
        cur_task_index++;
        if(comm_len-3==cur_task_index){
            pthread_cond_broadcast(&condQueue);
        }
    
        pthread_mutex_unlock(&mutexQueue);
        execute(task);
        pthread_mutex_lock(&mutexQueue);
        completed++;
        pthread_mutex_unlock(&mutexQueue);

    }

}




int main(int argc, char* argv[]) {
    for(int i=0;i<1000;i++){
        myresult[i]=(struct result*) malloc(sizeof(struct result));
        myresult[i]->headcount=0;
        myresult[i]->tailcount=0;
        myresult[i]->tail='\0';

    }
    comm_len=argc;
    if (argc < 2) {//没有文件输入
        fprintf(stderr, "Usage: %s file1 [file2 ...]\n", argv[0]);
        return 1;
    }
    //初始化互斥锁和条件变量
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    int i;
    FILE *input_file;
    int current_char = -1;
    uint8_t count = 0;
    if(strcmp(argv[1],"-j")==0){//是并行
        thread_num=atoi(argv[2]);
        pthread_t th[thread_num];
        for (i = 0; i < thread_num; i++) {
            pthread_create(&th[i], NULL, &startThread, NULL) != 0;
            }


        //submit tasks to queue
        //printf("linma = %d\n",argc);
        for(i=3;i<argc;i++){
            int r=i-3;
            //printf("%s",argv[i]);
            pthread_mutex_lock(&mutexQueue);
            taskcount+=1;
            mytask[r].fileindex = r;
            strcpy(mytask[r].filename, argv[i]);
            pthread_cond_signal(&condQueue);
            pthread_mutex_unlock(&mutexQueue);
        }
        while(1){
            if((taskcount==completed)&&(completed==argc-3)){
                break;
            }
        }
        // for (i = 0; i < thread_num; i++) {
        // //等待第i个工作线程完成并退出
        // pthread_join(th[i], NULL);
        // //确保所有工作线程都已完成它们的任务并退出，然后主线程才会继续执行。这样可以确保主线程在销毁互斥锁和条件变量之前等待所有工作线程完成
        // }
        // printf("merge = %c\n", myresult[0]->head);

        mergeresult();
        pthread_mutex_lock(&mutexQueue);
        flag=1;
        pthread_cond_broadcast(&condQueue);
        pthread_mutex_unlock(&mutexQueue);
        for (i = 0; i < thread_num; i++) {
        //等待第i个工作线程完成并退出
            pthread_join(th[i], NULL);
        //确保所有工作线程都已完成它们的任务并退出，然后主线程才会继续执行。这样可以确保主线程在销毁互斥锁和条件变量之前等待所有工作线程完成
        }
        
        

    }else{//是串行
            //read from first file in binary mode
        for (int i = 1; i < argc; i++) {
            input_file = fopen(argv[i], "rb");
            if (input_file == NULL) {
                fprintf(stderr,"argv[1] is %s\n",argv[1]);
                fprintf(stderr,"%d",strcmp(argv[1],"-j"));
                fprintf(stderr, "Error opening file: %s\n", argv[i]);
                return 1;
            }

            rle_encode(input_file, &current_char, &count);
            fclose(input_file);
        }
    //After processing all input files, if current_char is not equal to -1, output the last character and its count to the standard output.
        if (current_char != -1) {
            putchar(current_char);
            putchar(count);
        }

        return 0;

    }
    return 0;
}