/*
The MIT License (MIT)

Copyright (c) 2020 Pierre Lindenbaum PhD.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>
#include <stdint.h>
#include "signal.h"
#include <zlib.h>
#include "htslib/kseq.h"


KSEQ_INIT(gzFile, gzread)

typedef struct splitFile {
	char* filename;
	gzFile out;
	long count;
	}SplitFile;

static void usage(FILE* out) {
fprintf(out,"split2file. Pierre Lindenbaum 2020.\n");
fprintf(out,"Usage:\n");
fprintf(out,"  split2file -C (compression-level 0-9) -n (num)  -o <PREFIX> <fastq|stdin> \n");
fprintf(out,"\n");
}

int main(int argc,char** argv) {
    int opt;
    int i;
    long nsplits=0;
    char* prefix = NULL;
    int level=5;
    gzFile fp1;
    kseq_t* ks1;
    SplitFile* splitFiles = NULL;
    while ((opt = getopt(argc, argv, "ho:n:C:")) != -1) {
	    switch (opt) {
	    case 'h':
		    usage(stdout);
		    return 0;
            case 'C': level=atoi(optarg);
		    break;
	     case 'o':
		    prefix = optarg;
		    break;
	    case 'n':
		    nsplits =atol(optarg);
		    break;
	    case '?':
		    fprintf(stderr,"unknown option '%c'.\n",(char)optopt);
		    return EXIT_FAILURE;
	    default: /* '?' */
		    fprintf(stderr,"unknown option\n");
		    return EXIT_FAILURE;
	    }
	}
    if(prefix==NULL || strlen(prefix)==0UL) {
	fprintf(stderr,"empty prefix.\n" );
	usage(stderr);
	return EXIT_FAILURE;
	}
    if(nsplits<=0)  {
	fprintf(stderr,"Bad value for nsplit : %ld.\n",nsplits );
	usage(stderr);
	return EXIT_FAILURE;
        }
    if(level<0 || level>9)  {
	fprintf(stderr,"Bad compression level: %d.\n",level );
	usage(stderr);
	return EXIT_FAILURE;
        }
    splitFiles = (SplitFile*)malloc(sizeof(SplitFile)*nsplits);
    if(splitFiles==NULL) {
	fprintf(stderr,"Out of memory %ld.\n",nsplits );
	return EXIT_FAILURE;
	}

    if(!(argc==optind || optind+1==argc))
	{
	fprintf(stderr,"Illegal number of arguments.\n");
	usage(stderr);
	return EXIT_FAILURE;
	}

    for(i=0;i< nsplits;i++) {
        char mode[5];
	char tmp[30];
	FILE* exists=NULL;
	sprintf(tmp,".%05d.fastq.gz",(i+1));

	splitFiles[i].filename = (char*)malloc(sizeof(char)*(strlen(prefix)+strlen(tmp)+1));
	if(splitFiles==NULL) {
		fprintf(stderr,"Out of memory %ld.\n",nsplits );
		return EXIT_FAILURE;
		}
	splitFiles[i].filename[0]=0;
	strcat(splitFiles[i].filename,prefix);
	strcat(splitFiles[i].filename,tmp);
	splitFiles[i].count = 0L;

	exists = fopen(splitFiles[i].filename,"rb");
	if(exists!=NULL) {
		fclose(exists);
		fprintf(stderr,"File already exists %s.\n",splitFiles[i].filename );
		return EXIT_FAILURE;
		}

	sprintf(mode,"wb%d",level);
	splitFiles[i].out = gzopen(splitFiles[i].filename,mode);
	if(splitFiles[i].out==NULL) {
		fprintf(stderr,"Cannot open %s for writing.(%s)\n",splitFiles[i].filename,strerror(errno));
		return EXIT_FAILURE;
		}

	}
	
    #define KSWRITE(X) gzwrite(out,(void*)(X.s),X.l)

    long nReads = 0L;

		
		fp1 = (optind==argc?gzdopen(fileno(stdin), "r"):gzopen(argv[optind], "r"));
		if(fp1==NULL) {
			fprintf(stderr,"Cannot open %s.(%s)\n",(optind==argc?"<STDIN>":argv[optind]),strerror(errno));
			return EXIT_FAILURE;
			}
		ks1 = kseq_init(fp1);\
		if(ks1==NULL) {
			fprintf(stderr,"Cannot initialize reader for \"%s\".\n",(optind==argc?"<STDIN>":argv[optind])); 			return EXIT_FAILURE;
			}
		while (kseq_read(ks1) >= 0)  {
			gzFile out  = splitFiles[nReads%nsplits].out;
						
			gzputc(out,'@');
			KSWRITE(ks1->name);
			gzputc(out,'\n');
			KSWRITE(ks1->seq);
			gzputc(out,'\n');
			gzputc(out,'+');
			gzputc(out,'\n');
			KSWRITE(ks1->qual);
			if(gzputc(out,'\n')==-1) {
				fprintf(stderr,"[split2file]I/O error\n");
				return EXIT_FAILURE;
				}
			splitFiles[nReads%nsplits].count++;
			nReads++;			
			}
	kseq_destroy(ks1);
	gzclose(fp1);

	    

     
    for(i=0;i< nsplits;i++) {
	fprintf(stderr,"[LOG] closing \"%s\" N=%ld.\n",splitFiles[i].filename,splitFiles[i].count);
	gzFile out  = splitFiles[i].out;
	gzclose(out);
	free(splitFiles[i].filename);
	}
    free(splitFiles);

     return EXIT_SUCCESS;
     }
