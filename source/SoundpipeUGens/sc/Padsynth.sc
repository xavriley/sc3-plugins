Padsynth : UGen {
    *ar { arg freq = 440.0, bw = 40.0, num_harmonics = 64, mul = 1.0, add = 0.0;
        ^this.multiNew('audio', freq, bw, num_harmonics).madd(mul, add)
    }
}
