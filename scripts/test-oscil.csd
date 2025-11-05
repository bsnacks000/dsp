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

; TODO break down into multiple test modules

instr 1 ; test ftoscil3
    afreq   line 1, p3, 18000
    aout ftoscil3 afreq, 0, p4

    out aout * 0.75
endin


instr 2 ; test oftoscil3
    afreq   line 1, p3, 12000
    aout oftoscil3 afreq, 0, p4, p5

    out aout * 0.75
endin

instr 3 ; test ftoscil3
    afreq init 0
    afreq = 120.0
    aout ftoscil3 afreq, 0, p4

    out aout * 0.75
endin


instr 4 ; neg freq test
    afreq line 1000, p3, -1000
    aout ftoscil3 afreq, 0, 1
    out aout * 0.75
endin

</CsInstruments>
<CsScore>
f1 0 8192 10 1; sine
f2 0 8192 10 1 0 .333 0 .2 0 .143 0 .111 0 .0909 0 .077 0 .0666 0 .0588 ; sqr
f3 0 8192 10 1 0.5 0.3333 0.25 0.2 0.1666 0.1428 0.125 0.1111 0.1; saw
f4 0 8192 10 1 0 0.1111 0 0.04 0 0.0204 0 0.0123 0; tri
f5 0 8192 10 10 0 0 0 0 1 1 1 1 1 1; buzz
;
;i1 0 3 1
;i1 0 3 2 ; mmmm... such aliasing!
;i1 6 3 3
;i1 9 3 4
;i1 12 3 5

;i2 0 5 2 8
;i2 0 3 2 8

;i3 0 60 4;

i4 0 3;

</CsScore>
</CsoundSynthesizer>
