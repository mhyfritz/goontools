goontools
=========

`goontools` processes GOON files.

GOON files?!

GOON (GenOme jsON) is a simple, flexible format that is easy to process and extend.
Its specification is the following:

* a GOON file is a line-delimited JSON (LDJSON) file
* every line encodes a datum of type *Object*
* every such object exposes a sequence name key with an associated *String* value
  and one or two position keys with associated integer values
  (strictly speaking of type *Number* with no fractional part)

...and that's it. Here's one valid GOON record, where *CHR* is the
sequence name key and *POS* the (only) position key:

~~~
{"CHR": "chrX", "POS": 500017, "ALLELES": ["C", "T"]}
~~~

And here's another one, where *chrom* is the sequence name key and
*chromStart* and *chromEnd* are position keys, whose values define
an interval on the sequence.

~~~
{"chrom": "chr20", "chromStart": 0, "chromEnd": 500, "strand": "+"}
~~~

Back to `goontools`.

`goontools` provides a set of commands to manipulate GOON files.
In particular, following commands are currently implemented:

*TODO: extend this section...*

* `sort`
* `index`
* `view`
* `idxstat`

`goontools` takes inspiration from <cite>[`tabix`][1]</cite> and 
<cite>[`samtools`][2]</cite>.
A lot of inspiration. In fact, most heavy lifting is
done using parts of their codebase.

NOTE: this is work in progess. Here be <del>dragons</del> bugs. Potentially.

[1] http://samtools.sourceforge.net/
[2] http://bioinformatics.oxfordjournals.org/content/27/5/718.full
