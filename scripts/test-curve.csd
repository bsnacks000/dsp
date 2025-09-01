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
    aramp ccurve p4, p3, p5, p6
    asig oscili aramp , 220, 1

    out asig * 0.7
endin;

</CsInstruments>
<CsScore>
    f1 0 8192 10 1
    ;i1 0 1      0.0 1.0 8.0
    ;i1 1 1      1.0 0.0 -8.0
    ;i1 2 1      0.0 1.0 -8.0
    ;i1 3 1      1.0 0.0 8.0
    ;i1 4 1      0.0 1.0 0.01
    ;i1 5 1      1.0 0.0 -0.01  ; boo

    i1 0 1  1.0 0.0 -0.1  ; ok i guess

</CsScore>
</CsoundSynthesizer>
