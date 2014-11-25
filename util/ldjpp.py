#! /usr/bin/env python

from __future__ import print_function
import argparse
import json

parser = argparse.ArgumentParser(description='Pretty-print LDJSON.')
parser.add_argument('--indent', metavar='N', type=int, default=2,
                    dest='indent', help='indentation for pretty-printing')
parser.add_argument('--file', metavar='FILE', required=True, dest='file',
                    type=argparse.FileType('r'), help='input LDJSON file')
parser.add_argument('--sort', action='store_true', dest='sortkeys',
                    help='sort object keys')

args = parser.parse_args()

for line in args.file:
    record = json.loads(line)
    print(json.dumps(record, indent=args.indent, sort_keys=args.sortkeys))
