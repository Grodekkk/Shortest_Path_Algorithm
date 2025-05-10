#include <iostream>

using namespace std;

#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define INFINITY -1

struct MetaInfo
{
    int width, height, startX, startY, destinationX, destinationY, skiLiftCount;
};

//every field on the map has info about: height, shortest path from start, was it visited by dikstja algortithm
struct HillField
{
    int hillHeight;
    int hillShortestPath;
    bool visited;
};

//==========================================================================================================================
//=============================   MIN-HEAP DATA STRUCTURE  =================================================================
//==========================================================================================================================

struct Node
{
    int shortest;
    int x;
    int y;
};

struct MinHeap
{
    Node** tree;
    int heapsize;
};

//create empty array representing binary tree, fill it with empty spaces
void initializeHeap(MinHeap* m, MetaInfo* data)
{
    //create new tree represented by array of N * M size
    int width = data->width;
    int height = data->height;
    int size = width * height * 2;
    //tworzymy tablice wskaznikow na struktury Node wielkosci M * N
    m->tree = new Node * [size];

    //fill the aray with "empty" data
    for (int i = 0; i < size; i++)
    {
        //tworzymy w środku nowy obiekt Node
        m->tree[i] = new Node;

        //przypisujemy mu wartosci
        m->tree[i]->shortest = -1;
        m->tree[i]->x = -1;
        m->tree[i]->y = -1;
    }

    m->heapsize = 0;
}

//====== DODAJ LICZBE DO HEAPU, UPORZADKUJ JA ODPOWIEDNIO=============
void insertHeap(MinHeap* m, int value, int x, int y)
{
    //dodajemy kolejny element do heapa, wiec jego zawartosc sie powieksza
    //dla pustego heapu zajmujemy tree[0]
    int i = m->heapsize;

    //przypisujemy "najkrotsza droge punktu, oraz gdzie nasz punkt sie znajduje
    m->tree[i]->shortest = value;
    m->tree[i]->x = x;
    m->tree[i]->y = y;

    //przykladowo zmiana rozmiaru z 0 na 1
    m->heapsize++;

    //dopoki nie dotrzemy do sa,ego szczytu drzewa (worst case scenario)
    while (i > 0)
    {
        //wzor na rodzica
        int parent = (i - 1) / 2;

        //jesli rodzic jest wiekszy -> zamiana
        if (m->tree[i]->shortest < m->tree[parent]->shortest)
        {
            //kopiujemy całą zawartosc node jak shortestPath, x, y
            Node* parrentData = m->tree[parent];

            m->tree[parent] = m->tree[i];
            m->tree[i] = parrentData;

            i = parent;
        }
        else
        {
            break;
        }
    }
}

//====== ZDEJMIJ LICZBE Z HEAPU ======================================
Node removeHeap(MinHeap* m)
{
    //krok 1 kopiujemy dane ze szczytu wierzcholka do zwyklej Node (nie wskaznika)
    Node result;
    result.shortest = m->tree[0]->shortest;
    result.x = m->tree[0]->x;
    result.y = m->tree[0]->y;



    //step 2 replace top and bottom of the heap
    m->tree[0]->shortest = m->tree[m->heapsize - 1]->shortest;
    m->tree[0]->x = m->tree[m->heapsize - 1]->x;
    m->tree[0]->y = m->tree[m->heapsize - 1]->y;


    //step 3 remove last node from tree, (clean values), zmniejsz rozmiar drzewka o 1
    m->tree[m->heapsize - 1]->shortest = -1;
    m->tree[m->heapsize - 1]->x = -1;
    m->tree[m->heapsize - 1]->y = -1;
    m->heapsize--;

    //step 4 compare topnode with children
    int i = 0;

    while (true)
    {
        int leftChild = (2 * i) + 1;
        int rightChild = (2 * i) + 2;

        bool leftSmaller = false;
        bool rightSmaller = false;

        //cleaning tutaj m.tree[i].shortest zamiast pisac zawsz to mozna zrobic pointery parentshortest childshortest itp nie wiadomo tylko czy bd dzialac

        //sprawdzamy czy lewe dziecko jest mniejsze, nie ma dziecka to nie jest mniejsze z automatu
        if (leftChild < m->heapsize && m->tree[i]->shortest > m->tree[leftChild]->shortest)
        {
            leftSmaller = true;
        }




        //sprawdzamy czy prawe jest mniejsze, , nie ma dziecka to nie jest mniejsze z automatu
        if (rightChild < m->heapsize && m->tree[i]->shortest > m->tree[rightChild]->shortest)
        {
            rightSmaller = true;
        }


        //zapisz wartosc rodzica do podmianki
        Node* parentData = m->tree[i];

        //opcja najgorsza -> obydwoje dzieci sa mniejsze zamieniamy z mniejszym
        if (rightSmaller && leftSmaller)
        {
            if (m->tree[leftChild]->shortest < m->tree[rightChild]->shortest)
            {
                m->tree[i] = m->tree[leftChild];
                m->tree[leftChild] = parentData;
                i = leftChild;
                continue;
            }
            else
            {
                m->tree[i] = m->tree[rightChild];
                m->tree[rightChild] = parentData;
                i = rightChild;
                continue;
            }
        }

        //opcja druga -> tylko jedno jest mniejsze
        if (rightSmaller || leftSmaller)
        {
            if (leftSmaller)
            {
                m->tree[i] = m->tree[leftChild];
                m->tree[leftChild] = parentData;
                i = leftChild;
                continue;
            }
            else
            {
                m->tree[i] = m->tree[rightChild];
                m->tree[rightChild] = parentData;
                i = rightChild;
                continue;
            }
        }

        //warunek konca
        if (rightSmaller == false && leftSmaller == false)
        {
            return result;
        }
    }
}

