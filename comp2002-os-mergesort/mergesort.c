/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"


/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
	memcpy(B + leftstart, A + leftstart, (rightend - leftstart +1) * sizeof(int));

	int i = leftstart;
	int j = rightstart;
	int k = leftstart;


	while (i <= leftend && j <= rightend) {
		if (B[i] <= B[j]) {
			A[k++] = B[i++];
		} else {
			A[k++] = B[j++];
		}
    }

	while (i <= leftend) {
		A[k++] = B[i++];
	}

	while (j <= rightend) {
		A[k++] = B[j++];
	}
}

/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right){
	if (left >= right) {
		return;
	}

	int mid = left + (right - left) / 2;

	my_mergesort(left, mid);
	my_mergesort(mid + 1, right);
	merge(left, mid, mid+1, right);
}

/* this function will be called by the testing program. */
void * parallel_mergesort(void *arg){
	struct argument *a = (struct argument *) arg;

    if (a->level >= cutoff) {
		my_mergesort(a->left, a->right);
		return NULL;
	}

	int mid = a->left + (a->right - a->left) / 2;

	struct argument *lefArg = buildArgs(a->left, mid, a->level + 1);
	struct argument *rightArg = buildArgs(mid + 1, a->right, a->level + 1);

	pthread_t tid1, tid2;

	pthread_create(&tid1, NULL, parallel_mergesort, lefArg);
	pthread_create(&tid2, NULL, parallel_mergesort, rightArg);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	merge(a->left, mid, mid + 1, a->right);

    return NULL;
}

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level){
	struct argument *arg = (struct argument *) malloc(sizeof(struct argument));
    arg->left = left;
    arg->right = right;
    arg->level = level;
    return arg;
}

