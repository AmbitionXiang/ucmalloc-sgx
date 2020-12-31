#
# Copyright (C) 2011-2020 Intel Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#   * Neither the name of Intel Corporation nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#

######## SGX SDK Settings ########

SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= HW
SGX_ARCH ?= x64
SGX_DEBUG ?= 1

include $(SGX_SDK)/buildenv.mk

ifeq ($(shell getconf LONG_BIT), 32)
	SGX_ARCH := x86
else ifeq ($(findstring -m32, $(CXXFLAGS)), -m32)
	SGX_ARCH := x86
endif

ifeq ($(SGX_ARCH), x86)
	SGX_COMMON_FLAGS := -m32
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x86/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x86/sgx_edger8r
else
	SGX_COMMON_FLAGS := -m64
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r
endif

ifeq ($(SGX_DEBUG), 1)
ifeq ($(SGX_PRERELEASE), 1)
$(error Cannot set SGX_DEBUG and SGX_PRERELEASE at the same time!!)
endif
endif

ifeq ($(SGX_DEBUG), 1)
        SGX_COMMON_FLAGS += -O0 -g
else
        SGX_COMMON_FLAGS += -O2
endif

SGX_COMMON_FLAGS += -Wall -Wextra -Winit-self -Wpointer-arith -Wreturn-type \
                    -Waddress -Wsequence-point -Wformat-security \
                    -Wmissing-include-dirs -Wfloat-equal -Wundef -Wshadow \
                    -Wcast-align -Wcast-qual -Wconversion -Wredundant-decls
SGX_COMMON_CXXFLAGS := $(SGX_COMMON_FLAGS) -Wnon-virtual-dtor -std=c++11

######## App Settings ########

ifneq ($(SGX_MODE), HW)
	Urts_Library_Name := sgx_urts_sim
else
	Urts_Library_Name := sgx_urts
endif

