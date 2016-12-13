#!/usr/bin/env python3
from pyeda.inter import *
import sys

try:
    inputfn = sys.argv[1]
    outputfn = sys.argv[2]
except:
    print("Usage : ./hw2.py <input file> <output file>");
    exit(1)

board = []
try:
    with open(inputfn) as inputfile:
        for r in inputfile.readlines():
            board.append([int(i) for i in r.split()])
    n = len(board)
    sqrtn = int(n**0.5)
    for r in board:
        if len(r) != n:
            raise
except:
    print("Error : failed to read input file or wrong format.")
    exit(1)

b = 0
while 2**b < n:
    b += 1

x = exprvars('x', n, n, b)

res = {}
for i in range(n):
    for j in range(n):
        if board[i][j] != 0:
            for l in range(b):
                res[x[i, j, l]] = ((board[i][j] - 1) >> l) & 1

fv = And(*[
        And(*[
            And(*[
                Or(*[ 
                    Xor(
                        res.get(x[i, j, l], x[i, j, l]),
                        (k >> l) & 1
                        )
                    for l in range(b)])
                for k in range(n, 2**b)])
            for j in range(n)])
        for i in range(n)])
fr = And(*[
        And(*[
            And(*[
                Or(*[
                    Xor(
                        res.get(x[i, j, l], x[i, j, l]),
                        res.get(x[i, k, l], x[i, k, l])
                        )
                    for l in range(b)])
                for k in range(j)])
            for j in range(n)])
        for i in range(n)])
fc = And(*[
        And(*[
            And(*[
                Or(*[ 
                    Xor(
                        res.get(x[j, i, l], x[j, i, l]),
                        res.get(x[k, i, l], x[k, i, l])
                        )
                    for l in range(b)])
                for k in range(j)])
            for j in range(n)])
        for i in range(n)])
fb = And(*[
        And(*[
            And(*[
                Or(*[
                    Xor(
                        res.get(x[i // sqrtn * sqrtn + j // sqrtn, i % sqrtn * sqrtn + j % sqrtn, l],
                            x[i // sqrtn * sqrtn + j // sqrtn, i % sqrtn * sqrtn + j % sqrtn, l]),
                        res.get(x[i // sqrtn * sqrtn + k // sqrtn, i % sqrtn * sqrtn + k % sqrtn, l],
                            x[i // sqrtn * sqrtn + k // sqrtn, i % sqrtn * sqrtn + k % sqrtn, l])
                        )
                    for l in range(b)])
                for k in range(j)])
            for j in range(n)])
        for i in range(n)])
f = And(fv, fr, fc, fb)

f = expr2bdd(f)

with open(outputfn, 'w') as outputfile:
    outputfile.write(str(f.satisfy_count()))            
