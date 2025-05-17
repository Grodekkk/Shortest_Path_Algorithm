#include <iostream>

using namespace std;

#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define INFINITY -1
#define NO_SHIFT 0

//single skilift
struct SkiLift
{
    int startX, startY;
    int endX, endY;
    int travelTime;
    int runsEvery;
};

//starting input about the test
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
    Node* tree;
    int heapsize;
};

//create empty array representing binary tree, fill it with empty spaces
void initializeHeap(MinHeap* m, MetaInfo* data)
{
    //create new tree represented by array of N * M size
    int width = data->width;
    int height = data->height;
    int size = width * height;                                      //this might be too small width * height * 2

    //make array of node structure
    m->tree = new Node[size];

    //fill the aray with "empty" data
    for (int i = 0; i < size; i++)
    {
        
        //create it's default values
        m->tree[i].shortest = -1;
        m->tree[i].x = -1;
        m->tree[i].y = -1;
    }

    m->heapsize = 0;
}


//====== INSERT NUMBER TO HEAP, SORT IT ACCORDINGLY =================
void insertHeap(MinHeap* m, int value, int x, int y)
{
    //inserting -> heap grows
    //for empty heap (size = 0) we take tree[0]
    int i = m->heapsize;

    //write in shortest path, and coordinates of given point
    m->tree[i].shortest = value;
    m->tree[i].x = x;
    m->tree[i].y = y;

    //increment heapsize
    m->heapsize++;

    //iterate until number will get on top of a heap (longest case)
    while (i > 0)
    {
        //parrent formula
        int parent = (i - 1) / 2;

        //if parrent is bigger -> switch
        if (m->tree[i].shortest < m->tree[parent].shortest)
        {
            //copy whole node as pointer
            Node parrentData = m->tree[parent];

            //switch pointers
            m->tree[parent] = m->tree[i];
            m->tree[i] = parrentData;

            i = parent;
        }
        //if parrent is smaller, number is in correct place, end loop
        else
        {
            break;
        }
    }
}

//====================   REMOVE NUMBER FROM HEAP ===================================
Node removeHeap(MinHeap* m)
{
    //step 1: deep copy of the top of a heap
    Node result;
    result.shortest = m->tree[0].shortest;
    result.x = m->tree[0].x;
    result.y = m->tree[0].y;


    //step 2: replace top and bottom of the heap
    m->tree[0].shortest = m->tree[m->heapsize - 1].shortest;
    m->tree[0].x = m->tree[m->heapsize - 1].x;
    m->tree[0].y = m->tree[m->heapsize - 1].y;


    //step 3: remove last node from tree, (clean values), decrement heapSize
    m->tree[m->heapsize - 1].shortest = -1;
    m->tree[m->heapsize - 1].x = -1;
    m->tree[m->heapsize - 1].y = -1;
    m->heapsize--;

    //step 4 compare topNode with children, until children are bigger
    int i = 0;

    while (true)
    {
        //child formulas
        int parrent = i;
        int leftChild = (2 * i) + 1;
        int rightChild = (2 * i) + 2;

        
        //values of shortest distances of "parrent" and "children"
        int currentNode = m->tree[i].shortest;
        int leftChildNode = m->tree[leftChild].shortest;
        int rightChildNode = m->tree[rightChild].shortest;

        int smallest = parrent;

        //check if left child is smaller than parrent, and if it exist
        if (leftChild < m->heapsize && m->tree[leftChild].shortest < m->tree[smallest].shortest)
        {
            smallest = leftChild;
        }

        //check if left child is smaller than parrent, and leftchild, and if it exist
        if (rightChild < m->heapsize && m->tree[rightChild].shortest < m->tree[smallest].shortest)
        {
            smallest = rightChild;
        }


        //save parrentData for switching
        Node parentData = m->tree[i];

        //end condition
        if (smallest == i)
        {
            return result;
        }
        
        m->tree[i] = m->tree[smallest];
        m->tree[smallest] = parentData;
        i = smallest;
       
 
    }
}

//====================   DELETE HEAP ===============================================
void deleteHeap(MinHeap* m)
{
    delete[] m->tree;   // wystarczy jedna operacja
}


//==========================================================================================================================
//=============================   BAZOWE DZIALANIE ALGORYTMU DLA JEDNEGO ZBIORU DANYCH =====================================
//==========================================================================================================================

//get "meta" information about the problem [lenght,height, destination, etc.]
void readMetaInfo(MetaInfo* data)
{
    cin >> data->width >> data->height >> data->startX >> data->startY >> data->destinationX >> data->destinationY >> data->skiLiftCount;
}

