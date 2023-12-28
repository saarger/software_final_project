import sys
import numpy as np
import random


# open file and read the vectors
def open_file(filename):
    vectors = []
    with open(filename, 'r') as f:
        for line in f:
            values = line.strip().split(',')
            vector = tuple([float(value) for value in values])
            vectors.append(vector)
    return vectors

# k-means algorithm
def k_means(k,iter,input_data):
    # check input
    try:
        if not 1<iter<1000:
            print("Invalid maximum iteration!")
            return
        epsilon = 0.001
        vectors = open_file(input_data)
        if not 1 < k < len(vectors):
            print("Invalid number of clusters!")
            return

        vectors = np.array(vectors)
        centroids,labels =  kmeansp(vectors,k,iter,epsilon)
        return centroids,labels
        

    except Exception:
        print("An Error Has Occurred")
        return


def kmeansp(X, k, max_iters=100, tolerance=1e-4):
    """
    K-Means clustering algorithm.
    
    Parameters:
    - X: Data points to cluster, shape (n_samples, n_features)
    - k: Number of clusters
    - max_iters: Maximum number of iterations
    - tol: Tolerance to declare convergence
    
    Returns:
    - centroids: Final centroid positions, shape (k, n_features)
    - labels: Label of each data point, shape (n_samples,)
    """
    # Randomly initialize centroids
    # Initialize centroids randomly
    centroids = []
    for i in range(k):
        centroids.append(X[i])
    centroids = np.array(centroids)

    # To store the value of centroids when it updates
    centroids_old = np.zeros(centroids.shape)

    # Cluster labels
    labels = np.zeros(len(X))

    # Error function - distance between new centroids and old centroids
    error = np.linalg.norm(centroids - centroids_old)

    # Loop until error is small enough or max iterations reached
    iterations = 0
    while error > tolerance and iterations < max_iters:
        # Assigning each value to its closest cluster
        for i in range(len(X)):
            distances = np.linalg.norm(X[i] - centroids, axis=1)
            labels[i] = np.argmin(distances)
        
        # Storing the old centroid values
        centroids_old = np.copy(centroids)
        
        # Finding the new centroids by taking the average value
        for i in range(k):
            points = [X[j] for j in range(len(X)) if labels[j] == i]
            centroids[i] = np.mean(points, axis=0)
        
        error = np.linalg.norm(centroids - centroids_old)
        iterations += 1
    
    return centroids, labels

# update centroids
def update_centroids(dic):
    delta_uk = 0
    new_centroids = []
    for key in dic.keys():
        sum = tuple([0]*len(dic.get(key)[0]))
        for vector in dic.get(key):
            sum = tuple([sum[i]+vector[i] for i in range(len(vector))])
        new_centroids.append(tuple([sum[i]/len(dic.get(key)) for i in range(len(sum))]))
        delta_uk = max(delta_uk,d(key,new_centroids[-1]))
    return new_centroids,delta_uk


# update dict
def update_dict(centroids):
    dic = {}
    for i in range(len(centroids)):
        dic[centroids[i]] = []
    return dic

# calculate distance
def d(p,q):
    sum = 0
    for i in range(len(p)):
        sum += (p[i]-q[i])**2
    return sum**0.5

# adapt to assumptions
def adapt_to_assumptions(centroids):
    new_centroids = []
    for centroid in centroids:
        new_centroids.append(tuple(['{:.4f}'.format(centroid[i]) for i in range(len(centroid))]))
    return new_centroids

def arg_parsing():


    try:
        args = sys.argv
        if len(args)>4 or len(args)<3:
            raise Exception
        k = int(args[1])
        if len(args) == 3:
            max_iter = 200
            idx_of_file = 2
        else:
            idx_of_file = 3
            max_iter = int(args[2])
        input_filename = args[idx_of_file]
        print(k_means(k, max_iter, input_filename))
    except Exception:
        print("An Error Has Occurred")




# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    arg_parsing()

