#
# The following parameters can be defined:
# ROOT: the path to the Propeller GCC folder (propgcc, most likely)
# MODEL: the memory model to use.
# DEFINEFLAGS: ... ???

# If Unit tests are being run, the #define UNIT_TEST flag is set.

ifndef MODEL
MODEL = cmm
endif

#If no root given, make best guess
ifndef ROOT
ROOT = ../../
#ROOT=$(shell root=$$(pwd); while [ "$$(basename $$root)" != "libpropeller" ]; do root=$$(dirname $$root); done; echo $$root"/")
endif

ifndef DEFINEFLAGS
DEFINEFLAGS =
endif

#Note the "string" version of *.cpp: that's so that the gcc compiler evaluates
#the wildcard, instead of make. This is because unit testing generates a .cpp
#file with the ruby command, and wildcard evaluates before it's generated.
#Have to do the wildcard *.S because otherwise gcc complains if there are not
#any .S files in the directory (if you pass it a *.S).
SRCS=*.cpp $(wildcard *.S) #$(wildcard *.cpp) $(wildcard *.S)
OBJS=$(subst .cpp,.o,$(SRCS))

UNITYPATH = ${ROOT}libpropeller/unity_tools/
BOARDCONFIGURATIONPATH = ${ROOT}libpropeller/board/

INCLUDE = -I${UNITYPATH} -I${BOARDCONFIGURATIONPATH} -I../ -I ../../ -I${ROOT}

#-Wa,-alh,-L  == output to STDOUT the generated assembly, with C interspersed



DEBUGFLAGS = -Wall -fverbose-asm #-Wa,-alhm,-L#-pedantic #-fstack-usage #-save-temps #-ftime-report -fmem-report #-Q

CXX = /opt/parallax/bin/propeller-elf-g++
CLD = propeller-elf-ld
#-Wall


INLINEFLAGS = #-fno-default-inline

OPTIMIZEFLAGS = -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-rtti -m32bit-doubles -mfcache -fpermissive
CXXFLAGS = -m${MODEL} ${INCLUDE} ${DEBUGFLAGS} ${DEFINEFLAGS} ${OPTIMIZEFLAGS} ${INLINEFLAGS} -std=gnu++0x -fno-strict-aliasing 
CLDFLAGS = -Wl,--gc-sections #-Wl,--print-gc-sections #-Wl,--verbose #-Wl,--script=${GCCUTILITIESPATH}main.ld 

Unit:
	ruby ${UNITYPATH}generate_test_runner.rb
	${CXX} ${CXXFLAGS} ${CLDFLAGS} -o main.elf -D UNIT_TEST ${SRCS} ${UNITYPATH}unity.cpp
	rm -f *.generate.*

#Debug:
#	${CXX} ${CXXFLAGS} ${CLDFLAGS} -o main.elf ${SRCS} 
	



#Advanced makeflie stuff Taken from http://mad-scientist.net/make/autodep.html#output

#DEPDIR = dependencies
#df = $(DEPDIR)/$(*F)

#OUT_DIR=bin

#SOURCES=$(wildcard *.cpp)
#OBJECTS=
#OBJECTS=$(patsubst %, $(OUT_DIR)/%,$(_OBJECTS))
#EXECUTABLE=main.elf



#Debug: $(SOURCES) $(EXECUTABLE)
#	
#$(EXECUTABLE): $(OBJECTS) 
#	$(CXX) $(CLDFLAGS) ${CXXFLAGS} $(OBJECTS) -o $@

#%.o : %.cpp
#	${CXX} ${CXXFLAGS} -MD -c -o $@ $<
#	cp $(df).d $(df).P; \
#		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
#			-e '/^$$/ d' -e 's/$$/ :/' < $(df).d >> $(df).P; \
#		rm -f $(df).d
#-include $(SRCS:%.c=$(DEPDIR)/%.P)

#%.o : %.cpp
#	@${CXX} ${CXXFLAGS} -Wp,-MD,${df}.d -c -o $@ $<
#	@cp ${df}.d ${df}.P; \
#		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
#			-e '/^$$/ d' -e 's/$$/ :/' < ${df}.d >> ${df}.P; \
#		rm -f ${df}.d

#-include *.P



cleanDebug: clean

clean:
	${RM} *.o *.elf *.map *.rawmap a.out *.generate.* *.d *.P


