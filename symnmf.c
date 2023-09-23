#include "symnmf.h"

int size_vec,num_vectors;

double** goal_manager(int goal, double** vectors, int vec_size, int vectors_num){
    size_vec = vec_size;
    num_vectors = vectors_num;
    double** A_matrix;
    switch(goal){
        case 1:
            A_matrix = similarity_matrix(vectors);
            break;
        case 2:
            A_matrix = create_mat(num_vectors,size_vec);
            break;
        default:
            A_matrix = NULL;
            break;
    }
    return A_matrix;
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