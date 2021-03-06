
MPIVERSION = $(shell mpirun --version 2>&1| head -n 1)
ifneq (,$(findstring Open MPI,$(MPIVERSION)))
    SMILEICXX ?= mpicxx
else
    SMILEICXX ?= mpiicpc
endif


HDF5_ROOT_DIR ?=

BUILD_DIR ?= build

PYTHONCONFIG ?= python-config

EXEC = smilei

default: $(EXEC)

####################################################
DESCRIBE:=$(shell git describe 2>/dev/null || echo '??')
BRANCH:=$(shell git rev-parse --abbrev-ref HEAD 2>/dev/null || echo '??')
VERSION="$(DESCRIBE)-$(BRANCH)"
COMMITDATE:="$(shell git show -s --pretty="%ci" 2>/dev/null || echo '??')"

CXXFLAGS += -D__VERSION=\"$(VERSION)\" -D__COMMITDATE=\"$(COMMITDATE)\" -D__CONFIG=\""$(config)"\"

CXXFLAGS += -std=c++0x 
ifneq ($(strip $(HDF5_ROOT_DIR)),)
CXXFLAGS += -I${HDF5_ROOT_DIR}/include 
LDFLAGS += -L${HDF5_ROOT_DIR}/lib 
endif
LDFLAGS += -lhdf5 



ifneq (,$(findstring poincare,$(HOSTNAME)))
    LDFLAGS += -lgpfs -lz -L/gpfslocal/pub/python/anaconda/Anaconda-2.1.0/lib
endif

#add subdirs
DIRS := $(shell find src -type d)
#add include directives for subdirs
CXXFLAGS += $(DIRS:%=-I%)

#collect all cpp files
SRCS := $(shell find src/* -name \*.cpp)
OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))
DEPS := $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.d))
PYSCRIPTS := $(shell find src/Python -name \*.py)
CXXFLAGS += -I$(BUILD_DIR)/src/Python
PYHEADERS := $(addprefix $(BUILD_DIR)/, $(PYSCRIPTS:.py=.pyh))

PY_CXXFLAGS:=$(shell $(PYTHONCONFIG) --includes)
CXXFLAGS+=$(PY_CXXFLAGS)


ifneq ($(strip $(PYTHONHOME)),)
LDFLAGS+=-L$(PYTHONHOME)/lib
endif 

PY_LDFLAGS:=$(shell $(PYTHONCONFIG) --ldflags)
LDFLAGS+=$(PY_LDFLAGS)


# check for variable config
ifneq (,$(findstring debug,$(config)))
	CXXFLAGS += -g -pg -Wall -D__DEBUG -O0 # -shared-intel 
else
	CXXFLAGS += -O3 # -xHost -ipo
	SPHINXOPTS = '-W'
endif

ifneq (,$(findstring scalasca,$(config)))
    SMILEICXX = scalasca -instrument $(SMILEICXX)
endif

ifneq (,$(findstring turing,$(config)))
	CXXFLAGS += -I$(BG_PYTHONHOME)/include/python2.7 -qlanglvl=extended0x
	LDFLAGS  += -qnostaticlink -L(BG_PYTHONHOME)/lib64 -lpython2.7 -lutil
endif

ifeq (,$(findstring noopenmp,$(config)))
	SMILEI_COMPILER:=$(shell $(SMILEICXX) --version 2>&1|head -n 1)
    ifneq (,$(findstring icpc,$(SMILEI_COMPILER)))
        OPENMPFLAGS = -openmp
    else
        OPENMPFLAGS = -fopenmp 
	LDFLAGS += -lm
    endif
    OPENMPFLAGS += -D_OMP
    LDFLAGS += $(OPENMPFLAGS)
    #LDFLAGS += -mt_mpi
    CXXFLAGS += $(OPENMPFLAGS)
endif

clean:
	rm -f $(OBJS) $(DEPS) $(PYHEADERS)
	rm -rf $(BUILD_DIR) 
	rm -rf smilei-$(VERSION).tgz
	make -C doc clean

distclean: clean
	rm -f $(EXEC)

env:
	echo "$(MPIVERSION)"

# this generates a .h file containing a char[] with the python script in binary then
# you can just include this file to get the contents (in Params/Params.cpp)
$(BUILD_DIR)/%.pyh: %.py
	@ if [ ! -d "$(@D)" ]; then mkdir -p "$(@D)"; fi;
	@ echo "Creating binary char for $< : $@"
	@ cd "$(<D)" && xxd -i "$(<F)" > "$(@F)"
	@ mv "$(<D)/$(@F)" "$@"

$(BUILD_DIR)/%.d: %.cpp
	@ if [ ! -d "$(@D)" ]; then mkdir -p "$(@D)"; fi;
	@ echo "Checking dependencies for $<"
# create and modify dependecy file .d to take into account the location subdir
	@ $(SMILEICXX) $(CXXFLAGS) -MM $< 2>/dev/null | sed -e "s@\(^.*\)\.o:@$(BUILD_DIR)/$(shell  dirname $<)/\1.d $(BUILD_DIR)/$(shell  dirname $<)/\1.o:@" > $@  

$(BUILD_DIR)/%.o : %.cpp
	$(SMILEICXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(SMILEICXX) $(OBJS) -o $(BUILD_DIR)/$@ $(LDFLAGS) 
	cp $(BUILD_DIR)/$@ $@

# these are kept for backward compatibility and might be removed (see make help)
obsolete:
	@ echo "[WARNING] Please consider using make config=\"$(MAKECMDGOALS)\""

debug: obsolete
	make config=debug

scalasca: obsolete
	make config=scalasca


ifeq ($(filter pygenerator doc help clean default tar sphinx,$(MAKECMDGOALS)),) 
# Let's try to make the next lines clear: we include $(DEPS) and pygenerator
-include $(DEPS) pygenerator
# and pygenerator will create all the $(PYHEADERS) (which are files)
pygenerator : $(PYHEADERS)
endif


.PHONY: pygenerator doc help clean default tar sphinx

doc:
	make -C doc all

sphinx:
	make SPHINXOPTS=${SPHINXOPTS} -C doc/Sphinx html
	
tar:
	git archive -o smilei-$(VERSION).tgz --prefix smilei-$(VERSION)/ HEAD

help: 
	@echo 'Usage: make config=OPTIONS'
	@echo '	    OPTIONS is a string composed of one or more of:'
	@echo '	        debug      : to compile in debug mode (code runs really slow)'
	@echo '         scalasca   : to compile using scalasca'
	@echo '         noopenmp   : to compile without openmp'
	@echo ' examples:'
	@echo '     make config=debug'
	@echo '     make config=noopenmp'
	@echo '     make config="debug noopenmp"'
	@echo ''
	@echo 'Environment variables :'
	@echo "     SMILEICXX     : mpi c++ compiler [${SMILEICXX}]"
	@echo "     HDF5_ROOT_DIR : HDF5 dir [${HDF5_ROOT_DIR}]"
	@echo "     BUILD_DIR     : directory used to store build files [${BUILD_DIR}]"
	@echo 
	@echo 'Other commands :'
	@echo '     make doc     : builds the documentation'
	@echo '     make tar     : creates an archive of the sources'
	@echo '     make clean   : remove build'
	@echo 
	@echo 'http://www.maisondelasimulation.fr/smilei'

