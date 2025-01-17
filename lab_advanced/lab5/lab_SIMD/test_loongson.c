#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <lsxintrin.h>
#include <lasxintrin.h>

extern __inline __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__m128    __lsx_setr_s (float __A, float __B, float __C, float __D)
{
  return __extension__ (__m128){ __A, __B, __C, __D };
}

extern __inline __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__m256    __lasx_setr_s (float __A, float __B, float __C, float __D,float __E, float __F, float __G, float __H)
{
  return __extension__ (__m256){ __A, __B, __C, __D ,__E,  __F,  __G,  __H};
}

void print_matrix(float**m, int rows, int cols) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            printf("%f ", m[i][j]);
        }
        printf("\n");
    }
}

void common_matrix_multiplication(float**matrix, float**matrix_1, float**matrix_2, int m, int n, int r) {
    for (int i=0; i<m; i++) {
        for (int j=0; j<r; j++) {
            for (int k=0; k<n; k++) {
                matrix[i][j] += matrix_1[i][k] * matrix_2[k][j];
            }
        }
    }
}

void lsx_matrix_multiplication(float**matrix, float**matrix_1, float**matrix_2, int m, int n, int r) {
    float*tmp = (float*)malloc(4*sizeof(float));
    for (int i=0; i<4; i++) tmp[i] = 0.f;
    for (int i=0; i<m; i++) {
        for (int j=0; j<r; j++) {
            for (int k=0; k<n; k+=4) {
		__lsx_vst(__lsx_vfmul_s( 
                      (__m128)__lsx_vld((float*)(matrix_1[i]+k), 0),
                      __lsx_setr_s(matrix_2[k][j], matrix_2[k+1][j], matrix_2[k+2][j], matrix_2[k+3][j])), tmp, 0);
                matrix[i][j] += tmp[0] + tmp[1] + tmp[2] + tmp[3];
            }
        }
    }  
}

void lasx_matrix_multiplication(float**matrix, float**matrix_1, float**matrix_2, int m, int n, int r) {
    float*tmp = (float*)malloc(8*sizeof(float));
    for (int i=0; i<8; i++) tmp[i] = 0.f;
    for (int i=0; i<m; i++) {
        for (int j=0; j<r; j++) {
		for (int k=0; k<n; k+=8) {
	            __lasx_xvst(__lasx_xvfmul_s(
			   (__m256)__lasx_xvld((float*)(matrix_1[i]+k), 0),
			   __lasx_setr_s(matrix_2[k][j], matrix_2[k+1][j], matrix_2[k+2][j], matrix_2[k+3][j],matrix_2[k+4][j], 
			   matrix_2[k+5][j], matrix_2[k+6][j], matrix_2[k+7][j])), tmp, 0);
		    matrix[i][j] += tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
		}
        }
    }
}

int main(int argc, char* argv[]) {               

    // read matrix size from one input file
    FILE* fp1 = fopen("input/input1.txt", "r");
    int m = 0, n = 0, r = 0; // mat_1 = m x n, mat_2 = n x r
    fscanf(fp1, "%d %d %d", &m, &n, &r);
    fclose(fp1);

    // allocate memory for 3 matrices
    float**matrix_1 = (float**)malloc(m*sizeof(float*));
    for (int i=0; i<m; i++) {
        matrix_1[i] = (float*)malloc(n*sizeof(float));
    }
    float**matrix_2 = (float**)malloc(n*sizeof(float*));
    for (int i=0; i<n; i++) {
        matrix_2[i] = (float*)malloc(r*sizeof(float));
    }
    float**matrix = (float**)malloc(m*sizeof(float*));
    for (int i=0; i<r; i++) {
        matrix[i] = (float*)malloc(r*sizeof(float));
    }

    // initialize matrices
    // srand((unsigned)time(NULL));
    for (int i=0; i<m; i++) {
        for (int j=0; j<n; j++) {
            matrix_1[i][j] = (float)(rand() / (float)RAND_MAX);
        }
    }
    for (int i=0; i<n; i++) {
        for (int j=0; j<r; j++) {
            matrix_2[i][j] = (float)(rand() / (float)RAND_MAX);
        }
    }
    for (int i=0; i<m; i++) {
        for (int j=0; j<r; j++) {
            matrix[i][j] = 0.f;
        }
    }

    // matrix multiplication
    clock_t start_t, end_t, clock_dif; double clock_dif_sec;
    
    for (int i=0; i<10; i++) {
        //common_matrix_multiplication(matrix, matrix_1, matrix_2, m, n, r);
        //lsx_matrix_multiplication(matrix, matrix_1, matrix_2, m, n, r);
        lasx_matrix_multiplication(matrix, matrix_1, matrix_2, m, n, r);
    }

    end_t = clock(); clock_dif = end_t - start_t;
    clock_dif_sec = (double) (clock_dif/1000000.0);
    double avg_cost = clock_dif_sec / 10;

    // write result to an output file
    FILE* fp2 = fopen("output/lasx_input1.txt", "w+");
    fprintf(fp2, "%lf", avg_cost);
    fclose(fp2);
}

