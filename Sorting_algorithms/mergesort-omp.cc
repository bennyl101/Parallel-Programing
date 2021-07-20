/**
 *  \file mergesort.cc
 *
 *  \brief Implement your mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "sort.hh"

void
mySort (int N, keytype* A)
{
  if(N < 2){
	  return;
		}
		else{
			int x = N%2;
			if(x == 0){/*check to see if N is even or odd*/
				int y = N/2;
				keytype* B = newKeys(y);
				keytype* C = newKeys(y);
				for(int j = 0; j<y; j++){/*Create two arrays of two halves of A array*/
					B[j] = A[j];
					C[j] = A[j+y];
				}
				#pragma omp parallel sections
				{
					#pragma omp section
					{
						mySort(y , B);
					}
					#pragma omp section
					{
						mySort(y , C);
					}
				}

				A = pmerge(B, C);
			}
			else{
				int y = N/2;
				int z = y + 1;
				keytype* B = newKeys(y);
				keytype* C = newKeys(z);
				for(int j = 0 ; j < y; j++){	
					B[j] = A[j];
					C[j] = A[j+y];
				}
				C[y] = A[2*y]; 
				#pragma omp parallel sections
				{
					#pragma omp section
					{
						mySort(y, B);
					}
					#pragma omp section
					{
						mySort(z, C);
					}
				}
				A = pmerge(B, C);
			}	
		}
}

keytype* pmerge(keytype* B, keytype* C){
	int keyB = 0;
	int keyC = 0;
	int index = 0;
	
	int index_split = 0;
	int length_B = sizeof(B)/sizeof(B[0]);
	int length_C = sizeof(C)/sizeof(C[0]);
	int mid = 0;
	
				#pragma omp parallel sections
				{
					#pragma omp section
					{
						simple_sort(length_B, B);
					}
					#pragma omp section
					{
						simple_sort(length_C, C);
		}
	}

	if(length_B == 1 || length_C == 1){

		keytype* A = newKeys(length_C + length_B);
		while(keyB < length_B && keyC < length_C){
			if(B[keyB] >= C[keyC]){
				A[index] = C[keyC];
				keyC++;
				index++;
			}else{
				A[index] = B[keyB];
				keyB++;
				index++;
				}	
			}
		while(keyB < length_B){
			A[index] = B[keyB];
			index++;
			keyB++;			
		}
		while(keyC < length_C){
			A[index] = C[keyC];
			index++;
			keyC++;
		}
		return A;
	}

		keytype* B_1 = newKeys(length_B/2);
		keytype* B_2 = newKeys(length_B/2);
		keytype* C_1 = newKeys(length_C/2);
		keytype* C_2 = newKeys(length_C/2);
		
		keytype* D = newKeys(length_B + length_C);
		keytype* E = newKeys(length_B + length_C);
		
	if(length_B >= length_C){

		
		mid = length_B/2;
		index_split = B_search(C, C[mid], 0, length_C - 1); //C array is being split
		if(length_B%2 == 0){
			B_1 = newKeys(mid);
			B_2 = newKeys(mid);
			for(int j = 0; j<mid; j++){/*Create two arrays of two halves of A array*/
				B_1[j] = B[j];
				B_2[j] = B[j+mid];	
			}
			C_1 = newKeys(index_split);
			C_2 = newKeys(length_C - index_split);
		
			for(int i = 0; i < index_split;i++){
				C_1[i] = C[i];
			}
			for(int k = 0; k < (length_C - index_split); k++){
				C_2[k] = C[index_split + k];
			}
	
		}else{
			B_1 == newKeys(mid);
			B_2 == newKeys(mid + 1);
			for(int j = 0 ; j < mid; j++){	
				B_1[j] = B[j];
				B_2[j] = B[j+mid];
			}
			B_2[mid] = B[2*mid]; 
			
			C_1 = newKeys(index_split);
			C_2 = newKeys(length_C - index_split);
		
			for(int i = 0; i < index_split;i++){
				C_1[i] = C[i];
			}
			for(int k = 0; k < (length_C - index_split); k++){
				C_2[k] = C[index_split + k];
			}
		}
	
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				D = pmerge(B_1, C_1);
			}
			#pragma omp section
			{
				E = pmerge(B_2, C_2);
			}
		}
		int size_D = sizeof(D)/sizeof(D[0]);
		int size_E = sizeof(E)/sizeof(E[0]);
			
		keytype* A = newKeys(size_D + size_E);
			
		for(int z = 0; z < size_D; z++){
			A[z] = D[z];
		}
		for(int y = 0; y < size_E; y++){
			A[size_D + y] = E[size_D + y];
		}
		return A;
	}else{
		mid = length_C/2;
		index_split = B_search(B, B[mid], 0, length_B - 1); //B array is being split
		if(length_C%2 == 0){
			C_1 = newKeys(mid);
			C_2 = newKeys(mid);
			for(int j = 0; j<mid; j++){/*Create two arrays of two halves of A array*/
				C_1[j] = C[j];
				C_2[j] = C[j+mid];	
			}
		}else{
			C_1 == newKeys(mid);
			C_2 == newKeys(mid + 1);
			for(int j = 0 ; j < mid; j++){	
				C_1[j] = C[j];
				C_2[j] = C[j+mid];
			}
			C_2[mid] = C[2*mid]; 
		}
		
		B_1 = newKeys(index_split);
		B_2 = newKeys(length_B - index_split);
		
		for(int i = 0; i < index_split;i++){
			B_1[i] = B[i];
		}
		for(int k = 0; k < (length_B - index_split); k++){
			B_2[k] = B[index_split + k];
		}
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				D = pmerge(B_1, C_1);
			}
			#pragma omp section
			{
				E = pmerge(B_2, C_2);
			}
		}
		int size_D = sizeof(D)/sizeof(D[0]);
		int size_E = sizeof(E)/sizeof(E[0]);
			
		keytype* A = newKeys(size_D + size_E);
			
		for(int z = 0; z < size_D; z++){
			A[z] = D[z];
		}
		for(int y = 0; y < size_E; y++){
			A[size_D + y] = E[size_D + y];
		}
		return A;
	}

}	

int B_search(keytype* A, int e, int low, int high){//int e is key to find index of
	if(high<=low)
		return(e>A[low])?(low+1):low;
	int mid = (low+high)/2;
	if(e==A[mid])
		return mid;
	if(e>A[mid])
		return B_search(A, e, mid+1, high);
	return B_search(A,e,low, mid - 1);
}

void simple_sort(int N, keytype* A){
	for(int j = 2; j <N; j++){
		int key = A[j];
		int i = j-1;
		int x = B_search(A, key, 1, i);
			while(i>=x){
					A[i+1] = A[i];
					i--;
			}
			A[i+1] = key;
	}
}
/* eof */
