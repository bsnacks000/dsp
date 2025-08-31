<CsoundSynthesizer>
<CsOptions>
    --opcode-lib=./build/csnd/libblpcsnd.so
    --opcode-lib=./build/csnd/libblpcsnd.dylib
    -t 60
</CsOptions>
<CsInstruments>


; NOTE due to issue with gen01 args
; we can't dynamically set this internally (gen01args not populating)
; would need to use *filesr* opcode and pass in at runtime.

sr      = 44100
ksmps   = 64
nchnls  = 1
0dbfs   = 1

gift ftgen 0, 0, 0, 1, "almostjungle1.wav", 0, 0, 1

instr 1 ; test sampler
    aspd line 2.0, p3, -2.0
    aout sampler aspd, 0, gift
    out aout * 0.75
endin


</CsInstruments>
<CsScore>
i1 0 10;

</CsScore>
</CsoundSynthesizer>
