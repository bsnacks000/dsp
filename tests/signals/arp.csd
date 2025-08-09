<CsoundSynthesizer>
<CsOptions>
; Write to a mono WAV file
-o ./test_signals/arp.wav
</CsOptions>
<CsInstruments>

; This is a simple arpeggiation with short pulses.
; Useful for testing echoes

sr = 44100
ksmps = 32
nchnls = 1
0dbfs = 1

instr 1
    iFreq = p4
    iDur = p3
    aEnv  linen 0.3, 0.01, iDur, 0.05
    aSig  oscili aEnv, iFreq
    out   aSig
endin

instr 99
    ; Silence generator
    aZero = 0
    out aZero
endin

</CsInstruments>
<CsScore>

;           dur     freq
i1  0        0.1     261.63  ; C4
i1  0.5      .       311.13  ; Eb4
i1  1        .       392.00  ; G4
i1  1.5      .       523.25  ; C5
i1  2.0      .       1046.50  ; C4

; leave time to test the delay line decay in our project
i99 3.0   5.0         ; 5 seconds of silence
e
</CsScore>
</CsoundSynthesizer>
