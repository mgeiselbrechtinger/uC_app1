#!/bin/python3

import os
import random

platform = [0,1,0,0,0,0]

gaps = 0
while True:
    for i in range(len(platform)):
        upper = 128
        lower = -1
        if i > 0:
            lower = platform[i-1]
        
        while True:
            r = ord(os.urandom(1))
            
            if r == lower + 1 and r < upper:
                platform[i] = r
                break
            
            elif r > lower + 7 and r < upper:
                platform[i] = r
                gaps += 1
                break

    if gaps > 0:
        break

for p in platform:
    print(p)
