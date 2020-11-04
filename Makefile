CC?=gcc

ifeq ($(HTSLIB),)
$(error undefined $$HTSLIB)
endif

CFLAGS= -Wall -O3 -I$(HTSLIB)
LDFLAGS=  -L$(HTSLIB) -lz -lhts

all: test1 test2

splitfastq:  splitfastq.c
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

split2file:  split2file.c
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

test1: splitfastq test.R1.fq  test.R2.fq
	./splitfastq -n 5 -m 1 test.R1.fq  test.R2.fq | paste - - - - - - - - | cut -f1,5 > tmp0.txt
	test -s tmp0.txt
	cat test.R1.fq | paste - - - - | awk '(NR%5==1)' | cut -f 1 > tmp1.txt
	cat test.R2.fq | paste - - - - | awk '(NR%5==1)' | cut -f 1 > tmp2.txt
	paste tmp1.txt tmp2.txt > tmp3.txt
	test -s tmp3.txt
	cmp tmp0.txt tmp3.txt
	rm tmp0.txt tmp1.txt tmp2.txt tmp3.txt
	@echo "TEST: SUCCESS"

test2: split2file test.R1.fq
	rm -f PREFIX.*
	./split2file -C 9 -n 5 -o PREFIX test.R1.fq
	test -s ./PREFIX.00001.fastq.gz
	test -s ./PREFIX.00002.fastq.gz
	test -s ./PREFIX.00003.fastq.gz
	test -s ./PREFIX.00004.fastq.gz
	test -s ./PREFIX.00005.fastq.gz
	cat test.R1.fq | paste - - - - | awk '(NR%5==3)' | cut -f 1 > tmp1.txt
	gunzip -c ./PREFIX.00003.fastq.gz | paste - - - - |cut -f 1 > tmp2.txt
	cmp tmp1.txt tmp2.txt
	rm -f PREFIX.* tmp1.txt tmp2.txt
	@echo "TEST: SUCCESS"

