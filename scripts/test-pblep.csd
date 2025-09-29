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
    afreq line 1, p3, p4
    aout blepsaw afreq, 0
    out aout * 0.75
endin



instr 2
    afreq line 1, p3, p4
    aduty = 0.5
    aout blepsqr afreq, aduty, 0
    out aout * 0.75
endin



instr 3
    afreq = 100
    aduty line 0.1, p3, 0.9
    aout blepsqr afreq, aduty, 0
    out aout * 0.75
endin


</CsInstruments>
<CsScore>

;i1 0 3 1000;
;i2 0 3 1000;
i3 0 3

</CsScore>
</CsoundSynthesizer>
