import random
import numpy as np
import pandas as pd
import symnmfmodule
import sys
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

        
        run_kmeans_pp(3, 100,file_name)
    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)






def run_kmeans_pp(k,goal, input_filename):
    vectors = pd.read_csv(input_filename,header=None)
    vectors = vectors.values.tolist()
    centroids = symnmfmodule.fit(1, vectors, len(vectors), len(vectors[0]))
    print(len(centroids),len(centroids[0]))
    print(centroids[0][0],centroids[0][1] )
    # for centroid in centroids:
    #     for i, element in enumerate(centroid):
    #         if i != 0:
    #             print(",", end="")
    #         print(f"{element:.4f}", end="")
    #     print()

if __name__ == '__main__':

    
    arg_parsing()
    


