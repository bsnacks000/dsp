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

; note that --opcode-lib and other csnd options set at command line

instr 1 ; mult
    ; test your audio maths...
    a1 oscili 0.5, 440
    a2 oscili 0.5, 356
    a3 mult a1, a2
    out a3
endin

instr 4 ; ddelay2 test static
    ; test input
    aenv linseg 1, 0.05, 0
    asig oscili 0.4 * aenv, 440

    kdel init p4
    kfb  init p5

    adel = kdel         ; milliseconds
    afb  = kfb          ; 0 - 1
    idelmax = 1.0

    aout ddelay2 asig, adel, afb, idelmax
    out aout * 0.5 + asig * 0.5
endin;

instr 5 ; ddelay2 test modulation
    ; test input
    aenv linseg 1, 0.05, 0
    asig oscili 0.4 * aenv, 440

    adel linseg p4, p3, p5
    afb linseg p6, p3, p7
    idelmax = 1.0

    aout ddelay2 asig, adel, afb, idelmax
    out aout * 0.5 + asig * 0.5
endin;




</CsInstruments>
<CsScore>

    i4 0 2  200 0.6
    i5 2 5  100 800 0.7 0.5

</CsScore>
</CsoundSynthesizer>
