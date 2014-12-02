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

* `sort`
* `index`
* `view`
* `idxstat`

Let's take a look at `test.ldj` from the `example` directory:

~~~
$ head -1 test.ldj
{"KEY3": "qux0", "KEY2": "baz0", "KEY1": "bar0", "KEY0": "foo0", "POS": 5, "CHROM": "chr3"}
# only one position, so no END key needed:
$ goontools sort -s CHROM -b POS test.ldj > test.srt.ldj
$ bgzip test.srt.ldj
# zero-based positions, i.e. pass `-0`
$ goontools index -s CHROM -b POS -0 test.srt.ldj.gz
# by default `view` assumes position encoding from original file, i.e. 0-based, closed here
$ goontools view test.srt.ldj.gz chr1
{"KEY3": "qux17", "KEY2": "baz17", "KEY1": "bar17", "KEY0": "foo17", "POS": 0, "CHROM": "chr1"}
{"KEY3": "qux2", "KEY2": "baz2", "KEY1": "bar2", "KEY0": "foo2", "POS": 9, "CHROM": "chr1"}
{"KEY3": "qux7", "KEY2": "baz7", "KEY1": "bar7", "KEY0": "foo7", "POS": 9, "CHROM": "chr1"}
$ goontools view test.srt.ldj.gz chr1:0-8
{"KEY3": "qux17", "KEY2": "baz17", "KEY1": "bar17", "KEY0": "foo17", "POS": 0, "CHROM": "chr1"}
$ goontools view test.srt.ldj.gz chr1:9
{"KEY3": "qux2", "KEY2": "baz2", "KEY1": "bar2", "KEY0": "foo2", "POS": 9, "CHROM": "chr1"}
{"KEY3": "qux7", "KEY2": "baz7", "KEY1": "bar7", "KEY0": "foo7", "POS": 9, "CHROM": "chr1"}
~~~

`goontools` takes inspiration from `tabix`[1] and `samtools`[2].
A lot of inspiration. In fact, most heavy lifting is
done using parts of their codebase.
Kudos [Heng](http://en.wikipedia.org/wiki/Heng_Li)!

NOTE: this is work in progess. Here be <del>dragons</del> bugs. Potentially.

[1] http://samtools.sourceforge.net/

[2] http://bioinformatics.oxfordjournals.org/content/27/5/718.full
