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

    ifreq = 100.0
    ifreq_lo = p4
    ifreq_hi = p5
    iq_lo = p6
    iq_high = p7

    ain = vco(0.5, ifreq, 1, 0.5)

    afreq = line(p4, p3, p5)
    aq = line(p6, p3, p7)

    aout = rcladder(ain, afreq, aq)


    out aout

endin


</CsInstruments>
<CsScore>
f1 0 8192 10 1

i1 0 5  20 5000 5 5
i1 5 5  500 1000 0.1 25

</CsScore>
</CsoundSynthesizer>
