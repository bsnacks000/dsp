# regression tests 

The regression test suite uses the csound 7 API and works with the opcodes 
generated in the `csnd` build which are mainly proof of concept instruments or 
test implementations of the core modules.

### csds 

The csds serve as test cases. They should only test one opcode and one 
scenario at a time.

Csound is run in NRT mode and wav output is 32bit float

### wavs 

wav output from the runner. These can be listened back to but do not land 
in version control. 

### quantize 

A python script is used to quantize the output wav and compute a checksum. 

### checksums

Once a wav is computed and approved we cache a checksum which is then version 
controlled. 

### test 

The test runner recomputes checksums for each csd and compares it against its 
golden stored in checksums. If a hash has deviated significantly from what was 
originally set it is flagged and we investigate the issue.
