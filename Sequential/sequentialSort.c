// Bucket sort in C

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Array size nArray
// Number of buckets nBucket
// Each bucket capacity intervalo

struct Node {
  int data;
  struct Node *next;
};

void BucketSort(int arr[], int nArray, int nBucket);
struct Node *InsertionSort(struct Node *list);
void print(int arr[], int nArray);
void printBuckets(struct Node *list);
int getBucketIndex(int value, int intervalo);

// Sorting function
void BucketSort(int arr[], int nArray, int nBucket) {
  int intervalo = nArray/nBucket;
  int i, j;
  struct Node **buckets;

  // Create buckets and allocate memory size
  buckets = (struct Node **)malloc(sizeof(struct Node *) * nBucket);

  // Initialize empty buckets
  for (i = 0; i < nBucket; ++i) {
    buckets[i] = NULL;
  }

  // Fill the buckets with respective elements
  for (i = 0; i < nArray; ++i) {
    struct Node *current;
    int pos = getBucketIndex(arr[i],intervalo);
    current = (struct Node *)malloc(sizeof(struct Node));
    current->data = arr[i];
    current->next = buckets[pos];
    buckets[pos] = current;
  }

  // Print the buckets along with their elements
  //for (i = 0; i < nBucket; i++) {
    //printf("Bucket[%d]: ", i);
    //printBuckets(buckets[i]);
    //printf("\n");
  //}

  // Sort the elements of each bucket
  for (i = 0; i < nBucket; ++i) {
    buckets[i] = InsertionSort(buckets[i]);
  }

  //printf("-------------\n");
  //printf("Buckets after sorting\n");
  //for (i = 0; i < nBucket; i++) {
    //printf("Bucket[%d]: ", i);
    //printBuckets(buckets[i]);
    //printf("\n");
  //}

  // Put sorted elements on arr
  for (j = 0, i = 0; i < nBucket; ++i) {
    struct Node *node;
    node = buckets[i];
    while (node) {
      arr[j++] = node->data;
      node = node->next;
    }
  }

  return;
}

// Function to sort the elements of each bucket
struct Node *InsertionSort(struct Node *list) {
  struct Node *k, *nodeList;
  if (list == 0 || list->next == 0) {
    return list;
  }

  nodeList = list;
  k = list->next;
  nodeList->next = 0;
  while (k != 0) {
    struct Node *ptr;
    if (nodeList->data > k->data) {
      struct Node *tmp;
      tmp = k;
      k = k->next;
      tmp->next = nodeList;
      nodeList = tmp;
      continue;
    }

    for (ptr = nodeList; ptr->next != 0; ptr = ptr->next) {
      if (ptr->next->data > k->data)
        break;
    }

    if (ptr->next != 0) {
      struct Node *tmp;
      tmp = k;
      k = k->next;
      tmp->next = ptr->next;
      ptr->next = tmp;
      continue;
    } else {
      ptr->next = k;
      k = k->next;
      ptr->next->next = 0;
      continue;
    }
  }
  return nodeList;
}

int getBucketIndex(int value, int intervalo) {
  return value / intervalo;
}

void print(int ar[], int nArray) {
  int i;
  for (i = 0; i < nArray; ++i) {
    printf("%d ", ar[i]);
  }
  printf("\n");
}

// Print buckets
void printBuckets(struct Node *list) {
  struct Node *cur = list;
  while (cur) {
    printf("%d ", cur->data);
    cur = cur->next;
  }
}

// Driver code
int main(int argc, char **argv) {

  int i;
  double start;
  double end;
  const int nArray = atoi(argv[1]);
  const int nBucket = atoi(argv[2]);
  start = omp_get_wtime();
  int *array_Mega = malloc(nArray*sizeof(int));;
  for (i = 0; i < nArray; i++) array_Mega[i] = rand() % nArray;

  //printf("Initial array: ");
  //print(array_Mega, nArray);
  //printf("-------------\n");

  BucketSort(array_Mega, nArray, nBucket);
  //printf("-------------\n");
  //printf("Sorted array: ");
  //print(array_Mega, nArray);
  end = omp_get_wtime();
  printf("It took %f seconds\n", end-start);
  return 0;

}
