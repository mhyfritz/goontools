#! /usr/bin/env python

from __future__ import print_function
import click
import json
from collections import OrderedDict


def json_loader(sortkeys):
    def _loader(line):
        if sortkeys:
            return json.loads(line)
        else:
            # if --no-sortkeys, let's preserve file order
            return json.JSONDecoder(object_pairs_hook=OrderedDict).decode(line)

    return _loader


@click.command()
@click.option('indent', '-i', '--indent', default=2,
              help='indentation for pretty-printing')
@click.option('--sortkeys/--no-sortkeys', default=False,
              help='sort object keys')
@click.argument('infile', type=click.File())
def cli(indent, sortkeys, infile):
    """Pretty-print LDJSON."""
    loader = json_loader(sortkeys)
    for line in infile:
        record = loader(line)
        print(json.dumps(record, indent=indent, sort_keys=sortkeys))

if __name__ == '__main__':
    cli()
