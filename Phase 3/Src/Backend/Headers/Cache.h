#ifndef CACHE
#define CACHE
// #include "Common.h"
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <random>
#include <cmath>
#include <climits>
#include <cstdlib>

using namespace std;

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
    uint8_t *memory;
    int ll_pos;

public:
    // Constructor
    Cache(int cs, int bs, string ct, string rp, int ht, int mp, int nw, uint8_t* mem) : ll_pos(0),cacheSize(cs), blockSize(bs), numBlocks(cs / bs), cacheType(ct), replacementPolicy(rp), hitTime(ht), missPenalty(mp), numWays(nw), accesses(0), hits(0), misses(0), coldMisses(0), conflictMisses(0), capacityMisses(0), memoryStalls(0),  memory(mem)
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
                hits++;
                hitIndex = i;
                break;
            }
        }
        // if miss
        if (hitIndex == -1){
            misses++;
            if(replacementPolicy=="LRU")
                hitIndex=updateLRU(startIndex, endIndex, address);
            else if(replacementPolicy=="LFU")
                hitIndex=updateLFU(startIndex, endIndex, address);
            else if(replacementPolicy=="RANDOM")
                hitIndex=updateRandom(startIndex, endIndex, address);
            else if(replacementPolicy=="FIFO")
                hitIndex=updateFIFO(startIndex, endIndex, address);
        }
        // hit
        // hits++;
        if(replacementPolicy=="LRU")
            return retrieveLRU(startIndex, endIndex, address, hitIndex);
        else if(replacementPolicy=="LFU")
            return retrieveLFU(startIndex, endIndex, address, hitIndex);
        return retrieve(address, hitIndex);
    }
    //write function
    void write(uint32_t address, uint32_t value) {
    int index = ((address / (blockSize*4)) % numSets);
    int tag = address / ((blockSize * numSets * 4));
    int startIndex = index * numWays;
    int endIndex = startIndex + numWays - 1;
    int way = -1;
    int hitIndex = -1;

    // check if hit
    for (int i = startIndex; i <= endIndex; i++) {
        if ((cacheArray[i].tag == tag) && cacheArray[i].valid) {
            hitIndex = i;
            break;
        }
    }

    // if miss
    if (hitIndex == -1) {
        misses++;
        if (replacementPolicy == "LRU") {
            hitIndex = updateLRU(startIndex, endIndex, address);
        } else if (replacementPolicy == "LFU") {
            hitIndex = updateLFU(startIndex, endIndex, address);
        } else if (replacementPolicy == "RANDOM") {
            hitIndex = updateRandom(startIndex, endIndex, address);
        } else if (replacementPolicy == "FIFO") {
            hitIndex = updateFIFO(startIndex, endIndex, address);
        }
    } 
    *(uint32_t*)(memory+address)=value;
    // int memAddress = address;
    int memAddress = (address / (4*blockSize))*4*blockSize;
    for (int i = 0; i < blockSize; i++) {
        int temp=memory[memAddress];
        cacheArray[hitIndex].Data[i] = *(uint32_t*)(memory+memAddress) ;
        memAddress += 4;
    }

    // If the block is evicted from the cache, write it back to memory
    if (cacheArray[hitIndex].accessCount == 0) {
        int evictedAddress = (cacheArray[hitIndex].tag * numSets + index) * blockSize * 4;
        for (int i = 0; i < blockSize; i++) {
            memory[evictedAddress + i*4] = cacheArray[hitIndex].Data[i];
        }
        cacheArray[hitIndex].dirty = false;
        cacheArray[hitIndex].valid = false;
        cacheArray[hitIndex].tag = -1;
    }
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
        return target;
        // for (int i = startIndex; i <= endIndex; i++)
        // {
        //     if (i != target && cacheArray[i].accessCount)
        //         cacheArray[i].accessCount--;
        // }
        // cacheArray[target].Data = getData(address);
        // return cacheArray[target].Data[(address/4)%blockSize];
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
        idx->accessCount = 0;
        return idx-cacheArray.begin();
        // return idx->Data[(address/4)%blockSize];
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
            return target;
            // return cacheArray[target].Data[(address/4)%blockSize];
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(startIndex,endIndex);
        target=dis(gen);
        cacheArray[target].Data = getData(address);
        cacheArray[target].tag=address / ((blockSize * numSets * 4));
        return target;
        // return cacheArray[target].Data[(address/4)%blockSize];
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
            return target;
            // return cacheArray[target].Data[(address/4)%blockSize];
        }
        auto idx=min_element(cacheArray.begin()+startIndex,cacheArray.begin()+endIndex+1, [](cacheBlock &a, cacheBlock &b){return a.accessCount<b.accessCount;});
        idx->Data = getData(address);
        idx->valid=true;
        idx->accessCount = ll_pos;
        idx->tag=address / ((blockSize * numSets * 4));
        ll_pos++;
        return idx-cacheArray.begin();
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


#endif