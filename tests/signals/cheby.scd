(
// Use SC3’s built-in Chebyshev generator
~coeffs = [0.2, 0.75, 0.7, 2, 0.35, 0.5, 0.1, 0.9, 0.0];  // T0 to T8

// Create transfer function using Signal.chebyFill
~chebyTable = Signal.chebyFill(1024, ~coeffs);

// Normalize and clip for safety
~chebyTable = ~chebyTable.normalize.clip2(1);

// Plot the result
~x = Signal.linlin((0..1023), 0, 1023, -1.0, 1.0);
~x.plot(~chebyTable, bounds: Rect(100, 100, 600, 300), name: "SC3 Cheby Transfer");
)


(
// Audio test using the SC-generated Chebyshev shaper
{
    var input = SinOsc.ar(200);
    var shaped = Shaper.ar(Buffer.sendCollection(s, ~chebyTable.asWavetable), input);
    shaped * 0.2 ! 2  // stereo, attenuated
}.play;
)
