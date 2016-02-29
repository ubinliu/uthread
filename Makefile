#COMAKE2 edit-mode: -*- Makefile -*-
####################64Bit Mode####################
ifeq ($(shell uname -m),x86_64)
CC=gcc
CXX=g++
CXXFLAGS=-g \
  -pipe \
  -W \
  -Wall \
  -Werror
CFLAGS=-g \
  -pipe \
  -W \
  -Wall \
  -Werror
CPPFLAGS=-D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.0.0\"
INCPATH=-I. \
  -I./include \
  -I../include/
DEP_INCPATH=

#============ CCP vars ============
CCHECK=@ccheck.py
CCHECK_FLAGS=
PCLINT=@pclint
PCLINT_FLAGS=
CCP=@ccp.py
CCP_FLAGS=


#COMAKE UUID
COMAKE_MD5=b06407bb6d36990c7244cc42db61a5f9  COMAKE


.PHONY:all
all:comake2_makefile_check main .copy 
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mall[0m']"
	@echo "make all done"

.PHONY:comake2_makefile_check
comake2_makefile_check:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mcomake2_makefile_check[0m']"
	#in case of error, update 'Makefile' by 'comake2'
	@echo "$(COMAKE_MD5)">comake2.md5
	@md5sum -c --status comake2.md5
	@rm -f comake2.md5

.PHONY:ccpclean
ccpclean:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mccpclean[0m']"
	@echo "make ccpclean done"

.PHONY:clean
clean:ccpclean
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mclean[0m']"
	rm -rf main
	rm -rf ./output/bin/main
	rm -rf .copy
	rm -rf output
	rm -rf main_main.o
	rm -rf main_uthread.o

.PHONY:dist
dist:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mdist[0m']"
	tar czvf output.tar.gz output
	@echo "make dist done"

.PHONY:distclean
distclean:clean
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mdistclean[0m']"
	rm -f output.tar.gz
	@echo "make distclean done"

.PHONY:love
love:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlove[0m']"
	@echo "make love done"

main:main_main.o \
  main_uthread.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mmain[0m']"
	$(CXX) main_main.o \
  main_uthread.o -Xlinker "-("   -lpthread \
  -lcrypto \
  -lrt \
  -lssl -Xlinker "-)" -o main
	mkdir -p ./output/bin
	cp -f --link main ./output/bin

.copy:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.copy[0m']"
	mkdir -p output

main_main.o:main.c \
  uthread.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mmain_main.o[0m']"
	$(CC) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CFLAGS)  -o main_main.o main.c

main_uthread.o:uthread.c \
  uthread.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mmain_uthread.o[0m']"
	$(CC) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CFLAGS)  -o main_uthread.o uthread.c

endif #ifeq ($(shell uname -m),x86_64)


