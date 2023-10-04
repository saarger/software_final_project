import sys
import numpy as np
from sklearn.cluster import KMeans
from sklearn.decomposition import NMF
from sklearn.metrics import silhouette_score

def read_data(file_name):
    data = []
    with open(file_name, 'r') as file:
        for line in file:
            point = list(map(float, line.strip().split(',')))
            data.append(point)
    return np.array(data)

def perform_symnmf(data, k):
    # call to 1.4
    pass

def perform_kmeans(data, k):
    # call to kmean from HW1
    pass

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 analysis.py <k> <file_name.txt>")
        sys.exit(1)
        
    k = int(sys.argv[1])
    file_name = sys.argv[2]
    
    data = read_data(file_name)
    
    symnmf_labels = perform_symnmf(data, k)
    kmeans_labels = perform_kmeans(data, k)
    
    symnmf_score = silhouette_score(data, symnmf_labels)
    kmeans_score = silhouette_score(data, kmeans_labels)
    
    print(f"nmf: {symnmf_score:.4f}")
    print(f"kmeans: {kmeans_score:.4f}")

if __name__ == "__main__":
    main()
