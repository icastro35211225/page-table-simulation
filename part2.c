#include <stdio.h>
#include <stdlib.h>

typedef struct{
  int valid;
  int fnum;
}PTE;

int find_em_emptyframe(int *);
int select_victimLRU(int *);

int main(int argc, char* argv[]){
  unsigned long la, pa;
  PTE PT[32];
  unsigned int p=5, f=3, d=7;
  unsigned int pnum, fnum, dnum;
  int CLK = 0, i, x, y;
  int freeframes[8] = {0,1,1,1,1,1,1,1};
  int LRUcount[8] = {0};
  int revmap[8] = {-1};
  int pageFaults = 0;

  if(argc <= 1){
    printf("ERROR! Not nough command line args\n");
    return -1;
  }

  FILE *fIn = fopen(argv[1], "rb");
  if(fIn == NULL){
    printf("ERROR! Could not open %s\n", argv[1]);
    return -2;
  }

  FILE *fOut = fopen(argv[2], "wb");
  if(fOut == NULL){
    printf("ERROR! Could not open %s\n", argv[2]);
    return -3;
  }

  for(i = 0; i < 32; i++){
    PT[i].valid = 0;
  }

  while(fread(&la, sizeof(la), 1, fIn) != 0){
    CLK++;
    pnum = la >> d;
    dnum = la & 0x7F;
    if(PT[pnum].valid == 1){
      fnum = PT[pnum].fnum;
      pa = (fnum << d) + dnum;
      fwrite(&pa, sizeof(unsigned long), 1, fOut);
      LRUcount[fnum] = CLK;
    }else{
      x = find_em_emptyframe(freeframes);
      if(x > 0){
        PT[pnum].fnum = x;
        PT[pnum].valid = 1;
        fnum = PT[pnum].fnum;
        pa = (fnum << d) + dnum;
        fwrite(&pa, sizeof(unsigned long), 1, fOut);
        LRUcount[fnum] = CLK;
        revmap[x] = pnum;
        pageFaults++;
      }else{
        y = select_victimLRU(LRUcount);
        PT[revmap[y]].valid = 0;
        PT[pnum].fnum = y;
        PT[pnum].valid = 1;
        fnum = PT[pnum].fnum;
        pa = (fnum << d) + dnum;
        fwrite(&pa, sizeof(unsigned long), 1, fOut);
        LRUcount[fnum] = CLK;
        revmap[fnum] = pnum;
        pageFaults++;
      }
    }
  }
  printf("Page Faults: %d\n", pageFaults);
}

int find_em_emptyframe(int *frames){
  int i;
  for(i = 1; i < 8; i++){
    if(frames[i] == 1){
      frames[i] = 0;
      return i;
    }
  }
  return 0;
}

int select_victimLRU(int *lru){
  int i, min, index = 0;
  min = lru[1];
  for(i = 1; i < 8; i++){
    if(lru[i] <= min){
      min = lru[i];
      index = i;
    }
  }
  return index;
}
