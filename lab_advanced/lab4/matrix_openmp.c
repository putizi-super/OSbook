 //#include<stdio.h>
 #include"/home/zby/local/include/omp.h"
/**
 int main() {
 	int sum = 0;
 #pragma omp parallel for
 	for (int i = 0; i < 100000; i++) {
 		sum = sum + i;
 	}
 	printf("%d", sum);
 	return 0;
 }
**/
/**
 * @file mm0.c
 * @author By
 * @brief basic mm(row)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#define bool int
#define true 1
#define false 0


void read_csv(int file_no, float* array) {
    FILE * fp = NULL;
    char *line, *word;
    char buffer[10000];
    int i = 0;
    if(file_no == 0){
        fp = fopen("./dataset/shape_1000/A_1000.csv", "r");
    } else if(file_no == 1) {
        fp = fopen("./dataset/shape_1000/B_1000.csv", "r");
    } else if(file_no == 2) {
        fp = fopen("./dataset/shape_1000/C_1000.csv", "r");
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


typedef struct
{
  float* matrix_A;
  float* matrix_B;
  float* matrix_C;
  int m_from;
  int m_to;
  int k;
  int n;
} mul_thread_args;

void* matrix_mul_th(void* args){
    mul_thread_args* mul_args = (mul_thread_args*) args;

    int k = mul_args->k;
    int n = mul_args->n;
    float(*A)[k] = (float(*)[k]) mul_args->matrix_A;
    float(*B)[n] = (float(*)[n]) mul_args->matrix_B;
    float(*C)[n] = (float(*)[n]) mul_args->matrix_C;

    int mi, ki, ni;
    for(mi = mul_args->m_from; mi < mul_args->m_to; mi++){
        for(ni = 0; ni < n; ni++){
            for(ki = 0; ki < k; ki++){
                C[mi][ni] += A[mi][ki] * B[ki][ni];
            }
        }
    }

    return NULL;
}

void matrix_mul(float* matrix_A, float* matrix_B, float* matrix_C, int m, int k, int n){
    float(*A)[k] = (float(*)[k]) matrix_A;
    float(*B)[n] = (float(*)[n]) matrix_B;
    float(*C)[n] = (float(*)[n]) matrix_C;

    memset(C, 0, m*n*sizeof(float));

    //在使用openmp时，需要把归纳变量的声明写到for中
    //int mi, ki, ni;
	#pragma omp parallel for
    {
    for(int mi = 0; mi < m; mi++){
        for(int ni = 0; ni < n; ni++){
            for(int ki = 0; ki < k; ki++){
                C[mi][ni] += A[mi][ki] * B[ki][ni];
            }
        }
    }
    }
}


int main(){
    // int m = 3, k = 3, n = 5, i, j;
    // float* A = malloc(3*3*sizeof(float));
    // float* B = malloc(3*5*sizeof(float));
    // float* C = malloc(3*5*sizeof(float));

    // A[0] = 1; A[1] = 2; A[2] = 3;
    // A[3] = 4; A[4] = 5; A[5] = 6;
    // A[6] = 7; A[7] = 8; A[8] = 9;

    // B[0] = 1; B[1] = 1; B[2] = 1; B[3] = 1; B[4] = 1;
    // B[5] = 2; B[6] = 2; B[7] = 2; B[8] = 2; B[9] = 2;
    // B[10] = 3; B[11] = 3; B[12] = 3; B[13] = 3; B[14] = 3;

    // matrix_mul(A, B, C, m, k, n);

    // for(i = 0; i < m; i++){
    //     for(j = 0; j < n; j++){
    //         printf("%f\t", C[i*n + j]);
    //     }
    //     printf("\n");
    // }

    // free(A);
    // free(B);
    // free(C);
    int m = 1000, k = 1000, n = 1000, i, j;
    float* A = malloc(m*k*sizeof(float));
    float* B = malloc(k*n*sizeof(float));
    float* C = malloc(m*n*sizeof(float));
    float* C_answer = malloc(m*n*sizeof(float));

    read_csv(0, A);
    read_csv(1, B);
    read_csv(2, C_answer);

    matrix_mul(A, B, C, m, k, n);
	
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

