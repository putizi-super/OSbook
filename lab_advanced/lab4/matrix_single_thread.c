/**
 * @file mm0.c
 * @author By
 * @brief basic mm(row)
 */

#include <bits/types/struct_timeval.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>

#define bool int
#define true 1
#define false 0


void read_csv(int file_no, float* array) {
    FILE * fp = NULL;
    char *line, *word;
    char buffer[10000];
    int i = 0;
    if(file_no == 0){
        fp = fopen("./input/shape_1000/A_1000.csv", "r");
    } else if(file_no == 1) {
        fp = fopen("./input/shape_1000/B_1000.csv", "r");
    } else if(file_no == 2) {
        fp = fopen("./input/shape_1000/C_1000.csv", "r");
    } else{
        assert(0);
    }
    if(fp == NULL){
        assert(0);
    }
    while((line = fgets(buffer, sizeof(buffer), fp))!=NULL){
        word = strtok(line, ",");
        while (word != NULL){
            array[i++] = strtol(word, NULL, 10);
            word = strtok(NULL, ",");
        }
    }
}

bool test_result(float* result, float* answer, int len){
    int i;
    for(i = 0; i < len; i++){
        if(result[i] != answer[i]){
            return false;
        }
    }
    return true;
}


void matrix_mul(float* matrix_A, float* matrix_B, float* matrix_C, int m, int k, int n){
    float(*A)[k] = (float(*)[k]) matrix_A;
    float(*B)[n] = (float(*)[n]) matrix_B;
    float(*C)[n] = (float(*)[n]) matrix_C;

    memset(C, 0, m*n*sizeof(float));

     int mi, ki, ni;
     for(mi = 0; mi < m; mi++){
         for(ni = 0; ni < n; ni++){
             for(ki = 0; ki < k; ki++){
                 C[mi][ni] += A[mi][ki] * B[ki][ni];
             }
         }
     }
}

int main(){
    int m = 1000, k = 1000, n = 1000, i, j;
    float* A = malloc(m*k*sizeof(float));
    float* B = malloc(k*n*sizeof(float));
    float* C = malloc(m*n*sizeof(float));
    float* C_answer = malloc(m*n*sizeof(float));

    read_csv(0, A);
    read_csv(1, B);
    read_csv(2, C_answer);

    struct timeval start;
    struct timeval end;

    gettimeofday(&start,NULL);
    
    matrix_mul(A, B, C, m, k, n);

    gettimeofday(&end,NULL);

    float total_time;
    total_time = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
    printf("time = %f\n",total_time);
    if(test_result(C, C_answer, m*n)){
        printf("check pass!\n");
    }else{
        printf("answer wrong!\n");
    }
    

    free(A);
    free(B);
    free(C);
    free(C_answer);
    return 0;
}
