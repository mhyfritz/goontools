#! /usr/bin/env python

from __future__ import print_function
from random import choice, randint, seed
import json

seed(0xDEADBEEF)

chroms_chr = ['chr1', 'chr3', 'chr10', 'chr11', 'chrX']
chroms_raw = [c.lstrip('chr') for c in chroms_chr]

# BED-style file
with open('bed.ldj', 'w') as f:
    for i in xrange(50):
        s = randint(0, 10)
        e = s + randint(1, 10)
        rec = {
            'chrom': choice(chroms_raw),
            'chromStart': s,
            'chromEnd': e,
            'name': 'name_{}'.format(i),
            'score': randint(0, 1000),
            'strand': choice('+-')
        }
        print(json.dumps(rec), file=f)

# GFF-style file
with open('gff.ldj', 'w') as f:
    for i in xrange(50):
        s = randint(1, 10)
        e = s + randint(0, 10)
        rec = {
            'seqname': choice(chroms_chr),
            'source': 'goontools',
            'feature': 'unknown',
            'start': s,
            'end': e,
            'score': randint(0, 1000),
            'strand': choice('+-'),
            'group': 'group_{}'.format(i),
        }
        print(json.dumps(rec), file=f)