void deleteHeap(MinHeap* m, int totalSize)
{
    for (int i = 0; i < totalSize; i++)
    {
        delete m->tree[i];
    }
    delete[] m->tree;
}

//==========================================================================================================================
//=============================   BAZOWE DZIALANIE ALGORYTMU DLA JEDNEGO ZBIORU DANYCH =====================================
//==========================================================================================================================

//get "meta" information about the problem [lenght,height, destination, etc.]
void readMetaInfo(MetaInfo* data)
{
    cin >> data->width >> data->height >> data->startX >> data->startY >> data->destinationX >> data->destinationY >> data->skiLiftCount;
}

//create array of HillField data structures, fill it accordingly
HillField** createHillArray(int width, int height)
{
    HillField** map = new HillField * [height];  // alokacja wierszy

    for (int i = 0; i < height; i++)
    {
        map[i] = new HillField[width];        // alokacja kolumn dla każdego wiersza

        for (int j = 0; j < width; j++)
        {
            cin >> map[i][j].hillHeight;      // wczytaj wysokość pola
            map[i][j].hillShortestPath = -1;  // ustaw "nieskończony" koszt
            map[i][j].visited = false;        // jeszcze nieodwiedzone
        }
    }

    return map;
}

// Zwalnianie pamięci tablicy dynamicznej
void deleteHillArray(HillField** map, int length)
{
    for (int i = 0; i < length; i++)
        delete[] map[i];
    delete[] map;
}

