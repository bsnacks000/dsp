<CsoundSynthesizer>
<CsOptions>
-o ./test_signals/gbuzz_tone.wav
</CsOptions>

<CsInstruments>
sr      = 48000
ksmps   = 32
nchnls  = 1
0dbfs   = 1

instr 1
    ; Fundamental frequency
    ifreq = 50

    ; Create a harmonically rich buzz tone
    ; gbuzz(amp, freq, nharm, lharm, rharm, mul)
    a1 gbuzz 0.4, ifreq, 100, 1, 1, 1

    out a1
endin
</CsInstruments>

<CsScore>
f 1 0 16384 11 1
i1 0  5
e
</CsScore>
</CsoundSynthesizer>