App_Cpp_Files := $(wildcard App/*.cpp) $(wildcard App/Edger8rSyntax/*.cpp) $(wildcard App/TrustedLibrary/*.cpp)
App_Include_Paths := -IInclude -IApp -I$(SGX_SDK)/include

App_C_Flags := -fPIC -Wno-attributes $(App_Include_Paths)

# Three configuration modes - Debug, prerelease, release
#   Debug - Macro DEBUG enabled.
#   Prerelease - Macro NDEBUG and EDEBUG enabled.
#   Release - Macro NDEBUG enabled.
ifeq ($(SGX_DEBUG), 1)
        App_C_Flags += -DDEBUG -UNDEBUG -UEDEBUG
else ifeq ($(SGX_PRERELEASE), 1)
        App_C_Flags += -DNDEBUG -DEDEBUG -UDEBUG
else
        App_C_Flags += -DNDEBUG -UEDEBUG -UDEBUG
endif

App_Cpp_Flags := $(App_C_Flags)
App_Link_Flags := -L$(SGX_LIBRARY_PATH) -l$(Urts_Library_Name) -lpthread 

App_Cpp_Objects := $(App_Cpp_Files:.cpp=.o)

App_Name := app

######## Enclave Settings ########

ifneq ($(SGX_MODE), HW)
	Trts_Library_Name := sgx_trts_sim
	Service_Library_Name := sgx_tservice_sim
else
	Trts_Library_Name := sgx_trts
	Service_Library_Name := sgx_tservice
endif
Crypto_Library_Name := sgx_tcrypto

Enclave_Cpp_Files := Enclave/Enclave.cpp $(wildcard Enclave/Edger8rSyntax/*.cpp) $(wildcard Enclave/TrustedLibrary/*.cpp)
Enclave_Include_Paths := -IInclude -IEnclave -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx

Enclave_C_Flags := $(Enclave_Include_Paths) -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections $(MITIGATION_CFLAGS)
CC_BELOW_4_9 := $(shell expr "`$(CC) -dumpversion`" \< "4.9")
ifeq ($(CC_BELOW_4_9), 1)
	Enclave_C_Flags += -fstack-protector
else
	Enclave_C_Flags += -fstack-protector-strong
endif

Enclave_Cpp_Flags := $(Enclave_C_Flags) -nostdinc++

# Enable the security flags
Enclave_Security_Link_Flags := -Wl,-z,relro,-z,now,-z,noexecstack

# To generate a proper enclave, it is recommended to follow below guideline to link the trusted libraries:
#    1. Link sgx_trts with the `--whole-archive' and `--no-whole-archive' options,
#       so that the whole content of trts is included in the enclave.
#    2. For other libraries, you just need to pull the required symbols.
#       Use `--start-group' and `--end-group' to link these libraries.
# Do NOT move the libraries linked with `--start-group' and `--end-group' within `--whole-archive' and `--no-whole-archive' options.
# Otherwise, you may get some undesirable errors.
Enclave_Link_Flags := $(Enclave_Security_Link_Flags) \
    -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_TRUSTED_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--whole-archive -lsgx_tcmalloc -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections   \
	-Wl,--version-script=Enclave/Enclave.lds

Enclave_Cpp_Objects := $(sort $(Enclave_Cpp_Files:.cpp=.o))

Enclave_Name := enclave.so
Signed_Enclave_Name := enclave.signed.so
Enclave_Config_File := Enclave/Enclave.config.xml

TCMALLOC_Default_Include_Paths := -I./Enclave/gperftools/gperftools-2.5/src
TCMALLOC_Include_Paths := $(TCMALLOC_Default_Include_Paths) -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx -IInclude -IEnclave

am__append_2 = -Wall -Wwrite-strings -Woverloaded-virtual

am__append_3 = -fno-builtin -fno-builtin-function
am__append_5 = -Wno-unused-result
am__append_8 = -DNO_FRAME_POINTER

TCMALLOC_CFlags := -Wall -DHAVE_CONFIG_H -DNO_TCMALLOC_SAMPLES -DNDEBUG -DNO_HEAP_CHECK -DTCMALLOC_SGX -DTCMALLOC_NO_ALIASES -fstack-protector -ffreestanding -nostdinc -fvisibility=hidden -fPIC $(am__append_2) $(am__append_3) $(am__append_5) $(am__append_8)

SYSTEM_ALLOC_CC = ./Enclave/gperftools/gperftools-2.5/src/system-alloc.cc
libtcmalloc_minimal_internal_la_SOURCES = ./Enclave/gperftools/gperftools-2.5/src/common.cc \
                                          ./Enclave/gperftools/gperftools-2.5/src/internal_logging.cc \
                                          $(SYSTEM_ALLOC_CC) \
                                          ./Enclave/gperftools/gperftools-2.5/src/central_freelist.cc \
                                          ./Enclave/gperftools/gperftools-2.5/src/page_heap.cc \
                                          ./Enclave/gperftools/gperftools-2.5/src/sampler.cc \
                                          ./Enclave/gperftools/gperftools-2.5/src/span.cc \
                                          ./Enclave/gperftools/gperftools-2.5/src/stack_trace_table.cc \
                                          ./Enclave/gperftools/gperftools-2.5/src/static_vars.cc \
                                          ./Enclave/gperftools/gperftools-2.5/src/symbolize.cc \
                                          ./Enclave/gperftools/gperftools-2.5/src/thread_cache.cc \
                                          ./Enclave/gperftools/gperftools-2.5/src/malloc_hook.cc \
                                          ./Enclave/gperftools/gperftools-2.5/src/maybe_threads.cc \
										  ./Enclave/gperftools/gperftools-2.5/src/malloc_extension.cc \
										  ./Enclave/gperftools/gperftools-2.5/src/tcmalloc.cc

TCMALLOC_Objects := $(libtcmalloc_minimal_internal_la_SOURCES:.cc=.o) Enclave/gperftools/gperftools-2.5/src/base/spinlock.o Enclave/gperftools/gperftools-2.5/src/base/sgx_utils.o

ifeq ($(SGX_MODE), HW)
ifeq ($(SGX_DEBUG), 1)
	Build_Mode = HW_DEBUG
else ifeq ($(SGX_PRERELEASE), 1)
	Build_Mode = HW_PRERELEASE
else
	Build_Mode = HW_RELEASE
endif
else
ifeq ($(SGX_DEBUG), 1)
	Build_Mode = SIM_DEBUG
else ifeq ($(SGX_PRERELEASE), 1)
	Build_Mode = SIM_PRERELEASE
else
	Build_Mode = SIM_RELEASE
endif
endif


.PHONY: all target run
all: .config_$(Build_Mode)_$(SGX_ARCH)
	@$(MAKE) target

ifeq ($(Build_Mode), HW_RELEASE)
target:  $(App_Name) $(Enclave_Name)
	@echo "The project has been built in release hardware mode."
	@echo "Please sign the $(Enclave_Name) first with your signing key before you run the $(App_Name) to launch and access the enclave."
	@echo "To sign the enclave use the command:"
	@echo "   $(SGX_ENCLAVE_SIGNER) sign -key <your key> -enclave $(Enclave_Name) -out <$(Signed_Enclave_Name)> -config $(Enclave_Config_File)"
	@echo "You can also sign the enclave using an external signing tool."
	@echo "To build the project in simulation mode set SGX_MODE=SIM. To build the project in prerelease mode set SGX_PRERELEASE=1 and SGX_MODE=HW."


else
target: $(App_Name) $(Signed_Enclave_Name)
ifeq ($(Build_Mode), HW_DEBUG)
	@echo "The project has been built in debug hardware mode."
else ifeq ($(Build_Mode), SIM_DEBUG)
	@echo "The project has been built in debug simulation mode."
else ifeq ($(Build_Mode), HW_PRERELEASE)
	@echo "The project has been built in pre-release hardware mode."
else ifeq ($(Build_Mode), SIM_PRERELEASE)
	@echo "The project has been built in pre-release simulation mode."
else
	@echo "The project has been built in release simulation mode."
endif

endif

run: all
ifneq ($(Build_Mode), HW_RELEASE)
	@$(CURDIR)/$(App_Name)
	@echo "RUN  =>  $(App_Name) [$(SGX_MODE)|$(SGX_ARCH), OK]"
endif

.config_$(Build_Mode)_$(SGX_ARCH):
	@rm -f .config_* $(App_Name) $(Enclave_Name) $(Signed_Enclave_Name) $(App_Cpp_Objects) App/Enclave_u.* $(Enclave_Cpp_Objects) Enclave/Enclave_t.*
	@touch .config_$(Build_Mode)_$(SGX_ARCH)

######## App Objects ########

App/Enclave_u.h: $(SGX_EDGER8R) Enclave/Enclave.edl
	@cd App && $(SGX_EDGER8R) --untrusted ../Enclave/Enclave.edl --search-path ../Enclave --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

App/Enclave_u.c: App/Enclave_u.h

App/Enclave_u.o: App/Enclave_u.c
	@$(CC) $(SGX_COMMON_CFLAGS) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

App/%.o: App/%.cpp  App/Enclave_u.h
	@$(CXX) $(SGX_COMMON_CXXFLAGS) $(App_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(App_Name): App/Enclave_u.o $(App_Cpp_Objects)
	@$(CXX) $^ -o $@ $(App_Link_Flags)
	@echo "LINK =>  $@"

######## Enclave Objects ########

Enclave/Enclave_t.h: $(SGX_EDGER8R) Enclave/Enclave.edl
	@cd Enclave && $(SGX_EDGER8R) --trusted ../Enclave/Enclave.edl --search-path ../Enclave --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Enclave/Enclave_t.c: Enclave/Enclave_t.h

Enclave/Enclave_t.o: Enclave/Enclave_t.c
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave/%.o: Enclave/%.cpp Enclave/Enclave_t.h
	@$(CXX) $(SGX_COMMON_CXXFLAGS) $(Enclave_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

#For TCMALLOC
Enclave/gperftools/gperftools-2.5/src/base/sgx_utils.o: Enclave/gperftools/gperftools-2.5/src/base/sgx_utils.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/base/spinlock.o: Enclave/gperftools/gperftools-2.5/src/base/spinlock.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/maybe_threads.o: Enclave/gperftools/gperftools-2.5/src/maybe_threads.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"


Enclave/gperftools/gperftools-2.5/src/common.o: Enclave/gperftools/gperftools-2.5/src/common.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/internal_logging.o: Enclave/gperftools/gperftools-2.5/src/internal_logging.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@ 
	@echo "CXX  <= $<"

#OCALL is needed in system-alloc.cc
Enclave/gperftools/gperftools-2.5/src/system-alloc.o: Enclave/gperftools/gperftools-2.5/src/system-alloc.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(Enclave_Cpp_Flags) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/central_freelist.o: Enclave/gperftools/gperftools-2.5/src/central_freelist.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/page_heap.o: Enclave/gperftools/gperftools-2.5/src/page_heap.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/sampler.o: Enclave/gperftools/gperftools-2.5/src/sampler.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/span.o: Enclave/gperftools/gperftools-2.5/src/span.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/stack_trace_table.o: Enclave/gperftools/gperftools-2.5/src/stack_trace_table.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/static_vars.o: Enclave/gperftools/gperftools-2.5/src/static_vars.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/symbolize.o: Enclave/gperftools/gperftools-2.5/src/symbolize.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/thread_cache.o: Enclave/gperftools/gperftools-2.5/src/thread_cache.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/malloc_hook.o: Enclave/gperftools/gperftools-2.5/src/malloc_hook.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/malloc_extension.o: Enclave/gperftools/gperftools-2.5/src/malloc_extension.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

Enclave/gperftools/gperftools-2.5/src/tcmalloc.o: Enclave/gperftools/gperftools-2.5/src/tcmalloc.cc
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $(TCMALLOC_Include_Paths) -c $< -o $@
	@echo "CXX  <= $<"

$(Enclave_Name): Enclave/Enclave_t.o $(Enclave_Cpp_Objects) $(TCMALLOC_Objects)
	@$(CXX) $(SGX_COMMON_CFLAGS) $(TCMALLOC_CFlags) $^ -o $@ $(Enclave_Link_Flags)
	@echo "LINK =>  $@"

$(Signed_Enclave_Name): $(Enclave_Name) 
	@$(SGX_ENCLAVE_SIGNER) sign -key Enclave/Enclave_private_test.pem -enclave $(Enclave_Name) -out $@ -config $(Enclave_Config_File)
	@echo "SIGN =>  $@"

.PHONY: clean

clean:
	@rm -f .config_* $(App_Name) $(Enclave_Name) $(Signed_Enclave_Name) $(App_Cpp_Objects) App/Enclave_u.* $(Enclave_Cpp_Objects) Enclave/Enclave_t.*
