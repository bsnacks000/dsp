<CsoundSynthesizer>
<CsOptions>
-o ./test_signals/saw_ramps.wav
</CsOptions>

<CsInstruments>
sr      = 48000
ksmps   = 32
nchnls  = 1
0dbfs   = 1

; gradually rising slopes on low freq saw
; for testing env follower (filter fc)

instr 1
    idur = p3
    kamp = p4
    irise = p5
    idec = p6
    ifreq = p7

    aenv  linen kamp, irise, idur, idec
    a1 vco aenv, ifreq, 1, 1
    out a1
endin
</CsInstruments>

<CsScore>
f 1 0 65536 10 1    ; a sine wave.

i1 0    0.5     0.3 0.01 0.1    50
i1 1.5  1.0     0.5 0.1 0.2     75
i1 2.5  1.5     0.7 0.35 0.5    100
i1 4    3.0     0.9 1.0 0.5    120

e
</CsScore>
</CsoundSynthesizer>
