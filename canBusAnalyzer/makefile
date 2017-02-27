###### CONFIGURATION ######

### List of applications to be build
APPLICATIONS = all

### Note: to override the search path for the xeno-config script, use "make XENO=..."


### Note: to override the kernel source path, use "make KSRC=..."



###### USER SPACE BUILD (no change required normally) ######
ifeq ($(KERNELRELEASE),)
ifneq ($(APPLICATIONS),)

### Default Xenomai installation path
XENO ?= /usr/xenomai

XENOCONFIG=$(shell PATH=$(XENO):$(XENO)/bin:$(PATH) which xeno-config 2>/dev/null)
### Sanity check
ifeq ($(XENOCONFIG),)
all::
	@echo ">>> Invoke make  like this: \"make XENO=/path/to/xeno-config\" <<<"
	@echo
endif
INCPATH =./include
CFLAGS = -g -Wall -I$(INCPATH)

CC=$(shell $(XENOCONFIG) --cc)

CFLAGS+=$(shell $(XENOCONFIG) --skin=native --cflags) $(MY_CFLAGS)

LDFLAGS=$(MY_LDFLAGS)
LDLIBS=$(shell $(XENOCONFIG) --skin=native --ldflags) \
	$(shell $(XENOCONFIG) --skin=rtdm --ldflags)

# This includes the library path of given Xenomai into the binary to make live
# easier for beginners if Xenomai's libs are not in any default search path.
LDFLAGS+=-Xlinker -rpath -Xlinker $(shell $(XENOCONFIG) --libdir)
INSTPATH= ./bin/canBatEXE
OBJPATH= ./obj
COND1='stat app 2>/dev/null | grep Modify'
COND2='stat $(INSTPATH) 2>/dev/null |grep Modify'


all: getobj canBatEXE install putobj

canBatEXE: main.o canfr_canconfig.o canfr_canrecv.o
		$(CC) -o $@ main.o canfr_canconfig.o canfr_canrecv.o  $(CFLAGS) $(LDFLAGS) $(LDLIBS)

main.o: main.c $(INCPATH)/canfrMsg.h $(INCPATH)/canfr_typedef.h $(INCPATH)/canfr_canconfig.h $(INCPATH)/canfr_canrecv.h 
		$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -c main.c 

canfr_canconfig.o: canfr_canconfig.c $(INCPATH)/canfrMsg.h  $(INCPATH)/canfr_typedef.h $(INCPATH)/canfr_canconfig.h $(INCPATH)/printActivation.h
		$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -c canfr_canconfig.c
		
canfr_canrecv.o: canfr_canrecv.c $(INCPATH)/canfrMsg.h  $(INCPATH)/canfr_typedef.h $(INCPATH)/canfr_canrecv.h $(INCPATH)/printActivation.h
		$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -c canfr_canrecv.c

getobj:
		-mv $(OBJPATH)/*.o . 2>/dev/null
		
putobj:
		-mv *.o $(OBJPATH) 2>/dev/null
		
#process only when app timestamp is changed
install:
		@if [ "$(COND1)" != "$(COND2)" ];\
		then\
				cp -p ./canBatEXE $(INSTPATH) 2>/dev/null;\
				rm ./canBatEXE;\
				chmod 777 $(INSTPATH);\
		fi
# this one is used for housekeeping
clean:
		-rm -f app
		-rm -f $(OBJPATH)/*.o
		-rm -f $(INSTPATH)


endif
endif
