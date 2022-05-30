#include "si5351a.h"
#include <stdio.h>

 #include "si5351a.c"



int main(int argc, char* argv[])
{
        
      int x;
      char* s = argv[argc - 1];
      sscanf(s, "%d", &x);
      si5351aSetFrequency(x);

        return 0;
}
