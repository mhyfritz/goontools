#! /usr/bin/env python

from __future__ import print_function
import csv
import json
import click
import gzip
from collections import OrderedDict


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
@click.argument('infile', type=click.File())
def toldj(sep, colnames, na, infile):
    sep = sep.decode('string-escape')
    f_reader = csv.reader(infile, delimiter=sep)
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
@click.option('colnames', '-c', '--colnames', metavar='CSV',
              help='column names comma-separated')
@click.option('na', '-n', '--na', default='',
              help='NA value to insert')
@click.argument('infile', type=click.File())
def fromldj(sep, colnames, na, infile):
    sep = sep.decode('string-escape')
    if colnames is not None:
        colnames = colnames.split(',')
    else:
        line = next(infile)
        d = json.JSONDecoder(object_pairs_hook=OrderedDict).decode(line)
        colnames = d.keys()
        infile.seek(0)
    print(sep.join(colnames))
    for line in infile:
        d = json.loads(line)
        print(sep.join(str(d.get(c, na)) for c in colnames))

cli.add_command(toldj)
cli.add_command(fromldj)

if __name__ == '__main__':
    cli()
