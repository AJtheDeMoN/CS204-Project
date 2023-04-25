#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <climits>
#include <cstdlib>

using namespace std;
struct cacheBlock
{
    unsigned tag;
    bool valid;
    bool dirty;
    int accessCount;
    unsigned data;
    cacheBlock(): tag(-1), valid(false), dirty(false), accessCount(0), data(0){}
};
class Cache
{
private:
    int cacheSize,blockSize,numSets,numWays,numBlocks;
    string cacheType, replacementPolicy;
    int hitTime, missPenalty, accesses, hits, misses, coldMisses, conflictMisses, capacityMisses, memoryStalls;
    cacheBlock *cacheArray;

public:
    // Constructor
    Cache(int cs, int bs, string ct, string rp, int ht, int mp, int nw) : cacheSize(cs), blockSize(bs), numBlocks(cs / bs), cacheType(ct), replacementPolicy(rp), hitTime(ht), missPenalty(mp), numWays(nw), accesses(0), hits(0), misses(0), coldMisses(0), conflictMisses(0), capacityMisses(0), memoryStalls(0)
    {
        if (cacheType == "direct_mapped")
        {
            numSets = numBlocks;
        }
        else if (cacheType == "fully_assoc")
        {
            numSets = 1;
            numWays = numBlocks;
        }
        else if (cacheType == "set_associtive")
        {
            numSets = numBlocks / nw;
        }
        cacheArray = new cacheBlock[numBlocks];
    }
    // Destructor
    ~Cache()
    {
        delete[] cacheArray;
    }
    // Access function
    int lookup(int address)
    {
        int index = ((address / blockSize) % numSets);
        int tag = address / ((blockSize * numSets));
        int startIndex = index * numWays;
        int endIndex = startIndex + numWays - 1;
        int way = -1;
        int hitIndex = -1;

        // check if hit
        for (int i = startIndex; i <= endIndex; i++)
        {
            if ((cacheArray[i].tag & tag) && cacheArray[i].valid)
            {
                hits++;
                hitIndex = i;
                break;
            }
        }
        // if miss
        if (hitIndex == -1)
        {
            misses++;
            if (replacementPolicy == "LRU")
            {
                int target_address=(address/blockSize)*blockSize;
                updateLRU(startIndex, endIndex, target_address);
            }
            else if (replacementPolicy == "LFU")
            {
                updateLFU(hitIndex, startIndex, endIndex);
            }
            return cacheArray[hitIndex].data;
        }
        else
        {
            int emptyIndex = -1;
            for (int i = startIndex; i <= endIndex; i++)
            {
                if (cacheArray[i].valid == false)
                {
                    emptyIndex = i;
                    break;
                }
            }
            if (emptyIndex != -1)
            {
                cacheArray[emptyIndex].tag = tag;
                cacheArray[emptyIndex].valid = true;
                cacheArray[emptyIndex].data = address;
                cacheArray[emptyIndex].dirty = false;
            }
            else
            {
                // Replace block using replacement policy
                int replaceIndex = -1;
                if (replacementPolicy == "LRU")
                {
                    replaceIndex = findLRU(startIndex, endIndex);
                }
                else if (replacementPolicy == "FIFO")
                {
                    replaceIndex = findFIFO(startIndex, endIndex);
                }
                else if (replacementPolicy == "Random")
                {
                    replaceIndex = findRandom(startIndex, endIndex);
                }
                else if (replacementPolicy == "LFU")
                {
                    replaceIndex = findLFU(startIndex, endIndex);
                }

                // check if block is dirty
                if (cacheArray[replaceIndex].dirty == true)
                {
                    memoryStalls += missPenalty;
                }
                cacheArray[replaceIndex].tag = tag;
                cacheArray[replaceIndex].valid = true;
                cacheArray[replaceIndex].data = address;
                cacheArray[replaceIndex].dirty = false;
            }
        }
        return address;
    }
    // update LRU function
    void updateLRU(int startIndex, int endIndex, int address)
    {
        int target = -1;
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (!cacheArray[i].valid || !cacheArray[i].accessCount)
            {
                cacheArray[i].accessCount = numWays - 1;
                target = i;
                break;
            }
        }
        // decrement recently used count for rest
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (i != target && !cacheArray[i].accessCount)
                cacheArray[i].accessCount--;
        }
        cacheArray[target].data = getData(address);
        // cacheArray[hitIndex].data = 0;
    }
    // Find LRU function
    int findLRU(int startIndex, int endIndex)
    {
        int maxData = -1;
        int maxIndex = -1;
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (cacheArray[i].valid && cacheArray[i].data > maxData)
            {
                maxData = cacheArray[i].data;
                maxIndex = i;
            }
        }
        return maxIndex;
    }
    // Update LFU function
    void updateLFU(int hitIndex, int startIndex, int endIndex)
    {
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (i != hitIndex && cacheArray[i].valid)
            {
                cacheArray[i].data++;
            }
        }
        cacheArray[hitIndex].data = 1;
    }
    // Find LFU function
    int findLFU(int startIndex, int endIndex)
    {
        int minData = INT_MAX;
        int minIndex = -1;
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (cacheArray[i].valid && cacheArray[i].data < minData)
            {
                minData = cacheArray[i].data;
                minIndex = i;
            }
        }
        return minIndex;
    }
    // Find Random function
    int findRandom(int startIndex, int endIndex)
    {
        int randomIndex = rand() % (endIndex - startIndex + 1) + startIndex;
        return randomIndex;
    }
    // Find FIFO function
    int findFIFO(int startIndex, int endIndex)
    {
        int minData = INT_MAX;
        int minIndex = -1;
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (cacheArray[i].valid && cacheArray[i].data < minData)
            {
                minData = cacheArray[i].data;
                minIndex = i;
            }
        }
        return minIndex;
    }
    uint32_t getData(uint32_t address){

    }
    // Print function
    void caclulateStats()
    {
        accesses = hits + misses;
        coldMisses = misses - conflictMisses - capacityMisses;
        conflictMisses = numSets - 1;
        capacityMisses = misses - conflictMisses - coldMisses;
        float cpiWithCache = (float)(accesses + memoryStalls) / float(accesses);
        float cpiWithoutCache = (float)(accesses * missPenalty) / (float)accesses;
        cout << "Accesses: " << accesses << endl;
        cout << "Hits: " << hits << endl;
        cout << "Misses: " << misses << endl;
        cout << "Cold Misses: " << coldMisses << endl;
        cout << "Conflict Misses: " << conflictMisses << endl;
        cout << "Capacity Misses: " << capacityMisses << endl;
        cout << "Memory Stalls: " << memoryStalls << endl;
        cout << "CPI with Cache: " << cpiWithCache << endl;
        cout << "CPI with Cache: " << cpiWithCache << endl;
        cout << "CPI with Cache: " << fixed << setprecision(2) << cpiWithCache << endl;
        cout << "CPI without Cache: " << cpiWithoutCache << endl;
        cout << "CPI without Cache: " << fixed << setprecision(2) << cpiWithoutCache << endl;
    }
};
// Main function
int main()
{
    // Read input parameters
    int cacheSize, blockSize, numWays, missPenalty, hitTime;
    string cacheType, replacementPolicy;
    cout << "Enter Cache Size: ";
    cin >> cacheSize;
    cout << "Enter Block Size: ";
    cin >> blockSize;

    cout << "Enter Cache Type: ";
    cin >> cacheType;
    cout << "Enter Replacement Policy: ";
    cin >> replacementPolicy;
    cout << "Enter hit time: ";
    cin >> hitTime;
    cout << "Enter Number of Ways: ";
    cin >> numWays;
    cout << "Enter Miss Penalty: ";
    cin >> missPenalty;
    // Create Cache
    Cache cache(cacheSize, blockSize, cacheType, replacementPolicy, hitTime, numWays, missPenalty);
    // Read file
    // Read input file
    ifstream inFile("instruction.txt");
    if (inFile.is_open())
    {
        int address;
        while (inFile >> address)
        {

            // Lookup
            cache.lookup(address);
        }
        inFile.close();
    }
    else
    {
        cout << "Error opening file";
    }
    // Print stats
    cache.caclulateStats();
    int a=0x00200293,b= 0x00119463, c=0x002000B3,d= 0x0051CC63;
    vector<int> lst={
        0x00208233,
        0x002000B3,
        0x00400133,
        0x00128293
    };
    return 0;
}
