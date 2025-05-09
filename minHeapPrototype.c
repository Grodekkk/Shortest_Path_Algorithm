// MINHEAP TESTS
//to do: jak to jest z usuwaniem i zamiana jak np rodzic ma 10 a dzieci tez 10 albo rodzic ma 10 a oboje dzieci po 5
#include <iostream>

using namespace std;

//====== DODAJ LICZBE DO HEAPU, UPORZADKUJ JA ODPOWIEDNIO=============
void insertHeap(int minHeap[], int* heapSize, int value)
{
	//dodajemy kolejny element do heapa, wiec jego zawartosc sie powieksza
	int i = *heapSize;
	minHeap[i] = value;
	(*heapSize)++;

	//dopoki nie dotrzemy do sa,ego szczytu drzewa (worst case scenario)
	while (i > 0)
	{
		//wzor na rodzica
		int parent = (i - 1) / 2;

		//jesli rodzic jest wiekszy -> zamiana
		if (minHeap[i] < minHeap[parent])
		{
			int parrentData = minHeap[parent];
			minHeap[parent] = minHeap[i];
			minHeap[i] = parrentData;

			i = parent;
		}
		else
		{
			break;
		}
	}
}

//====== ZDEJMIJ LICZBE Z HEAPU ======================================
int removeHeap(int minHeap[], int *heapSize)
{
	//step 1: save top of the heap to the variable
	int topHeap = minHeap[0];

	//step 2 replace top and bottom of the heap
	minHeap[0] = minHeap[*heapSize - 1];

	//step 3 remove last node from tree
	minHeap[*heapSize - 1] = -1;
	(*heapSize)--;

	//step 4 compare topnode with children
	int i = 0;

	while (true)
	{
		int leftChild = (2 * i) + 1;
		int rightChild = (2 * i) + 2;

		bool leftSmaller = false;
		bool rightSmaller = false;

		//sprawdzamy czy lewe dziecko jest mniejsze, nie ma dziecka to nie jest mniejsze z automatu
		if (leftChild < *heapSize && minHeap[i] > minHeap[leftChild])
		{
			leftSmaller = true;
		}
			

		

		//sprawdzamy czy prawe jest mniejsze, , nie ma dziecka to nie jest mniejsze z automatu
		if (rightChild < *heapSize && minHeap[i] > minHeap[rightChild])
		{
			rightSmaller = true;
		}
			

		//zapisz wartosc rodzica do podmianki
		int parentData = minHeap[i];

		//opcja najgorsza -> obydwoje dzieci sa mniejsze zamieniamy z mniejszym
		if (rightSmaller && leftSmaller)
		{
			if (minHeap[leftChild] < minHeap[rightChild])
			{
				minHeap[i] = minHeap[leftChild];
				minHeap[leftChild] = parentData;
				i = leftChild;
				continue;
			}
			else
			{
				minHeap[i] = minHeap[rightChild];
				minHeap[rightChild] = parentData;
				i = rightChild;
				continue;
			}
		}

		//opcja druga -> tylko jedno jest mniejsze
		if (rightSmaller || leftSmaller)
		{
			if (leftSmaller)
			{
				minHeap[i] = minHeap[leftChild];
				minHeap[leftChild] = parentData;
				i = leftChild;
				continue;
			}
			else
			{
				minHeap[i] = minHeap[rightChild];
				minHeap[rightChild] = parentData;
				i = rightChild;
				continue;
			}
		}

		//warunek konca
		if (rightSmaller == false && leftSmaller == false)
		{
			return topHeap;
		}
	}
}

int main()
{
	//======= KROK 1 TWORZYMY TABLICE, ZAPELNIAMY JA -1 (PUSTA) I USTAWIAMY WIELKOSC HEAPU
	int minHeap[100]; //poki co 100

	for (int i = 0; i < 100; i++)
	{
		minHeap[i] = -1;
	}

	int heapSize = 0;

	//[-1,-1,-1,...,-1]
	//  0  1  2 ... 99

	//KROK 2 -> DODAJEMY UZYWAJAC FUNKCJI HEAPIFY[?] RODZIC -> i>0 -> (i-1)/2


}
