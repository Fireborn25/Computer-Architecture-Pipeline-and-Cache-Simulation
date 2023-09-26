#include <bits/stdc++.h>
using ll = long long;
using namespace std;

ll useTime = 0;
ll WRMiss[2][2], WriteBacks[3], WR[2][2];
ll totalTimeTaken = 0;

struct Block
{
    ll dirtyBit = 0, lru = 0;
    vector<ll> data;
    Block(ll size)
    {
        data = vector<ll>(size,-1);
    }
};

struct Cache
{
    ll cacheType; 
    ll size_b, cache_size, Assoc;
    vector<Block> Set;
    vector<vector<Block>> Mem;
    vector<vector<ll>> TagArray;
    Cache *nxt; 

    Cache(ll bs, ll s, ll a, ll type)
    {
        cacheType = type;
        size_b = bs;
        cache_size = s;
        Assoc = a;
        Block block = Block(size_b);
        Set = vector<Block>(Assoc,block);
        Mem = vector<vector<Block>>(cache_size/(size_b*Assoc),Set);
        TagArray = vector<vector<ll>>(cache_size/(size_b*Assoc),vector<ll>(Assoc,-1));
    }

    void WRfun(ll address,int x)
    {
        totalTimeTaken += (cacheType==2)?200:(cacheType==1)?20:1;
        if(cacheType == 2) {return;}
        WR[x][cacheType]++;
        ll offset = address%(size_b);
        ll tag = address/(cache_size/Assoc);
        ll set = (address/(size_b))%(cache_size/(size_b*Assoc));

        for(ll i=0;i<Assoc;i++) {
            if(TagArray[set][i] == tag){
                Mem[set][i].lru = useTime++;
                if (x ==1){Mem[set][i].dirtyBit = 1;}
                return;
            }
        } 
        WRMiss[x][cacheType]++; 
        ll leastUsed = 0;
        for (ll i = 0; i < Assoc; i++){
            if(Mem[set][i].lru < Mem[set][leastUsed].lru){leastUsed = i;}
        }
        if (Mem[set][leastUsed].dirtyBit) {
            ll newaddr = TagArray[set][leastUsed]*(cache_size/Assoc)+set*(size_b);
            WriteBacks[cacheType]++;
            nxt->WRfun(newaddr,1);
        } 
        nxt->WRfun(address,0);
        Mem[set][leastUsed].lru = useTime++; 
        Mem[set][leastUsed].dirtyBit = (x==0)?0: 1; 
        TagArray[set][leastUsed] = tag; 
    }
};

void PrintResults(){
    cout << "===== Simulation Results =====";
    cout << "\ni. number of L1 reads:\t\t\t\t" << dec << WR[0][0];
    cout << "\nii. number of L1 read misses:\t\t\t" << dec << WRMiss[0][0];
    cout << "\niii. number of L1 writes:\t\t\t" << dec << WR[1][0];
    cout << "\niv. number of L1 write misses:\t\t\t" << dec << WRMiss[1][0];
    cout << "\nv. L1 miss rate:\t\t\t\t" << fixed << setprecision(4) << ((double)(WRMiss[0][0] + WRMiss[1][0]))/(WR[0][0] + WR[1][0]);
    cout << "\nvi. number of writebacks from L1 memory:\t" << dec << WriteBacks[0];
    cout << "\nvii. number of L2 reads:\t\t\t" << dec << WR[0][1];
    cout << "\nviii. number of L2 read misses:\t\t\t" << dec << WRMiss[0][1];
    cout << "\nix. number of L2 writes:\t\t\t" << dec << WR[1][1];
    cout << "\nx. number of L2 write misses:\t\t\t" << dec << WRMiss[1][1];
    cout << "\nxi. L2 miss rate:\t\t\t\t" << fixed << setprecision(4) << ((double)(WRMiss[0][1] + WRMiss[1][1]))/(WR[0][1] + WR[1][1]); 
    cout << "\nxii. number of writebacks from L2 memory:\t" << dec << WriteBacks[1] << "\n";
    cout << "Total time taken: " << totalTimeTaken << " ns \n";
}

ll Hextobinary(string hexadecimal){
    ll bin = 0,j =-1;
    ll size = hexadecimal.size() -1;
    for(ll i= size; i>=0;i--){
        if(hexadecimal[i]>='0' && hexadecimal[i]<='9'){
            j++;
            bin+=(hexadecimal[i]-48)*(pow(16,j));
        }
        else if(hexadecimal[i] >= 'a' && hexadecimal[i] <= 'f'){
            j++;
            bin+=(hexadecimal[i]-87)*(pow(16,j));
        }
        else{
            cerr<<"Invalid digit in hexadecimal"<<endl;
        }
    }
    return bin;
}

int main(int argc, char *argv[]){
	if ((argc != 7) ){
		cerr << "7 arguments required\n"<< argc;
		return 0;
	}
    ifstream file(argv[6]);
    if (file.is_open()){    
        Cache L1 = Cache(stoi(argv[1]),stoi(argv[2]),stoi(argv[3]), 0);
        Cache L2 = Cache(stoi(argv[1]),stoi(argv[4]),stoi(argv[5]), 1);
        Cache DRAM = Cache(stoi(argv[1]),1024,16,2);
        L1.nxt = &L2;
        L2.nxt = &DRAM;
        while(file.good()){
            string type, b;
            file >> type;
            if(file.good()){
                file >> b;
                ll addr = Hextobinary(b);
                if(type == "r" ){
                    //read
                    L1.WRfun(addr,0);
                }
                else if(type == "w" ){
                    //write
                    L1.WRfun(addr,1);
                }
                else {
                    cerr<< " Error while reading this line"<<'\n';
                }
            }
        }
        PrintResults();
        file.close();
    }
    else{
        cerr << "File could not be opened";
    }
    return 0;
}
