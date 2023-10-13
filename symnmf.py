import random
import math
import numpy as np
import pandas as pd
import symnmfmodule
import sys

np.random.seed(0)

def d(p,q):
    sum = 0
    for i in range(len(p)):
        sum += (p[i]-q[i])**2
    return sum

def arg_parsing():
    try:

        args = sys.argv
        #check length of args and if the input is valid
        if not len(args)==4 :
            raise Exception
        
        k = int(args[1])
        goal = args[2]
        if goal not in ["symnmf", "sym", "ddg", "norm"]:
            raise Exception
        file_name = args[3]

        mat = run_symnmf(k, goal,file_name)
        print_mat(mat)
    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)


def print_mat(mat):
    for row in mat:
        for i in range(len(row)):
            elem = row[i]
            if i != 0 and i != len(row):
                print(",", end="")
            print(f"{elem:.4f}", end="")
        print("")

def calc_mat_avg(mat):
    avg = 0
    for i in range(len(mat)):
        for j in range(len(mat[0])):
            avg += mat[i][j]
    avg = avg / (len(mat) * len(mat[0]))
    return avg

def init_H(k, n, m):
    high = 2 * (m / k) ** 0.5
    return np.random.uniform(high=high, size=(n, k)).tolist()


def run_symnmf(k,goal, input_filename):
    goal_to_num = {"sym":1,"ddg":2,"norm":3,"symnmf":4}
    vectors = pd.read_csv(input_filename,header=None)
    vectors = vectors.values.tolist()
    if goal_to_num.get(goal) == 4:
        W = symnmfmodule.fit(3, vectors, len(vectors), len(vectors[0]))
        H = init_H(k,len(W),calc_mat_avg(W))
        H = symnmfmodule.symnmf(H,W,len(vectors), k)
        return H
    else:
        mat = symnmfmodule.fit(goal_to_num.get(goal), vectors, len(vectors), len(vectors[0]))
        return mat

if __name__ == '__main__':
    arg_parsing()
    


