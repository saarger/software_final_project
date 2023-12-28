import sys
import numpy as np
import symnmf
import pandas as pd
from sklearn.cluster import KMeans
from sklearn.decomposition import NMF
from sklearn.metrics import silhouette_score
import random
import math
import symnmfmodule
import kmeans


def read_data(file_name):
    data = []
    with open(file_name, 'r') as file:
        for line in file:
            point = list(map(float, line.strip().split(',')))
            data.append(point)
    return np.array(data)

def get_labels_from_H(H):
    """
    Derive hard clustering labels from association matrix H.

    Parameters:
        H (np.array): Association matrix of shape (n_samples, n_clusters).
        
    Returns:
        labels (np.array): Array of cluster labels of shape (n_samples,).
    """
    # Use np.argmax to find the index of the maximum value in each row.
    # That index is the cluster label for each data point.
    labels = np.argmax(H, axis=1)
    return labels

def perform_symnmf(file_name, k):
    H = symnmf.run_symnmf(k, "symnmf", file_name)
    labels = get_labels_from_H(H)
    return labels
    

def perform_kmeans(file_name, k):
    cenntroid,labels = kmeans.k_means(k, 200, file_name)
    return labels


def main():
    if len(sys.argv) != 3:
        print("Usage: python3 analysis.py <k> <file_name.txt>")
        sys.exit(1)
        
    k = int(sys.argv[1])
    file_name = sys.argv[2]
    
    symnmf_labels = perform_symnmf(file_name, k)
    kmeans_labels = perform_kmeans(file_name, k)


    vectors = read_data(file_name)
    symnmf_score = silhouette_score(vectors, symnmf_labels)
    kmeans_score = silhouette_score(vectors, kmeans_labels)
    
    print(f"nmf: {symnmf_score:.4f}")
    print(f"kmeans: {kmeans_score:.4f}")

if __name__ == "__main__":
    try:
        main()
    except Exception:
        print("An Error Has Occurred")    