//main function of Dijkstra algorithm
int runDijkstraAlgorithm(MetaInfo* data, HillField** map, MinHeap* m)
{
    //get starting "node" and destination node
    int currentX = data->startX;
    int currentY = data->startY;
    map[currentY][currentX].hillShortestPath = 0;
    map[currentY][currentX].visited = true;
    

    int destX = data->destinationX;
    int destY = data->destinationY;

    //iterate until you havent find the "shortest path
    while (true)
    {
        //check if our current node is our destination [function]
        if (currentX == destX && currentY == destY)
        {
            return map[currentY][currentX].hillShortestPath;
        }

        //========================== LEFT SIDE RELAXATION===========================================================
        if (currentX > 0)
        {
            if (map[currentY][currentX + LEFT].visited == false)
            {
                //=================== OBLICZANIE ODLEGLOSCI TYLKO POMIEDZY SASIADAMI==========================================
                int pathBetween = 0;
                //jesli pole po lewej od gorki jest wyzsze wtedy przejscie miedzy nimi zajmuje wyzsza-nizsza + 1
                if (map[currentY][currentX + LEFT].hillHeight > map[currentY][currentX].hillHeight)
                {
                    //calculate path between two nodes    
                    pathBetween = map[currentY][currentX + LEFT].hillHeight - map[currentY][currentX].hillHeight + 1;
                }
                else
                {
                    pathBetween = 1;
                }


                //==========================OBLICZANIE OBECNEJ NAJKROTSZEJ SCIEZKI==========================================================================
                int possibleShortestPath = pathBetween + map[currentY][currentX].hillShortestPath;
                if (map[currentY][currentX + LEFT].hillShortestPath == INFINITY || map[currentY][currentX + LEFT].hillShortestPath > possibleShortestPath)
                {
                    map[currentY][currentX + LEFT].hillShortestPath = possibleShortestPath;



                    int shortest = map[currentY][currentX + LEFT].hillShortestPath;
                    int x = currentX + LEFT;
                    int y = currentY;
                    insertHeap(m, shortest, x, y);
                    //cout << "dodano na do heapu droge do (" << x << "," << y << ") rowna: " << shortest << endl;
                }

                //========================= DODAWANIE POTENCJALNEJ NAJKROTSZEJ SCIEZKI DO PRIORITY LIST=====================================================
                

            }
        }

        //=========================== RIGHT SIDE RELAXATION =========================================================
        if (currentX < data->width - 1)
        {
            if (map[currentY][currentX + RIGHT].visited == false)
            {
                //=================== OBLICZANIE ODLEGLOSCI TYLKO POMIEDZY SASIADAMI==========================================
                int pathBetween = 0;
                //jesli pole po lewej od gorki jest wyzsze wtedy przejscie miedzy nimi zajmuje wyzsza-nizsza + 1
                if (map[currentY][currentX + RIGHT].hillHeight > map[currentY][currentX].hillHeight)
                {
                    //calculate path between two nodes    
                    pathBetween = map[currentY][currentX + RIGHT].hillHeight - map[currentY][currentX].hillHeight + 1;
                }
                else
                {
                    pathBetween = 1;
                }

                //==========================OBLICZANIE OBECNEJ NAJKROTSZEJ SCIEZKI==========================================================================
                int possibleShortestPath = pathBetween + map[currentY][currentX].hillShortestPath;
                if (map[currentY][currentX + RIGHT].hillShortestPath == INFINITY || map[currentY][currentX + RIGHT].hillShortestPath > possibleShortestPath)
                {
                    map[currentY][currentX + RIGHT].hillShortestPath = possibleShortestPath;


                    int shortest = map[currentY][currentX + RIGHT].hillShortestPath;
                    int x = currentX + RIGHT;
                    int y = currentY;
                    insertHeap(m, shortest, x, y);
                    //cout << "dodano na do heapu droge do (" << x << "," << y << ") rowna: " << shortest << endl;
                }

                //========================= DODAWANIE POTENCJALNEJ NAJKROTSZEJ SCIEZKI DO PRIORITY LIST=====================================================
                
            }
        }

        //=========================== TOP SIDE RELAXATION ===========================================================
        if (currentY > 0)
        {
            //=================== OBLICZANIE ODLEGLOSCI TYLKO POMIEDZY SASIADAMI==========================================
            if (map[currentY + UP][currentX].visited == false)
            {
                int pathBetween = 0;
                //jesli pole po lewej od gorki jest wyzsze wtedy przejscie miedzy nimi zajmuje wyzsza-nizsza + 1
                if (map[currentY + UP][currentX].hillHeight > map[currentY][currentX].hillHeight)
                {
                    //calculate path between two nodes    
                    pathBetween = map[currentY + UP][currentX].hillHeight - map[currentY][currentX].hillHeight + 1;
                }
                else
                {
                    pathBetween = 1;
                }


                //==========================OBLICZANIE OBECNEJ NAJKROTSZEJ SCIEZKI==========================================================================
                int possibleShortestPath = pathBetween + map[currentY][currentX].hillShortestPath;
                if (map[currentY + UP][currentX].hillShortestPath == INFINITY || map[currentY + UP][currentX].hillShortestPath > possibleShortestPath)
                {
                    map[currentY + UP][currentX].hillShortestPath = possibleShortestPath;


                    int shortest = map[currentY + UP][currentX].hillShortestPath;
                    int x = currentX;
                    int y = currentY + UP;
                    insertHeap(m, shortest, x, y);
                    //cout << "dodano na do heapu droge do (" << x << "," << y << ") rowna: " << shortest << endl;
                }


                //========================= DODAWANIE POTENCJALNEJ NAJKROTSZEJ SCIEZKI DO PRIORITY LIST=====================================================
                

            }
        }

        //=========================== BOTTOM SIDE RELAXATION ========================================================
        if (currentY < data->height - 1)
        {
            //=================== OBLICZANIE ODLEGLOSCI TYLKO POMIEDZY SASIADAMI==========================================
            if (map[currentY + DOWN][currentX].visited == false)
            {
                int pathBetween = 0;
                //jesli pole po lewej od gorki jest wyzsze wtedy przejscie miedzy nimi zajmuje wyzsza-nizsza + 1
                if (map[currentY + DOWN][currentX].hillHeight > map[currentY][currentX].hillHeight)
                {
                    //calculate path between two nodes    
                    pathBetween = map[currentY + DOWN][currentX].hillHeight - map[currentY][currentX].hillHeight + 1;
                }
                else
                {
                    pathBetween = 1;
                }

                //==========================OBLICZANIE OBECNEJ NAJKROTSZEJ SCIEZKI==========================================================================
                int possibleShortestPath = pathBetween + map[currentY][currentX].hillShortestPath;
                if (map[currentY + DOWN][currentX].hillShortestPath == INFINITY || map[currentY + DOWN][currentX].hillShortestPath > possibleShortestPath)
                {
                    map[currentY + DOWN][currentX].hillShortestPath = possibleShortestPath;


                    int shortest = map[currentY + DOWN][currentX].hillShortestPath;
                    int x = currentX;
                    int y = currentY + DOWN;
                    insertHeap(m, shortest, x, y);
                    //cout << "dodano na do heapu droge do (" << x << "," << y << ") rowna: " << shortest << endl;
                }


                //========================= DODAWANIE POTENCJALNEJ NAJKROTSZEJ SCIEZKI DO PRIORITY LIST=====================================================
               


            }
        }

        //=========================== CHANGE TO SUCCESOR ============================================================

        //tutaj zamiana
        Node temp;
       
        //if there is "double" in tree list, remove it but do not proceed
        while (true)
        {

            if (m->heapsize == 0)
                return -11; // brak możliwej ścieżki

            temp = removeHeap(m);
            //cout << "z heapu wyjeto: (" << temp.x << "," << temp.y << ") rowna: " << temp.shortest << endl;

            //  ZABEZPIECZENIE DODATKOWE:
            if (temp.x < 0 || temp.y < 0 || temp.x >= data->width || temp.y >= data->height)
                continue; // śmieciowy rekord – pomiń

            if (map[temp.y][temp.x].visited == false)
            {
                break;
            }
                
        }
        
        
        currentX = temp.x;
        currentY = temp.y;
        map[currentY][currentX].visited = true;
        //cout << "ruch na: (" << currentX << "," << currentY << ")" << endl;
    }
}

