#define PY_SSIZE_T_CLEAN
#include "symnmf.h"
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define int_max 2147483647

double global_delta_uk = 1000000;

double epsilon;
double **data_points_c,**mat_c;


double** create_matrix(int vec_num, int vec_size);
static PyObject* fit(PyObject *self, PyObject *args);
double** parse_py_table_to_C(PyObject *lst, int vec_num, int vec_size);


double** parse_py_table_to_C(PyObject *lst, int vec_num, int vec_size)
{
    
    int row, col;
    double **data_points_c = create_matrix(vec_num, vec_size);
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


double** create_matrix(int vec_num, int vec_size){
    int i;
    double **mat;
    //mat = calloc(vec_num, sizeof(double*));
    mat = calloc(vec_num, vec_size*sizeof(double));
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
    int  goal, total_vec_number, vec_size,i;
    PyObject *data_points_py, *result_py;
    if (!PyArg_ParseTuple(args, "iOii", &goal, &data_points_py, &total_vec_number, &vec_size))
    {
        return NULL;
    }
    
    data_points_c = parse_py_table_to_C(data_points_py, total_vec_number, vec_size);
    result_c = goal_manager(goal, data_points_c, vec_size, total_vec_number);
    result_py = parse_list_to_py(result_c, total_vec_number, total_vec_number);
    
   
    for (i = 0; i < total_vec_number ; i++)
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
    
    // double **result_c;
    double **H_c;
    double **W_c;
    int  total_vec_number,k,i;
    PyObject *H_py, *W_py,*result_py;
    if (!PyArg_ParseTuple(args, "OOii", &H_py, &W_py, &total_vec_number, &k))
    {
        ;
        return NULL;
    }
    
    H_c = parse_py_table_to_C(H_py, total_vec_number, k);
    W_c = parse_py_table_to_C(W_py, total_vec_number, total_vec_number);
    
    H_c = calc_H(H_c,W_c, total_vec_number, k);
    
    // what is the returned matrix size? 
    result_py = parse_list_to_py(H_c, total_vec_number, k);
    
   
    for (i = 0; i < total_vec_number ; i++)
    {
        free(H_c[i]);
        free(W_c[i]);
        // free(result_c[i]);
    }
    free(W_c);
    // free(result_c);
    free(H_c);
    
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