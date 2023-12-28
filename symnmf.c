#include "symnmf.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int size_vec,num_vectors;

double** create_mat(int vec_num, int vec_size);
double** goal_manager(int goal, double** vectors, int vec_size, int vectors_num);
double** calc_H(double** init_H,double** W, int vec_size, int vectors_num);
double** copy_matrix(double** source, int rows, int cols);
double frobenius_norm_difference(double** A, double** B, int rows, int cols);
void free_matrix(double** matrix, int rows);
double** similarity_matrix(double** vectors);
double** diagonal_degree_matrix(double** A_matrix);
double** normalized_similarity_matrix(double** A_matrix, double** D_matrix);
double vector_distance(double* p, double* q);
void print_matrix(double** matrix, int rows, int cols);


double** goal_manager(int goal, double** vectors, int vec_size, int vectors_num){
    double** res_matrix;
    double** D_matrix;
    size_vec = vec_size;
    num_vectors = vectors_num;

    
    switch(goal){
        case 1: /* Similarity Matrix */
            res_matrix = similarity_matrix(vectors);
            break;
        case 2: /*  Diagonal Degree Matrix */
            res_matrix = similarity_matrix(vectors);
            res_matrix = diagonal_degree_matrix(res_matrix);
            break;
        case 3: /*  Normalized Similarity Matrix */
            res_matrix = similarity_matrix(vectors);
            D_matrix = diagonal_degree_matrix(res_matrix);
            res_matrix = normalized_similarity_matrix(res_matrix, D_matrix);
            /*  Free D_matrix as it is no longer needed */
            free_matrix(D_matrix, num_vectors);
            break;
        default:
            res_matrix = NULL;
            break;
    }
    return res_matrix;
}

/*  Helper function: Calculate Frobenius norm of the difference between two matrices */
double frobenius_norm_diff(double** A, double** B, int rows, int cols) {
    double sum = 0.0;
    int i;
    int j;
    double diff;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            diff = A[i][j] - B[i][j];
            sum += diff * diff;
        }
    }
    return sum;
}

