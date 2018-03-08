#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include IMPL
#define MAX_TABLE_SIZE 4096

#if OPT == 1
#define OUT_FILE "opt.txt"

entry *hashTable[MAX_TABLE_SIZE] = {NULL};
#else
#define OUT_FILE "orig.txt"
#endif

#define DICT_FILE "./dictionary/words.txt"

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

unsigned int BKDRhash(char *cPtr)
{
    int hash = 0;
    int seed = 31; 
    do
        hash = hash * seed + (*cPtr++);
    while(*cPtr != '\0');
    return (hash & (MAX_TABLE_SIZE -1)); //we learn from class
}

int main(int argc, char *argv[])
{

    FILE *fp;
    //int i = 0;
    char line[MAX_LAST_NAME_SIZE];
    struct timespec start, end;
    double cpu_time1, cpu_time2;

    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (fp == NULL) {
        printf("cannot open the file\n");
        return -1;
    }

    /* build the entry */
    entry *pHead, *e;
    pHead = (entry *) malloc(sizeof(entry));
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
    e->pNext = NULL;

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(line, sizeof(line), fp)) {
        line[strlen(line) -1] = '\0'; //change while loop to strlen
#if OPT == 1 //compilering OPT version
        unsigned int hashValue = BKDRhash(line);
        if(hashTable[hashValue] == NULL){
            hashTable[hashValue] =(entry*) malloc(sizeof(entry)); 
        }
        append(line , hashTable[hashValue]);  
#else       //compilering orgin version  
        e = append(line, e);
#endif
    }
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

    /* close file as soon as possible */
    fclose(fp);
    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";

#if OPT == 1 //compilering OPT version
    e = hashTable[BKDRhash(input)];
#else
    e = pHead;
#endif

    assert(findName(input, e) &&
           "Did you implement findName() in " IMPL "?");
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));



#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
    //printf("%s",findName(input, e)->lastName );
    findName(input, e);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    FILE *output = fopen(OUT_FILE, "a");
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

#if OPT == 1 //compilering OPT version
    int counter = 0;
    do
    {
        free(hashTable[counter] -> pNext);
        free(hashTable[(counter++)] );

    }while(counter < MAX_TABLE_SIZE);
#else
    if (pHead != NULL) {
        free(pHead->pNext);
        free(pHead);
    }
#endif

    return 0;
}


