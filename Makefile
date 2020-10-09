CC?=gcc

ifeq ($(HTSLIB),)
$(error undefined $$HTSLIB)
endif

CFLAGS= -Wall -O3 -I$(HTSLIB)
LDFLAGS= $(HTSLIB)/libhts.a -lz 

all: test

splitfastq:  splitfastq.c
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

test: splitfastq test.R1.fq  test.R2.fq
	./splitfastq -n 5 -m 1 test.R1.fq  test.R2.fq | paste - - - - - - - - | cut -f1,5 > tmp0.txt
	cat test.R1.fq | paste - - - - | awk '(NR%5==1)' | cut -f 1 > tmp1.txt
	cat test.R2.fq | paste - - - - | awk '(NR%5==1)' | cut -f 1 > tmp2.txt
	paste tmp1.txt tmp2.txt > tmp3.txt
	cmp tmp0.txt tmp3.txt
	rm tmp0.txt tmp1.txt tmp2.txt tmp3.txt
	@echo "TEST: SUCCESS"
