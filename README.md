# sysc-clock-mux

## Background

See [this post](https://forums.accellera.org/topic/7129-how-to-implement-a-proper-clock-mux-in-systemc/) for a background on what this SystemC module does.

## Execution

* Set `SYSTEMC_HOME` and `SYSTEMC_ARCH` environment variables

* Compile

    $ make

* Execute 

    $ ./sim.x

## Expected output

    Initial clock period: 2 ns
    @0 s: Triggered clock process
    @2 ns: Triggered clock process
    @4 ns: Triggered clock process
    @6 ns: Triggered clock process
    @8 ns: Triggered clock process
    @10 ns: Clock period changed to:3 ns
    @10 ns: Triggered clock process
    @12 ns: Triggered clock process
    @15 ns: Triggered clock process
    @18 ns: Triggered clock process
    @21 ns: Triggered clock process
    @24 ns: Triggered clock process
    @27 ns: Triggered clock process
    @30 ns: Clock period changed to:5 ns
    @30 ns: Triggered clock process
    @35 ns: Triggered clock process
    @40 ns: Triggered clock process
    @45 ns: Triggered clock process
    @50 ns: Triggered clock process
    @55 ns: Triggered clock process
