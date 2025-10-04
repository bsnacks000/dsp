<CsoundSynthesizer>
<CsOptions>
    --opcode-lib=./build/csnd/libblpcsnd.so
    --opcode-lib=./build/csnd/libblpcsnd.dylib
    -t 60
</CsOptions>
<CsInstruments>

sr      = 48000
ksmps   = 64
nchnls  = 1
0dbfs   = 1

instr 1
    asig = 0
    out asig
endin


</CsInstruments>
<CsScore>
f1 0 8192 10 1
i1 0 1
</CsScore>
</CsoundSynthesizer>
