#include "symnmf.h"
#include <math.h>


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


double** goal_manager(int goal, double** vectors, int vec_size, int vectors_num){
    size_vec = vec_size;
    num_vectors = vectors_num;
    double** res_matrix;
    switch(goal){
        case 1: // Similarity Matrix
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
double** calc_H(double** init_H,double** W, int vec_size, int vectors_num){
    size_vec = vec_size;
    num_vectors = vectors_num;

    int max_iter = 1000; 
    double epsilon = 1e-6; 
    double beta = 0.5;
    int i, j, l;

    for(int iter = 0; iter < max_iter; iter++) {
        double** H_old = copy_matrix(init_H, num_vectors, size_vec);
        
        for(i = 0; i < num_vectors; i++) {
            for(j = 0; j < size_vec; j++) {
                double numerator = 0.0, denominator = 0.0;
                for(l = 0; l < num_vectors; l++) {
                    numerator += W[i][l] * init_H[l][j];
                    denominator += init_H[i][l] * init_H[l][j] * init_H[l][j];
                }
                
                if(denominator == 0)
                    continue;
                
                init_H[i][j] *= (1 - beta + beta * numerator / denominator);
            }
        }
        
        if(frobenius_norm_difference(init_H, H_old, num_vectors, size_vec) < epsilon) {
            free_matrix(H_old, num_vectors);
            break;
        }
        
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

