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
    agate mpulse p4, 1

    agate_thresh = 0.5
    astart_level = 0.0
    aatk_sec = 0.01
    aatk_level = 0.75
    arel_sec = 0.1
    arel_level = 0.0

    aamp ar_line agate, agate_thresh, astart_level, aatk_sec, aatk_level, arel_sec, arel_level
    asig oscili aamp, 440, 1
    out asig
endin


</CsInstruments>
<CsScore>
    f1 0 8192 10 1
    i1 0 3 1;

</CsScore>
</CsoundSynthesizer>
