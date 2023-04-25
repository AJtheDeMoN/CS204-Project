#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <random>
#include <cmath>
#include <climits>
#include <cstdlib>
#include <algorithm>

using namespace std;
uint8_t memory[1024];
struct cacheBlock
{
    uint32_t tag;
    bool valid;
    bool dirty;
    int accessCount;
    vector<uint32_t> Data;
    cacheBlock(): tag(-1), valid(false), dirty(false), accessCount(0){}
    cacheBlock(int n): tag(-1), valid(false), dirty(false), accessCount(0), Data(n){}
};
class Cache
{
protected:
    int cacheSize,blockSize,numSets,numWays,numBlocks;
    string cacheType, replacementPolicy;
    int hitTime, missPenalty, accesses, hits, misses, coldMisses, conflictMisses, capacityMisses, memoryStalls;
    vector<cacheBlock> cacheArray;
    int ll_pos;

public:
    // Constructor
    Cache(int cs, int bs, string ct, string rp, int ht, int mp, int nw) : ll_pos(0),cacheSize(cs), blockSize(bs), numBlocks(cs / bs), cacheType(ct), replacementPolicy(rp), hitTime(ht), missPenalty(mp), numWays(nw), accesses(0), hits(0), misses(0), coldMisses(0), conflictMisses(0), capacityMisses(0), memoryStalls(0)
    {
        if (cacheType == "direct_mapped")
        {
            numSets = numBlocks;
            numWays=1;
        }
        else if (cacheType == "fully_assoc")
        {
            numSets = 1;
            numWays = numBlocks;
        }
        else if (cacheType == "set_assoc")
        {
            numSets = numBlocks / numWays;
        }
        cacheArray=vector<cacheBlock>(numBlocks, cacheBlock(blockSize));
    }
    // Destructor
    ~Cache()
    {
        cacheArray.clear();
    }
    // Access function
    uint32_t lookup(int address)
    {
        int index = ((address / (blockSize*4)) % numSets);
        int tag = address / ((blockSize * numSets * 4));
        int startIndex = index * numWays;
        int endIndex = startIndex + numWays - 1;
        int way = -1;
        int hitIndex = -1;

        // check if hit
        for (int i = startIndex; i <= endIndex; i++)
        {
            if ((cacheArray[i].tag == tag) && cacheArray[i].valid)
            {
                hitIndex = i;
                break;
            }
        }
        // if miss
        if (hitIndex == -1){
            misses++;
            if(replacementPolicy=="LRU")
                return updateLRU(startIndex, endIndex, address);
            else if(replacementPolicy=="LFU")
                return updateLFU(startIndex, endIndex, address);
            else if(replacementPolicy=="RANDOM"){
                return updateRandom(startIndex, endIndex, address);
            }
            else if(replacementPolicy=="FIFO"){
                return updateFIFO(startIndex, endIndex, address);
            }
        }
        // hit
        hits++;
        if(replacementPolicy=="LRU")
            return retrieveLRU(startIndex, endIndex, address, hitIndex);
        else if(replacementPolicy=="LFU")
            return retrieveLFU(startIndex, endIndex, address, hitIndex);
        return retrieve(address, hitIndex);
    }
    // Find LRU function
    uint32_t updateLRU(int startIndex, int endIndex, uint32_t address){
        int target = -1;
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (!cacheArray[i].valid || !cacheArray[i].accessCount)
            {
                cacheArray[i].accessCount = numWays - 1;
                cacheArray[i].valid = true;
                cacheArray[i].tag = address / ((blockSize * numSets * 4));
                target = i;
                break;
            }
        }
        // decrement recently used count for rest
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (i != target && cacheArray[i].accessCount)
                cacheArray[i].accessCount--;
        }
        cacheArray[target].Data = getData(address);
        return cacheArray[target].Data[(address/4)%blockSize];
    }
    // retrieve LRU function
    uint32_t retrieveLRU(int startIndex, int endIndex, uint32_t address, int hitIndex){
        for (int i=startIndex; i<endIndex; i++)
            if(i!=hitIndex && cacheArray[i].accessCount)
                cacheArray[i].accessCount--;
        return cacheArray[hitIndex].Data[(address/4)%blockSize];
    }
    // Update LFU function
    uint32_t updateLFU(int startIndex, int endIndex, uint32_t address){
        int target = -1;
        auto idx=min_element(cacheArray.begin()+startIndex,cacheArray.begin()+endIndex+1, [](cacheBlock &a, cacheBlock &b){return a.accessCount<b.accessCount;});
        idx->Data = getData(address);
        idx->valid=true;
        idx->tag=address / ((blockSize * numSets * 4));
        idx->accessCount = 1;
        return idx->Data[(address/4)%blockSize];
    }
    //retrieve LFU function
    uint32_t retrieveLFU(int startIndex, int endIndex, uint32_t address, int hitIndex){
        cacheArray[hitIndex].accessCount++;
        return cacheArray[hitIndex].Data[(address/4)%blockSize];
    }
    //update random function
    uint32_t updateRandom(int startIndex, int endIndex, uint32_t address){
        int target = -1;
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (!cacheArray[i].valid)
            {
                cacheArray[i].valid = true;
                cacheArray[i].tag = address / ((blockSize * numSets * 4));
                cacheArray[i].Data = getData(address);
                target = i;
                break;
            }
        }
        if(target!=-1)
            return cacheArray[target].Data[(address/4)%blockSize];
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(startIndex,endIndex);
        target=dis(gen);
        cacheArray[target].Data = getData(address);
        cacheArray[target].tag=address / ((blockSize * numSets * 4));
        return cacheArray[target].Data[(address/4)%blockSize];
    }
    //update FIFO function
    uint32_t updateFIFO(int startIndex, int endIndex, uint32_t address){
        int target = -1;
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (!cacheArray[i].valid)
            {
                cacheArray[i].valid = true;
                cacheArray[i].Data = getData(address);
                cacheArray[i].accessCount = ll_pos;
                cacheArray[i].tag = address / ((blockSize * numSets * 4));
                target = i;
                break;
            }
        }
        if(target!=-1){
            ll_pos++;
            return cacheArray[target].Data[(address/4)%blockSize];
        }
        auto idx=min_element(cacheArray.begin()+startIndex,cacheArray.begin()+endIndex+1, [](cacheBlock &a, cacheBlock &b){return a.accessCount<b.accessCount;});
        idx->Data = getData(address);
        idx->valid=true;
        idx->accessCount = ll_pos;
        idx->tag=address / ((blockSize * numSets * 4));
        ll_pos++;
        return idx->Data[(address/4)%blockSize];
    }
    
    //retrieve random/FIFO function
    uint32_t retrieve(uint32_t address, int hitIndex){
        return cacheArray[hitIndex].Data[(address/4)%blockSize];
    }
    vector<uint32_t> getData(uint32_t address){
        uint32_t mem_address=address/(4*blockSize)*4*blockSize;
        vector<uint32_t> vals(blockSize);
        for(int i=0; i<blockSize; i++, mem_address+=4){
            // vals[i]=*(uint32_t*)memory[mem_address+i*4];
            vals[i]=*(uint32_t*)(memory+mem_address);
        }
        // int val=mem_address-address;
        return vals;
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
void store_instructions(char* location){
    FILE *inp=fopen(location, "r");
    unsigned int address, code;
    while(fscanf(inp, "%x %x",&address,&code)!=EOF){
        *(uint32_t*)(memory+address)=code;
    }
}
int main()
{
    // Read input parameters
    int cacheSize, blockSize, numWays, missPenalty, hitTime;
    string cacheType, replacementPolicy;
    // cout << "Enter Cache Size: ";
    // cin >> cacheSize;
    // cout << "Enter Block Size: ";
    // cin >> blockSize;
    // cout << "Enter Cache Type: ";
    // cin >> cacheType;
    // cout << "Enter Replacement Policy: ";
    // cin >> replacementPolicy;
    // cout << "Enter hit time: ";
    // cin >> hitTime;
    // cout << "Enter Number of Ways: ";
    // cin >> numWays;
    // cout << "Enter Miss Penalty: ";
    // cin >> missPenalty;
    cacheSize=4, blockSize=1, cacheType="fully_assoc", replacementPolicy="FIFO", hitTime=1, numWays=2, missPenalty=20;
    // Create Cache
    store_instructions("temp.txt");
    Cache cache(cacheSize, blockSize, cacheType, replacementPolicy, hitTime, missPenalty, numWays);
    // Read file
    // Read input file
    ifstream inFile("instruction.txt");
    if (inFile.is_open())
    {
        int address;
        while (inFile >> hex>>address)
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
    return 0;
}
