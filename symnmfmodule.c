#define PY_SSIZE_T_CLEAN
#include "symnmf.h"
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define int_max 2147483647

double global_delta_uk = 1000000;
int num_vectors,size_vec;
double epsilon;
double **data_points_c,**mat_c;


double** create_mat(int vec_num, int vec_size);
static PyObject* fit(PyObject *self, PyObject *args);
double** parse_py_table_to_C(PyObject *lst, int vec_num, int vec_size);


double** parse_py_table_to_C(PyObject *lst, int vec_num, int vec_size)
{
    
    int row, col;
    double **data_points_c = create_mat(vec_num, vec_size);
    if (!data_points_c)
    {
        return NULL;
    }
    for (row = 0; row < vec_num ; row++)
    {
        PyObject *vector = PyList_GetItem(lst, row);
        for (col = 0 ; col < vec_size ; col++)
        {
            data_points_c[row][col] = PyFloat_AsDouble(PyList_GetItem(vector, col));
        }
    }
    return data_points_c;

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
    double** A_matrix;
    A_matrix = (double**)calloc(num_vectors,num_vectors*sizeof(double));
    if(!A_matrix){
        return NULL;
    }
    int i,j;
    for(i=0;i<num_vectors;i++){
        A_matrix[i] = (double*)calloc(num_vectors,sizeof(double));
        if(!A_matrix[i]){
            return NULL;
        }
    }
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

PyObject* parse_list_to_py(double** centroids_c,int row,int col)
{
    PyObject *centroid_py, *centroids_py;
    int i, j;
    centroids_py = PyList_New(0);
    /* ask if necessary*/
    if (!centroids_py)
    {
        return NULL;
    }
    for (i = 0; i < row ; i++)
    {
        centroid_py = PyList_New(0);
        if (centroid_py == NULL)
        {
            return NULL;
        }
        for (j = 0 ; j < col ; j++)
        {
            PyList_Append(centroid_py, PyFloat_FromDouble(centroids_c[i][j]));
        }
        PyList_Append(centroids_py, centroid_py);
    }
    return centroids_py;
}

static PyObject* fit(PyObject *self, PyObject *args)
{
    double** result_c;
    int k, goal, total_vec_number, vec_size,i;
    //double **data_points_c,**A_mat_c;
    PyObject *data_points_py, *result_py;
    if (!PyArg_ParseTuple(args, "iOii", &goal, &data_points_py, &total_vec_number, &vec_size))
    {
        return NULL;
    }

    size_vec = vec_size;
    data_points_c = parse_py_table_to_C(data_points_py, total_vec_number, size_vec);
    num_vectors = total_vec_number;
    
    result_c = goal_manager(goal, data_points_c, size_vec, num_vectors);
    
    result_py = parse_list_to_py(result_c, num_vectors, num_vectors);
    
   
    for (i = 0; i < num_vectors ; i++)
    {
        free(data_points_c[i]);
        free(result_c[i]);
    }
    free(data_points_c);
    free(result_c);
    
    return result_py;
}

static PyObject* symnmf(PyObject *self, PyObject *args)
{
    double** result_c;
    int k, goal, total_vec_number, vec_size,i;
    //double **data_points_c,**A_mat_c;
    PyObject *data_points_py, *result_py;
    if (!PyArg_ParseTuple(args, "iOii", &goal, &data_points_py, &total_vec_number, &vec_size))
    {
        return NULL;
    }

    size_vec = vec_size;
    data_points_c = parse_py_table_to_C(data_points_py, total_vec_number, size_vec);
    num_vectors = total_vec_number;
    
    result_c = goal_manager(goal, data_points_c, size_vec, num_vectors);
    
    result_py = parse_list_to_py(result_c, num_vectors, num_vectors);
    
   
    for (i = 0; i < num_vectors ; i++)
    {
        free(data_points_c[i]);
        free(result_c[i]);
    }
    free(data_points_c);
    free(result_c);
    
    return result_py;
}



/* methods from class*/

static PyMethodDef capiMethods[] = {
        {"fit",
                (PyCFunction) fit,
                     METH_VARARGS,
                PyDoc_STR("returns calculated centroids for given matrix of data points")},
        {"symnmf",
                (PyCFunction) symnmf,
                     METH_VARARGS,
                PyDoc_STR("returns calculated centroids for given matrix of data points")},
        
        {NULL, NULL, 0, NULL}
};




static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "symnmfmodule",
        NULL,
        -1,
        capiMethods
};

PyMODINIT_FUNC
PyInit_symnmfmodule(void) {
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m){
        return NULL;
    }
    return m;
}