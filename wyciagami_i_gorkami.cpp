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
        map[i] = new HillField[width];        // alokacja kolumn dla ka¿dego wiersza

        for (int j = 0; j < width; j++)
        {
            cin >> map[i][j].hillHeight;      // wczytaj wysokoœæ pola
            map[i][j].hillShortestPath = -1;  // ustaw "nieskoñczony" koszt
            map[i][j].visited = false;        // jeszcze nieodwiedzone
        }
    }

    return map;
}

// Zwalnianie pamiêci tablicy dynamicznej
void deleteHillArray(HillField** map, int length)
{
    for (int i = 0; i < length; i++)
        delete[] map[i];
    delete[] map;
}

//main function of Dijkstra algorithm
int runDijkstraAlgorithm(MetaInfo* data, HillField** map)
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

        int successorX = -1;
        int succesorY = -1;
        int succesorShortest = -1;
     //do the relaxation (update shortest path) for all 4 destinations
     // najpierw obliczamy roznice miedzy gorami (sasiednia droga)
     // nastepnie dodajemy poprzednia najkrotsza droge
     // totalnie da sie z tego zrobic o wiele bardziej przejrzysta funkcje
     //this can be later turned into "compass" [0,0,1,1]
     //                                        [1,0,1,0]  <- all four directions
     //                                        tutaj przydalaby sie osobna funkcja do exception handlingu

        //========================== LEFT SIDE RELAXATION===========================================================
        //relaksacja z lewej strony w lewo [zawsze gdy x>0]
        if (currentX > 0)
        {
            if (map[currentY][currentX + LEFT].visited == false)
            {
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

                int possibleShortestPath = pathBetween + map[currentY][currentX].hillShortestPath;
                if (map[currentY][currentX + LEFT].hillShortestPath == INFINITY || map[currentY][currentX + LEFT].hillShortestPath >= possibleShortestPath)
                {
                    map[currentY][currentX + LEFT].hillShortestPath = possibleShortestPath;
                }

                //checking if this is new shortest node -> next step in dikstrja algorithm
                if (succesorShortest == -1 || map[currentY][currentX + LEFT].hillShortestPath <= succesorShortest)
                {
                    successorX = currentX + LEFT;
                    succesorY = currentY;
                    succesorShortest = map[currentY][currentX + LEFT].hillShortestPath;
                }
            }
        }

        //=========================== RIGHT SIDE RELAXATION =========================================================
        if (currentX < data->width - 1)
        {
            if (map[currentY][currentX + RIGHT].visited == false)
            {
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

                int possibleShortestPath = pathBetween + map[currentY][currentX].hillShortestPath;
                if (map[currentY][currentX + RIGHT].hillShortestPath == INFINITY || map[currentY][currentX + RIGHT].hillShortestPath >= possibleShortestPath)
                {
                    map[currentY][currentX + RIGHT].hillShortestPath = possibleShortestPath;
                }

                //checking if this is new shortest node -> next step in dikstrja algorithm
                if (succesorShortest == -1 || map[currentY][currentX + RIGHT].hillShortestPath <= succesorShortest)
                {
                    successorX = currentX + RIGHT;
                    succesorY = currentY;
                    succesorShortest = map[currentY][currentX + RIGHT].hillShortestPath;
                }
            }
        }

        //=========================== TOP SIDE RELAXATION ===========================================================
        if (currentY > 0)
        {
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

                int possibleShortestPath = pathBetween + map[currentY][currentX].hillShortestPath;
                if (map[currentY + UP][currentX].hillShortestPath == INFINITY || map[currentY + UP][currentX].hillShortestPath >= possibleShortestPath)
                {
                    map[currentY + UP][currentX].hillShortestPath = possibleShortestPath;
                }

                //checking if this is new shortest node -> next step in dikstrja algorithm
                if (succesorShortest == -1 || map[currentY + UP][currentX].hillShortestPath <= succesorShortest)
                {
                    successorX = currentX;
                    succesorY = currentY + UP;
                    succesorShortest = map[currentY + UP][currentX].hillShortestPath;
                }
            }
        }

        //=========================== BOTTOM SIDE RELAXATION ========================================================
        if (currentY < data->height - 1)
        {
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

                int possibleShortestPath = pathBetween + map[currentY][currentX].hillShortestPath;
                if (map[currentY + DOWN][currentX].hillShortestPath == INFINITY || map[currentY + DOWN][currentX].hillShortestPath >= possibleShortestPath)
                {
                    map[currentY + DOWN][currentX].hillShortestPath = possibleShortestPath;
                }

                //checking if this is new shortest node -> next step in dikstrja algorithm
                if (succesorShortest == -1 || map[currentY + DOWN][currentX].hillShortestPath <= succesorShortest)
                {
                    successorX = currentX;
                    succesorY = currentY + DOWN;
                    succesorShortest = map[currentY + DOWN][currentX].hillShortestPath;
                }
            }
        }

        //=========================== CHANGE TO SUCCESOR ============================================================
        currentX = successorX;
        currentY = succesorY;
        map[currentY][currentX].visited = true;
    }
}

int main()
{
    MetaInfo data;

    readMetaInfo(&data);

    HillField** hillMap = createHillArray(data.width, data.height);

    // ...tutaj dalsza logika algorytmu Dijkstry
    int ShortestPath = runDijkstraAlgorithm(&data, hillMap);

    cout << ShortestPath;

    deleteHillArray(hillMap, data.height);
    
}
