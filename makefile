# BSD 3-Clause License
# 
# Copyright (c) 2022, Vayavya Labs Pvt. Ltd.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
ifeq ($(SYSTEMC_HOME),)
   $(error SYSTEMC_HOME variable is not set)
endif
ifeq ($(SYSTEMC_ARCH),)
   $(error SYSTEMC_ARCH variable is not set)
endif

SYSTEMC_LIB_PATH = $(SYSTEMC_HOME)/lib-$(SYSTEMC_ARCH)
SYSTEMC_INC = $(SYSTEMC_HOME)/include




CXX=g++
CC=gcc
LD=g++

INCLUDES = -I $(SYSTEMC_INC) -I ./src -I ./test
CC_FLAGS=-Wall -Werror -DSC_INCLUDE_DYNAMIC_PROCESSES -g -O0
LD_FLAGS+=-Wl,-rpath=$(SYSTEMC_LIB_PATH)
LIBS= -lsystemc -lm
LIB_PATH= -L $(SYSTEMC_LIB_PATH)

EXE = sim.x

SRCS = $(wildcard src/*.cpp)
SRCS += $(wildcard test/*.cpp)

$(EXE): $(SRCS) $(wildcard src/*.h) $(wildcard test/*.h)
	$(CXX) $(CC_FLAGS) $(INCLUDES) $(LD_FLAGS) $(SRCS) $(LIB_PATH) $(LIBS) -o $(EXE)

clean:
	rm -f $(EXE)
