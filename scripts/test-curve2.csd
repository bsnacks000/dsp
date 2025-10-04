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
    ; p4 = start, p5 = stop, p6 = factor
    aramp ccurve p4, p3, p5, p6
    asig oscili aramp, 220, 1
    out asig * 0.2
endin
</CsInstruments>
<CsScore>
    f1 0 8192 10 1

    ; Durations to test: 1.0, 0.1, 0.01
    ; Factors to test:   -8, -0.1, -0.01, 0.01, 0.1, 8

    ; start=1.0 → stop=0.0
    i1 0.0   1.0   1.0 0.0 -8.0
    i1 2.0   1.0   1.0 0.0 -0.1
    i1 4.0   1.0   1.0 0.0 -0.01    ; fail
    i1 6.0   1.0   1.0 0.0  0.01    ; fail
    i1 8.0   1.0   1.0 0.0  0.1     ; fail
    i1 10.0  1.0   1.0 0.0  8.0

    ; shorter 100ms segments
    i1 12.0  0.1   1.0 0.0 -8.0
    i1 14.0  0.1   1.0 0.0 -0.1
    i1 16.0  0.1   1.0 0.0 -0.01
    i1 18.0  0.1   1.0 0.0  0.01
    i1 20.0  0.1   1.0 0.0  0.1
    i1 22.0  0.1   1.0 0.0  8.0

    ; very short 10ms segments
    i1 24.0  0.01  1.0 0.0 -8.0
    i1 26.0  0.01  1.0 0.0 -0.1
    i1 28.0  0.01  1.0 0.0 -0.01
    i1 30.0  0.01  1.0 0.0  0.01
    i1 32.0  0.01  1.0 0.0  0.1
    i1 34.0  0.01  1.0 0.0  8.0
</CsScore>
</CsoundSynthesizer>
