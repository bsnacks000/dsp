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


instr 1;
    aramp lline p4, p3, p5
    asig oscili 0.5, aramp, 1
    out asig
endin;

</CsInstruments>
<CsScore>
    f1 0 8192 10 1
    i1 0 5      50 1000
    i1 5 10     1000 50


</CsScore>
</CsoundSynthesizer>