void printHeap(MinHeap* m)
{
    cout << "[DEBUG HEAP] ";
    for (int i = 0; i < m->heapsize; i++)
    {
        cout << m->tree[i]->shortest << " ";
    }
    cout << endl;
}


int main()
{
    /*
    MetaInfo dummyMeta;
    dummyMeta.width = 100;
    dummyMeta.height = 100;

    MinHeap heap;
    initializeHeap(&heap, &dummyMeta);

    cout << "=== DODAWANIE ===\n";
    
    insertHeap(&heap, 20, 0, 0);
    cout << "insertHeap, 20;\n";
    printHeap(&heap);

    insertHeap(&heap, 15, 1, 1);
    cout << "insertHeap(&heap, 15);\n";
    printHeap(&heap);
    insertHeap(&heap, 10, 2, 2);
    cout << "insertHeap(&heap, 10);\n";
    printHeap(&heap);
    
    cout << "\n=== USUWANIE ===\n";
        Node n = removeHeap(&heap);
        cout << "removeHeap() -> (" << n.x << "," << n.y << ") = " << n.shortest << endl;
        printHeap(&heap);

        n = removeHeap(&heap);
        cout << "removeHeap() -> (" << n.x << "," << n.y << ") = " << n.shortest << endl;
        printHeap(&heap);

        n = removeHeap(&heap);
        cout << "removeHeap() -> (" << n.x << "," << n.y << ") = " << n.shortest << endl;
        printHeap(&heap);
    
    

    return 0;
    */



    //=================================== ^^^^ DEBUGGING HEAP ^^^ ===========================================================
    //=======================================================================================================================
    //=================================== vvvv MAIN PROGRAM vvvvv ===========================================================



    
    MetaInfo data;
    MinHeap priorityList;

    readMetaInfo(&data);
    initializeHeap(&priorityList, &data);


    HillField** hillMap = createHillArray(data.width, data.height);

    // ...tutaj dalsza logika algorytmu Dijkstry
    int ShortestPath = runDijkstraAlgorithm(&data, hillMap, &priorityList);

    cout << ShortestPath;

    deleteHillArray(hillMap, data.height);
    

}
