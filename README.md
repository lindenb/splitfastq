# splitfastq


read a single-end fastq (or a paired-end fastq) and emit a read (or a interleaved pair of fastq) every 'n' records.

## motivation


split big fastq to stdout for BWA without creating files

## compilation

compile with make. path to htslib directory MUST be specified with `HTSLIB=`

```
make HTSLIB=../../packages/htslib-1.11
```

## Usage


```
  splitfastq -n (num) -m (modulo) <fastq1> 
  splitfastq -n (num) -m (modulo) <fastq1> <fastq2> 
```

## Author

Pierre Lindenbaum PhD @yokofakun

2020