//read input about the Lifts 
SkiLift* readSkiLifts(int count)
{
    //allocating array of Skilift data structure
    SkiLift* liftList = new SkiLift[count];

    int counter = count;

    for (int i = 0; i < count; i++)
    {
        cin >> liftList[i].startX >> liftList[i].startY >> liftList[i].endX >> liftList[i].endY >> liftList[i].travelTime >> liftList[i].runsEvery;
    }

    return liftList;
}

//create array of HillField data structures, fill it accordingly
HillField** createHillArray(int width, int height)
{
    HillField** map = new HillField * [height];

    for (int i = 0; i < height; i++)
    {
        map[i] = new HillField[width];        

        for (int j = 0; j < width; j++)
        {
            //fill values in every field
            cin >> map[i][j].hillHeight;      
            map[i][j].hillShortestPath = -1;  
            map[i][j].visited = false;        
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

//relaxation of all four neighbours [helper]
void relaxNeighbourHelper(int shiftX, int shiftY, int currentX, int currentY, HillField** map, MinHeap* m)
{
    //if neighbour is visited, skip
    if (map[currentY + shiftY][currentX + shiftX].visited == true)
    {
        return;
    }
    

    //compute distance between neighbours
    int pathBetween = 0;
    int neighbourHeight = map[currentY + shiftY][currentX + shiftX].hillHeight;
    int currentHeight = map[currentY][currentX].hillHeight;

    //jesli pole po lewej od gorki jest wyzsze wtedy przejscie miedzy nimi zajmuje wyzsza-nizsza + 1
    if (neighbourHeight > currentHeight)
    {
        //calculate path between two nodes    
        pathBetween = neighbourHeight - currentHeight + 1;
    }
    else
    {
        pathBetween = 1;
    }


    //check if new shortest path is possible
    int possibleShortestPath = pathBetween + map[currentY][currentX].hillShortestPath;
    int neighbourShortest = map[currentY + shiftY][currentX + shiftX].hillShortestPath;

    if (neighbourShortest == INFINITY || neighbourShortest > possibleShortestPath)
    {
        map[currentY + shiftY][currentX + shiftX].hillShortestPath = possibleShortestPath;

        //if shortest path is updated, add it to "priority queue"
        int shortest = possibleShortestPath;
        int x = currentX + shiftX;
        int y = currentY + shiftY;
        insertHeap(m, shortest, x, y);
    }
}

void relaxNeighboursMain(int currentX, int currentY, HillField** map, MinHeap* m, MetaInfo* data)
{
    //left side
    if (currentX > 0)
    {
        relaxNeighbourHelper(LEFT, NO_SHIFT, currentX, currentY, map, m);
    }

    //right side
    if (currentX < data->width - 1)
    {
        relaxNeighbourHelper(RIGHT, NO_SHIFT, currentX, currentY, map, m);
    }

    //top side
    if (currentY > 0)
    {
        relaxNeighbourHelper(NO_SHIFT, UP, currentX, currentY, map, m);
    }

    //bottom side
    if (currentY < data->height - 1)
    {
        relaxNeighbourHelper(NO_SHIFT, DOWN, currentX, currentY, map, m);
    }
}

//relaxation helper function for Lifts
void liftListRelaxation(MetaInfo* data, HillField** map, MinHeap* m, SkiLift* liftList, int currentX, int currentY)
{
    for (int i = 0; i < data->skiLiftCount; i++)
    {
        //if current coordinate doesn't have list, skip iteration
        if (!(currentX == liftList[i].startX && currentY == liftList[i].startY))
        {
            continue;
        }

        //if endpoint of a lift is marked as visited, skip iteration
        if (map[liftList[i].endY][liftList[i].endX].visited == true)
        {
            continue;
        }

        //check how many minutes do we have to wait for the lift
        int moduloTimeLeft = map[currentY][currentX].hillShortestPath % liftList[i].runsEvery;
        int waitingTime = 0;
        if (moduloTimeLeft != 0)
        {
            waitingTime = liftList[i].runsEvery - moduloTimeLeft;
        }

        //"relax" the destination (if possible)
        int currentShortest = map[liftList[i].endY][liftList[i].endX].hillShortestPath;
        int possibleShortest = map[currentY][currentX].hillShortestPath + waitingTime + liftList[i].travelTime;

        if (currentShortest == INFINITY || currentShortest > possibleShortest)
        {
            map[liftList[i].endY][liftList[i].endX].hillShortestPath = possibleShortest;

            //ZAROWNO tu jak i w innych mozna chyba day po prostu ten "destinationpath"
            int shortest = possibleShortest;
            int x = liftList[i].endX;
            int y = liftList[i].endY;
            insertHeap(m, shortest, x, y);
        }
    }
}

//main function of Dijkstra algorithm
int runDijkstraAlgorithm(MetaInfo* data, HillField** map, MinHeap* m, SkiLift* liftList)
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
        //end condition: reached destination
        if (currentX == destX && currentY == destY)
        {
            return map[currentY][currentX].hillShortestPath;
        }

        //if there are lifts, check them
        if (liftList != nullptr)
        {
            liftListRelaxation(data, map, m, liftList, currentX, currentY);
        }

        //relaxation of neighbours
        relaxNeighboursMain(currentX, currentY, map, m, data);


        // node for minHeap values
        Node temp;
       
        //if there is "double" in tree list, remove it but do not proceed
        while (true)
        {
            //if there is no shortest path [possible delete]
            if (m->heapsize == 0)
            {
                return -11;
            }
                
            //get node from top of a heap
            temp = removeHeap(m);
            

            // if node has wrong values? [maybe also deletable][ [cleaning[
            if (temp.x < 0 || temp.y < 0 || temp.x >= data->width || temp.y >= data->height)
                continue; // śmieciowy rekord – pomiń

            //dodane test 15 [cos o powtorkach]
            if (map[temp.y][temp.x].hillShortestPath < temp.shortest)
                continue;

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



int main()
{
    //dodane test 15, podobno przyspiesza dzialanie cina
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    //initialise datastructures
    MetaInfo data;
    MinHeap priorityList;
    SkiLift* liftList = nullptr;

    //read test info from input
    readMetaInfo(&data);
    
    //add SkiLifts if data.skiLiftCount > 0
    if (data.skiLiftCount > 0)
    {
        liftList = readSkiLifts(data.skiLiftCount);
    }
    
    //initialize minheap
    initializeHeap(&priorityList, &data);

    //initialize map
    HillField** hillMap = createHillArray(data.width, data.height);

    //get shortest path from Dijkstra alghoritm
    int ShortestPath = runDijkstraAlgorithm(&data, hillMap, &priorityList, liftList);

    //print shortest path
    cout << ShortestPath;

    deleteHillArray(hillMap, data.height);
    
    
}


/*
//========================================================================
//========= DEBUGGING CODE FRAGMENTS =====================================
//========================================================================

//========= MAIN FOR CHECKING MINHEAP ====================================
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
    cout << "insertHeap, 15;\n";
    printHeap(&heap);

    insertHeap(&heap, 10, 2, 2);
    cout << "insertHeap, 10;\n";
    printHeap(&heap);

    // DODATKOWE 5 WSTAWIEŃ
    //insertHeap(&heap, 30, 3, 3);
    //cout << "insertHeap, 30;\n";
   // printHeap(&heap);

    insertHeap(&heap, 5, 4, 4);
    cout << "insertHeap, 5;\n";
    printHeap(&heap);

    insertHeap(&heap, 12, 5, 5);
    cout << "insertHeap, 12;\n";
    printHeap(&heap);

    insertHeap(&heap, 17, 6, 6);
    cout << "insertHeap, 17;\n";
    printHeap(&heap);

    insertHeap(&heap, 8, 7, 7);
    cout << "insertHeap, 8;\n";
    printHeap(&heap);


    cout << "\n=== USUWANIE ===\n";
    Node n;

    for (int i = 0; i < 7; ++i)
    {
        n = removeHeap(&heap);
        cout << "removeHeap() -> (" << n.x << "," << n.y << ") = " << n.shortest << endl;
        printHeap(&heap);
    }
    */

//cout added to relaxation
// 
// //cout << "dodano na do heapu droge do (" << x << "," << y << ") rowna: " << shortest << endl;
// 
// przy remove heap w main dijkstra
// //cout << "z heapu wyjeto: (" << temp.x << "," << temp.y << ") rowna: " << temp.shortest << endl;
//========= FUNCTION PRINTING HEAP CONTENTS ==============================
/*
void printHeap(MinHeap* m)
{
    cout << "[DEBUG HEAP] ";
    for (int i = 0; i < m->heapsize; i++)
    {
        cout << m->tree[i]->shortest << " ";
    }
    cout << endl;
}
*/

//===================== OLD RELAXATION
/*
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
*/
