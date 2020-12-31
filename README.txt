------------------------
Sample for allocating and managing untrusted memory inside an enclave

There are two versions for both C/C++ and Rust

Version for C/C++ can run directly, with Intel SGX SDK 2.12

Version for Rust should run after mv the work directory 'rust_version' under incubator-teaclave-sgx-sdk/samplecode, otherwise you should revise related path for rust-sgx-sdk in Makefile

------------------------
Note: Set the enclave HeapMaxSize equal or larger than 0x900000 in Enclave.config.xml. For example:
   <HeapMaxSize>0x900000</HeapMaxSize>

------------------------
Enclave/gperftools is revised by T.Kim

It is based on google gperftools-2.5

Related paper: "ShieldStore: Shielded In-memory Key-value Storage with SGX" EuroSys'19


