#!/bin/python3

import os
import random

platform = [0,1,0,0,0,0]

gaps = 0
while True:
    for i in range(len(platform)):
        upper = 127
        lower = -1
        if i > 0:
            lower = platform[i-1]
        
        r = random.randint(lower + 1, upper)
           
        platform[i] = r

        if r > lower + 6:
            gaps += 1
            
    if gaps > 0:
        break

for p in platform:
    print(p)
