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
    size_vec = vec_size;
    num_vectors = vectors_num;
    double** res_matrix;
    switch(goal){
        case 1: //Similarity Matrix
            res_matrix = similarity_matrix(vectors);
            break;
        case 2: // Diagonal Degree Matrix
            res_matrix = similarity_matrix(vectors);
            res_matrix = diagonal_degree_matrix(res_matrix);
            break;
        case 3: // Normalized Similarity Matrix
            res_matrix = similarity_matrix(vectors);
            double** D_matrix = diagonal_degree_matrix(res_matrix);
            res_matrix = normalized_similarity_matrix(res_matrix, D_matrix);
            // Free D_matrix as it is no longer needed
            free_matrix(D_matrix, num_vectors);
            break;
        default:
            res_matrix = NULL;
            break;
    }
    return res_matrix;
}

//1.4
double** calc_H(double** init_H,double** W, int vectors_num, int k){

    num_vectors = vectors_num;

    int max_iter = 300; 
    double epsilon = 1e-4;
    double beta = 0.5;
    int i, j, l;

    for(int iter = 0; iter < max_iter; iter++) {
        printf("Iteration %d\n", iter);
        double** H_old = copy_matrix(init_H, num_vectors, k);
        
        for(i = 0; i < num_vectors; i++) {
            for(j = 0; j < k; j++) {
                double numerator = 0.0, denominator = 0.0;
                for(l = 0; l < num_vectors; l++) {
                    numerator += W[i][l] * H_old[l][j];
                    denominator += H_old[i][l] * H_old[l][j] * H_old[l][j];
                }
                
                if(denominator == 0)
                    continue;
                
                init_H[i][j] *= (1 - beta + beta * numerator / denominator);
            }
        }        
        if(frobenius_norm_difference(init_H, H_old, num_vectors, k) < epsilon) {
            free_matrix(H_old, num_vectors);
            break;
        }
        printf("Frobenius norm difference: %lf\n", frobenius_norm_difference(init_H, H_old, num_vectors, k));
        free_matrix(H_old, num_vectors);
    }
    return init_H;
}

double** copy_matrix(double** source, int rows, int cols) {
    double** dest = create_mat(rows, cols);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            dest[i][j] = source[i][j];
        }
    }
    return dest;
}

double frobenius_norm_difference(double** A, double** B, int rows, int cols) {
    double sum = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            double diff = A[i][j] - B[i][j];
            sum += diff * diff;
        }
    }
    return sqrt(sum);
}

void free_matrix(double** matrix, int rows) {
    for(int i = 0; i < rows; i++)
        free(matrix[i]);
    free(matrix);
}

double** create_mat(int vec_num, int vec_size){
    int i;
    double **mat;
    //mat = calloc(vec_num, sizeof(double*));
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

//1.1
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

//1.2
double** diagonal_degree_matrix(double** A_matrix) {
    double** D_matrix = create_mat(num_vectors, num_vectors);
    for(int i = 0; i < num_vectors; i++) {
        double sum = 0;
        for(int j = 0; j < num_vectors; j++) {
            sum += A_matrix[i][j];
        }
        D_matrix[i][i] = sum; // Diagonal elements are the degrees
    }
    return D_matrix;
}

//1.3
double** normalized_similarity_matrix(double** A_matrix, double** D_matrix) {
    double** W_matrix = create_mat(num_vectors, num_vectors);
    for(int i = 0; i < num_vectors; i++) {
        for(int j = 0; j < num_vectors; j++) {
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
    FILE *file = fopen(file_name, "r");
    if(!file) {
        perror("Error opening file");
        return NULL;
    }
    
    char line[1024]; // Adjust the size as needed
    *vectors_num = 0;
    *vec_size = 0;

    // Count the number of lines (vectors) and the size of vectors in the file
    while(fgets(line, sizeof(line), file)) {
        if(strcmp(line, "\n") == 0) break; // Stop at an empty line

        char *token = strtok(line, ",");
        while(token) {
            if(*vectors_num == 0) (*vec_size)++;
            token = strtok(NULL, ",");
        }
        (*vectors_num)++;
    }
    
    // Allocate memory for vectors
    double **vectors = create_mat(*vectors_num, *vec_size);
    if(!vectors) {
        fclose(file);
        return NULL;
    }

    // Reset the file pointer to the beginning of the file
    rewind(file);
    
    // Read the vectors from the file
    for(int i = 0; i < *vectors_num; i++) {
        if(!fgets(line, sizeof(line), file) || strcmp(line, "\n") == 0) break;

        char *token = strtok(line, ",");
        for(int j = 0; token && j < *vec_size; j++) {
            vectors[i][j] = atof(token);
            token = strtok(NULL, ",");
        }
    }
    
    fclose(file);
    return vectors;
}

void print_matrix(double** matrix, int rows, int cols) {
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            printf("%lf", matrix[i][j]);
            if(j < cols - 1) printf(",");
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <goal> <file_name>\n", argv[0]);
        return 1;
    }
    
    const char* goal = argv[1];
    const char* file_name = argv[2];
    
    int goal_code;
    if(strcmp(goal, "sym") == 0) goal_code = 1;
    else if(strcmp(goal, "ddg") == 0) goal_code = 2;
    else if(strcmp(goal, "norm") == 0) goal_code = 3;
    else {
        fprintf(stderr, "Invalid goal: %s. Valid goals are 'sym', 'ddg', and 'norm'.\n", goal);
        return 1;
    }

    int vec_size, vectors_num;
    double** vectors = read_input_file(file_name, &vec_size, &vectors_num);
    if(!vectors) {
        fprintf(stderr, "Failed to read input file: %s\n", file_name);
        return 1;
    }

    double** result_matrix = goal_manager(goal_code, vectors, vec_size, vectors_num);
    if(!result_matrix) {
        fprintf(stderr, "Failed to calculate the result matrix for goal: %s\n", goal);
        return 1;
    }
    
    print_matrix(result_matrix, vectors_num, vectors_num);
    
    // Clean up allocated memory.
    free_matrix(result_matrix, vectors_num);
    free_matrix(vectors, vectors_num);
    
    return 0;
}
