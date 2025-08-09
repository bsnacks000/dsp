<CsoundSynthesizer>
<CsOptions>
-o ./test_signals/saw_tone.wav
</CsOptions>

<CsInstruments>
sr      = 48000
ksmps   = 32
nchnls  = 1
0dbfs   = 1

instr 1
    ; Fundamental frequency
    ifreq = 50
    iamp = 1.0

    a1 vco iamp, ifreq, 1, 1
    out a1
endin
</CsInstruments>

<CsScore>
f 1 0 65536 10 1    ; a sine wave.
i1 0  5
e
</CsScore>
</CsoundSynthesizer>
