# splitfastq


**splitfastq** read a single-end fastq (or a paired-end fastq) and emit a read (or a interleaved pair of fastq) every 'n' records. Here, the motivation is to pipe parallel invocations of this tool into bwa:

```
splitfastq -n 10 -m 7 R1.fq.gz R2.fq.gz | bwa mem -p ref.fa -
```

**split2file** split a fastq file into N gzipped fastqs

```
./split2file -n 5 -o PREFIX test.R1.fq
```


## compilation

compile with make. path to a compiled htslib directory MUST be specified with `HTSLIB=`

```
make HTSLIB=../../packages/htslib-1.11
```

## Usage


```
  splitfastq -n (num) -m (modulo) <fastq1> 
  splitfastq -n (num) -m (modulo) <fastq1> <fastq2> 


  split2file -C (compression-level 0-9) -n (num)  -o <PREFIX> <fastq|stdin> 

```

## Author

Pierre Lindenbaum PhD @yokofakun . 2020

