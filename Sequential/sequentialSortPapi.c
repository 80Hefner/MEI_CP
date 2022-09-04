// Bucket sort in C

#include <stdio.h>
#include <stdlib.h>
#include "papi.h"

#define NUM_EVENTS 4
int Events[NUM_EVENTS] = { PAPI_TOT_CYC, PAPI_TOT_INS, PAPI_L1_DCM, PAPI_L2_DCM};

long long values[NUM_EVENTS], min_values[NUM_EVENTS];

#define NUM_RUNS 5

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
  //  printf("Bucket[%d]: ", i);
  //  printBuckets(buckets[i]);
  //  printf("\n");
  //}

  // Sort the elements of each bucket
  for (i = 0; i < nBucket; ++i) {
    buckets[i] = InsertionSort(buckets[i]);
  }

  //printf("-------------\n");
  //printf("Bucktets after sorting\n");
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

  long long start_usec, end_usec, elapsed_usec, min_usec=0L;
  int num_hwcntrs = 0;
  int i,run;
  fprintf (stdout, "\nSetting up PAPI...");
  // Initialize PAPI
  PAPI_library_init (PAPI_VER_CURRENT);

  /* Get the number of hardware counters available */
  if ((num_hwcntrs = PAPI_num_counters()) <= PAPI_OK)  {
    fprintf (stderr, "PAPI error getting number of available hardware counters!\n");
    return 0;
  }
  fprintf(stdout, "done!\nThis system has %d available counters.\n\n", num_hwcntrs);

  // We will be using at most NUM_EVENTS counters
  if (num_hwcntrs >= NUM_EVENTS) {
    num_hwcntrs = NUM_EVENTS;
  } else {
    fprintf (stderr, "Error: there aren't enough counters to monitor %d events!\n", NUM_EVENTS);
    return 0;
  }


  for (run=0 ; run < NUM_RUNS ; run++) {
   fprintf (stderr, "\nrun=%d ", run);

   // use PAPI timer (usecs) - note that this is wall clock time
   // for process time running in user mode -> PAPI_get_virt_usec()
   // real and virtual clock cycles can also be read using the equivalent
   // PAPI_get[real|virt]_cyc()
   start_usec = PAPI_get_real_usec();

   /* Start counting events */
   if (PAPI_start_counters(Events, num_hwcntrs) != PAPI_OK) {
     fprintf (stderr, "PAPI error starting counters!\n");
     return 0;
   }
  const int nArray = atoi(argv[1]);
  const int nBucket = atoi(argv[2]);
  int *array_Mega = malloc(nArray*sizeof(int));;
  for (i = 0; i < nArray; i++) array_Mega[i] = rand() % nArray;

  //printf("Initial array: ");
  //print(array_Mega, nArray);
  //printf("-------------\n");

  BucketSort(array_Mega, nArray, nBucket);
  //printf("-------------\n");
  //printf("Sorted array: ");
  //print(array_Mega, nArray);

  /* Stop counting events */
   if (PAPI_stop_counters(values, NUM_EVENTS) != PAPI_OK) {
     fprintf (stderr, "PAPI error stoping counters!\n");
     return 0;
   }

   end_usec = PAPI_get_real_usec();
   fprintf (stderr, "done!\n");

   elapsed_usec = end_usec - start_usec;

   if ((run==0) || (elapsed_usec < min_usec)) {
      min_usec = elapsed_usec;
      for (i=0 ; i< NUM_EVENTS ; i++) min_values[i] = values [i];
   }

  } // end runs
  printf ("\nWall clock time: %lld usecs\n", min_usec);

  // output PAPI counters' values
  for (i=0 ; i< NUM_EVENTS ; i++) {
          char EventCodeStr[PAPI_MAX_STR_LEN];

          if (PAPI_event_code_to_name(Events[i], EventCodeStr) == PAPI_OK) {
                fprintf (stdout, "%s = %lld\n", EventCodeStr, min_values[i]);
          } else {
                fprintf (stdout, "PAPI UNKNOWN EVENT = %lld\n", min_values[i]);
          }
  }

#if NUM_EVENTS >1
  // evaluate CPI and Texec here
  if ((Events[0] == PAPI_TOT_CYC) && (Events[1] == PAPI_TOT_INS)) {
    float CPI = ((float) min_values[0]) / ((float) min_values[1]);
    fprintf (stdout, "CPI = %.2f\n", CPI);
  }
#endif

  printf ("\nThat's all, folks\n");
  return 1;

}
