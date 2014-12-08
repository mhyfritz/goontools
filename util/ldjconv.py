#! /usr/bin/env python

from __future__ import print_function
import csv
import json
import click
import gzip
from collections import OrderedDict


def opener(fn):
    return gzip.open if fn.endswith('.gz') else open


@click.group()
def cli():
    pass


@click.command()
@click.option('sep', '-d', '--delimiter', default='\t',
              help='delimiter')
@click.option('colnames', '-c', '--colnames',
              help='column names comma-separated')
@click.option('na', '-n', '--na', default=None,
              help='NA value to omit')
@click.argument('fn', metavar='FILE')
def toldj(sep, colnames, na, fn):
    sep = sep.decode('string-escape')
    with opener(fn)(fn) as f:
        f_reader = csv.reader(f, delimiter=sep)
        if colnames is not None:
            colnames = colnames.split(',')
        else:
            colnames = next(f_reader)
        for fields in f_reader:
            d = {c: f for c, f in zip(colnames, fields) if f != na}
            print(json.dumps(d))


@click.command()
@click.option('sep', '-d', '--delimiter', default='\t',
              help='delimiter')
@click.option('colnames', '-c', '--colnames',
              help='column names comma-separated')
@click.option('na', '-n', '--na', default='',
              help='NA value to insert')
@click.argument('fn', metavar='FILE')
def fromldj(sep, colnames, na, fn):
    sep = sep.decode('string-escape')
    with opener(fn)(fn) as f:
        if colnames is not None:
            colnames = colnames.split(',')
        else:
            line = next(f)
            d = json.JSONDecoder(object_pairs_hook=OrderedDict).decode(line)
            colnames = d.keys()
            f.seek(0)
        print(sep.join(colnames))
        for line in f:
            d = json.loads(line)
            print(sep.join(str(d.get(c, na)) for c in colnames))

cli.add_command(toldj)
cli.add_command(fromldj)

if __name__ == '__main__':
    cli()
