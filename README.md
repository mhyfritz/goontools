NOTE: this is work in progess. Here be <del>dragons</del> bugs. Potentially.

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

And in a different file you could have records with following
structure: sequence name key *chrom*  and position keys
*chromStart* and *chromEnd*, whose values define an interval on the sequence.

~~~
{"chrom": "chr20", "chromStart": 0, "chromEnd": 500, "strand": "+"}
~~~

Back to `goontools`.

`goontools` provides a set of commands to manipulate GOON files.
In particular, following commands are currently implemented:

* `index`
* `sort`
* `view`
* `idxstat`
* `extract` (experimental)
* `pprint`

Let's take a look at *bed.ldj* from the *example* directory:

~~~bash
# pretty-print first record
$ goontools pprint -n 1 bed.ldj
{
  "chromEnd": 5,
  "name": "name_0",
  "score": 426,
  "chromStart": 0,
  "chrom": "10",
  "strand": "-"
}
# sort and bgzip file:
$ goontools sort -s chrom -b chromStart bed.ldj > bed.srt.ldj
$ bgzip bed.srt.ldj
# positions are zero-based and "right-open" (non-inclusive end)
$ goontools index -0 -r -s chrom -b chromStart -e chromEnd bed.srt.ldj.gz
# check what we've indexed
$ goontools idxstat bed.srt.ldj.gz
SEQUENCE_NAME  1
SEQUENCE_NAME  10
SEQUENCE_NAME  11
SEQUENCE_NAME  3
SEQUENCE_NAME  X
SEQUENCE_KEY   chrom
START_KEY      chromStart
END_KEY        chromEnd
ZERO_BASED     true
RIGHT_OPEN     true
# get chromosome 1 records
$ goontools view bed.srt.ldj.gz 1
{"chromEnd": 11, "name": "name_6", "score": 711, "chromStart": 1, "chrom": "1", "strand": "+"}
{"chromEnd": 7, "name": "name_33", "score": 290, "chromStart": 2, "chrom": "1", "strand": "+"}
{"chromEnd": 4, "name": "name_49", "score": 923, "chromStart": 2, "chrom": "1", "strand": "-"}
{"chromEnd": 8, "name": "name_11", "score": 375, "chromStart": 4, "chrom": "1", "strand": "-"}
{"chromEnd": 9, "name": "name_37", "score": 669, "chromStart": 4, "chrom": "1", "strand": "-"}
{"chromEnd": 12, "name": "name_39", "score": 620, "chromStart": 4, "chrom": "1", "strand": "-"}
{"chromEnd": 14, "name": "name_21", "score": 402, "chromStart": 5, "chrom": "1", "strand": "-"}
{"chromEnd": 7, "name": "name_44", "score": 725, "chromStart": 6, "chrom": "1", "strand": "-"}
{"chromEnd": 14, "name": "name_19", "score": 593, "chromStart": 7, "chrom": "1", "strand": "-"}
{"chromEnd": 17, "name": "name_5", "score": 392, "chromStart": 10, "chrom": "1", "strand": "-"}
{"chromEnd": 14, "name": "name_16", "score": 364, "chromStart": 10, "chrom": "1", "strand": "+"}
{"chromEnd": 19, "name": "name_32", "score": 371, "chromStart": 10, "chrom": "1", "strand": "-"}
{"chromEnd": 18, "name": "name_34", "score": 628, "chromStart": 10, "chrom": "1", "strand": "-"}
# get intersecting intervals
# by default `view` assumes position encoding from original file, i.e. 0-based, half-open here
$ goontools view bed.srt.ldj.gz 1:7-10
{"chromEnd": 11, "name": "name_6", "score": 711, "chromStart": 1, "chrom": "1", "strand": "+"}
{"chromEnd": 8, "name": "name_11", "score": 375, "chromStart": 4, "chrom": "1", "strand": "-"}
{"chromEnd": 9, "name": "name_37", "score": 669, "chromStart": 4, "chrom": "1", "strand": "-"}
{"chromEnd": 12, "name": "name_39", "score": 620, "chromStart": 4, "chrom": "1", "strand": "-"}
{"chromEnd": 14, "name": "name_21", "score": 402, "chromStart": 5, "chrom": "1", "strand": "-"}
{"chromEnd": 14, "name": "name_19", "score": 593, "chromStart": 7, "chrom": "1", "strand": "-"}
# but we can also use a different encoding, e.g. pass a closed interval
$ goontools view -c bed.srt.ldj.gz 1:7-10
{"chromEnd": 11, "name": "name_6", "score": 711, "chromStart": 1, "chrom": "1", "strand": "+"}
{"chromEnd": 8, "name": "name_11", "score": 375, "chromStart": 4, "chrom": "1", "strand": "-"}
{"chromEnd": 9, "name": "name_37", "score": 669, "chromStart": 4, "chrom": "1", "strand": "-"}
{"chromEnd": 12, "name": "name_39", "score": 620, "chromStart": 4, "chrom": "1", "strand": "-"}
{"chromEnd": 14, "name": "name_21", "score": 402, "chromStart": 5, "chrom": "1", "strand": "-"}
{"chromEnd": 14, "name": "name_19", "score": 593, "chromStart": 7, "chrom": "1", "strand": "-"}
{"chromEnd": 17, "name": "name_5", "score": 392, "chromStart": 10, "chrom": "1", "strand": "-"}
{"chromEnd": 14, "name": "name_16", "score": 364, "chromStart": 10, "chrom": "1", "strand": "+"}
{"chromEnd": 19, "name": "name_32", "score": 371, "chromStart": 10, "chrom": "1", "strand": "-"}
{"chromEnd": 18, "name": "name_34", "score": 628, "chromStart": 10, "chrom": "1", "strand": "-"}
~~~

`goontools` takes inspiration and quite a bit of actual
code from `tabix`[1] and `samtools`[2].

[1] http://samtools.sourceforge.net/

[2] http://bioinformatics.oxfordjournals.org/content/27/5/718.full