/*  Helper function: Matrix multiplication */
double** matrix_multiply(double** A, double** B, int rowsA, int colsA, int rowsB, int colsB) {
    double** C;
    int i;
    int j;
    int k;

    if (colsA != rowsB) {
        /*  Handle error: matrices cannot be multiplied */
        return NULL;
    }

    C = malloc(rowsA * sizeof(double*));
    for (i = 0; i < rowsA; i++) {
        C[i] = malloc(colsB * sizeof(double));
        for (j = 0; j < colsB; j++) {
            C[i][j] = 0.0;
            for (k = 0; k < colsA; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

/*  Helper function: Matrix transpose */
double** matrix_transpose(double** A, int rows, int cols) {
    double** AT;
    int i;
    int j;
    AT = malloc(cols * sizeof(double*));
    for (i = 0; i < cols; i++) {
        AT[i] = malloc(rows * sizeof(double));
        for (j = 0; j < rows; j++) {
            AT[i][j] = A[j][i];
        }
    }
    return AT;
}

/*  Main function: Calculate H */
double** calc_H(double** H_init, double** W, int vectors_num, int k) {

    int max_iter = 300;
    double epsilon = 1e-4;
    double beta = 0.5;
    int i;
    int j;
    double** H;
    int iter;
    double** HT;
    double** HHT;
    double** HHTH;
    double** WH;

    /*  Allocate memory and initialize H */
    H = malloc(vectors_num * sizeof(double*));
    for (i = 0; i < vectors_num; i++) {
        H[i] = malloc(k * sizeof(double));
        for (j = 0; j < k; j++) {
            H[i][j] = H_init[i][j];
        }
    }



    for (iter = 0; iter < max_iter; iter++) {
        /*  Update H according to the provided rule */
        
        HT = matrix_transpose(H, vectors_num, k);  /*  Transpose of H at iteration i */
        HHT = matrix_multiply(H, HT, vectors_num, k, k, vectors_num);  /*  H*(H^T) */
        HHTH = matrix_multiply(HHT, H, vectors_num, vectors_num, vectors_num, k);  /*  H*(H^T)*H */
        WH = matrix_multiply(W, H, vectors_num, vectors_num, vectors_num, k);  /*  W*H */
        
        for (i = 0; i < vectors_num; i++) {
            for (j = 0; j < k; j++) {
                H[i][j] = H_init[i][j] * (1 - beta + beta * (WH[i][j] / HHTH[i][j]));
            }
        }
        
        /*  Free memory */
        free(HT);
        free(HHT);
        free(HHTH);
        free(WH);
        
        /*  Check stopping criteria */
        if (frobenius_norm_diff(H, H_init, vectors_num, k) < epsilon) {
            break;
        }

        for (i = 0; i < vectors_num; i++) {
            for (j = 0; j < k; j++) {
                H_init[i][j] = H[i][j];
            }
        }
    }

    for (i = 0; i < vectors_num; i++) {
        free(H_init[i]);
    }
    free(H_init);

    return H;
}

void deriveClusters(double** H, int* clusters, int n, int k) {
    int i;
    int j;
    float max_val;
    int max_idx;

    for(i = 0; i < n; i++) {
        max_idx = 0;
        max_val = H[i][0];
        for(j = 1; j < k; j++) {
            if(H[i][j] > max_val) {
                max_val = H[i][j];
                max_idx = j;
            }
        }
        clusters[i] = max_idx;
    }
}


double** copy_matrix(double** source, int rows, int cols) {
    double** dest;
    int i;
    int j;

    dest = create_mat(rows, cols);
    for(i = 0; i < rows; i++) {
        for(j = 0; j < cols; j++) {
            dest[i][j] = source[i][j];
        }
    }
    return dest;
}


void free_matrix(double** matrix, int rows) {
    int i;
    for(i = 0; i < rows; i++)
        free(matrix[i]);
    free(matrix);
}

double** create_mat(int vec_num, int vec_size){
    int i;
    double **mat;
    /* mat = calloc(vec_num, sizeof(double*)); */
    mat = calloc(vec_num, vec_num*sizeof(double));
    if (!mat)
    {
        return NULL;
    }
    for (i=0; i < vec_num; i++)
    {
        mat[i] = calloc(vec_size, sizeof(double));
        if (!mat[i])
        {
            return NULL;
        }
    }
    return mat;
}

double vector_distance(double* p, double* q){
    double sum=0;
    int i;
    for(i=0;i<size_vec;i++){
        sum = sum + pow(p[i] - q[i],2);
    }
    return sum;
}

/* 1.1 */
double** similarity_matrix(double** vectors){
    int i,j;
    double** A_matrix;
    A_matrix = create_mat(num_vectors,num_vectors);
    for(i=0;i<num_vectors;i++){
        for(j=0;j<num_vectors;j++){
            if(i!=j){
                A_matrix[i][j] = exp(-vector_distance(vectors[i],vectors[j])/2);
            }
            else{
                A_matrix[i][j] = 0;
            }
        }
    }
    return A_matrix;

}

/* 1.2 */
double** diagonal_degree_matrix(double** A_matrix) {
    int i;
    int j;
    double** D_matrix;

    D_matrix = create_mat(num_vectors, num_vectors);
    for(i = 0; i < num_vectors; i++) {
        double sum = 0;
        for(j = 0; j < num_vectors; j++) {
            sum += A_matrix[i][j];
        }
        D_matrix[i][i] = sum; /*  Diagonal elements are the degrees */
    }
    return D_matrix;
}

/* 1.3 */
double** normalized_similarity_matrix(double** A_matrix, double** D_matrix) {
    double** W_matrix;
    int i;
    int j;

    W_matrix = create_mat(num_vectors, num_vectors);
    for(i = 0; i < num_vectors; i++) {
        for(j = 0; j < num_vectors; j++) {
            if(D_matrix[i][i] != 0 && D_matrix[j][j] != 0) {
                W_matrix[i][j] = A_matrix[i][j] / (sqrt(D_matrix[i][i]) * sqrt(D_matrix[j][j]));
            } else {
                W_matrix[i][j] = 0;
            }
        }
    }
    return W_matrix;
}

double** read_input_file(const char* file_name, int* vec_size, int* vectors_num) {
    int i;
    int j;
    char *token;
    FILE *file;
    char line[1024]; /*  Adjust the size as needed */
    double **vectors;

    file = fopen(file_name, "r");

    if(!file) {
        perror("Error opening file");
        return NULL;
    }
    

    *vectors_num = 0;
    *vec_size = 0;


    /*  Count the number of lines (vectors) and the size of vectors in the file */
    while(fgets(line, sizeof(line), file)) {
        if(strcmp(line, "\n") == 0) break; /*  Stop at an empty line */

        token = strtok(line, ",");
        while(token) {
            if(*vectors_num == 0) (*vec_size)++;
            token = strtok(NULL, ",");
        }
        (*vectors_num)++;
    }
    
    /*  Allocate memory for vectors */
    vectors = create_mat(*vectors_num, *vec_size);
    if(!vectors) {
        fclose(file);
        return NULL;
    }

    /*  Reset the file pointer to the beginning of the file */
    rewind(file);
    
    /*  Read the vectors from the file */
    for(i = 0; i < *vectors_num; i++) {
        if(!fgets(line, sizeof(line), file) || strcmp(line, "\n") == 0) break;

        token = strtok(line, ",");
        for(j = 0; token && j < *vec_size; j++) {
            vectors[i][j] = atof(token);
            token = strtok(NULL, ",");
        }
    }
    
    fclose(file);
    return vectors;
}

void print_matrix(double** matrix, int rows, int cols) {
    int i;
    int j;
    

    for(i = 0; i < rows; i++) {
        for(j = 0; j < cols; j++) {
            printf("%.4f ", matrix[i][j]);
            if(j < cols - 1) printf(",");
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    const char* goal;
    const char* file_name;
    int goal_code;
    int vec_size, vectors_num;
    double** vectors;
    double** result_matrix;


    if(argc != 3) {
        fprintf(stderr, "An Error Has Occurred .\n Usage: %s <goal> <file_name>\n", argv[0]);
        return 1;
    }
    
    goal = argv[1];
    file_name = argv[2];
    
    if(strcmp(goal, "sym") == 0) goal_code = 1;
    else if(strcmp(goal, "ddg") == 0) goal_code = 2;
    else if(strcmp(goal, "norm") == 0) goal_code = 3;
    else {
        fprintf(stderr,"An Error Has Occurred .\n Invalid goal: %s. Valid goals are 'sym', 'ddg', and 'norm'.\n", goal);
        return 1;
    }

    
    vectors = read_input_file(file_name, &vec_size, &vectors_num);
    if(!vectors) {
        fprintf(stderr, "An Error Has Occurred .\n Failed to read input file: %s\n", file_name);
        return 1;
    }

    result_matrix = goal_manager(goal_code, vectors, vec_size, vectors_num);
    if(!result_matrix) {
        fprintf(stderr, "An Error Has Occurred .\n Failed to calculate the result matrix for goal: %s\n", goal);
        return 1;
    }
    
    print_matrix(result_matrix, vectors_num, vectors_num);
    
    /*  Clean up allocated memory. */
    free_matrix(result_matrix, vectors_num);
    free_matrix(vectors, vectors_num);
    
    return 0;
}
