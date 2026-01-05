# Hardware

Hardware designs for supporting Automatic Control for DC trains.

## Block Controller

Controller board for managing a block of the the layout.

## Experiments

### Block Occupancy Detection

Using a rectifier bridge inline with the track/block supply and an opto-coupler to isolate 12V rail from the 5V rail.

The rectifier bridge's `+` and `-` are shorted together and one side of the track/block supply goes through the `~` of the bridge-rectifier.

The input (LED) of the opto-coupler receives the voltage drop over the `~` of the bridge-rectifier (+ current limiting R). Use a bidirectional optocoupler - one that has two anti-parallel LEDs.

```
             o--------------------o
                       +             Track/Block
Track Supply        ~ /|\ ~
             o---+---  | ---+-----o
                 |    \|/   |
                 |     -    |    ________
                 |          +----|*     |--- Collector (pullup R to 5V)
                 +--[R??]--------|______|----Emitter   (GND)
```

### 38kHz IR optical gate detector

The Aliexpress IR optical sensors that work based on reflecting the light are suseptable to ambient light inteference.

The idea here is that the 38kHz modulation (using in remote controls) will mitigate that and allow for greater accuracy of detecting passing trains/wagons (less false positives).

38kHz IR sender => 38kHz IR receiver (all in one)

#### 38kHz IR Sender

Legenda:

```txt
Nand-gate:
    =|).

Capacitor:
    --||--

Resistor:
    -[xxx]-

Potentiometer:
    -[xxx/]-
```

38kHz Oscillator with Enable (E-active high) using a 74HC00 (must be CMOS):

```txt
      |---------[150k*]---------|
      |                         |  1nF
      |      |--[10k/]---[10k]--+--||---|
      |      |                          |
      |      +-----|                    |         |--|             |--|       OUT
       =|).--+      =|).----------------+--[10k]--+   =|).--[10k]--+   =|).----o
 E    |      +-----|                              |--|             |--|
 o----|
```

*) 150k Ohm => 2 100k in parallel, in series with 100k

The 38kHz IR reciever, commonly used in consumer electroncis, has VCC, GND and OUT pins.
Pass the VCC through a small resistor (10-100 ohm) and put a decoupling capacitor (1-100uF) on the VCC pin of the IR receiver (low-pass).
That should take care of any random spikes of activity on the OUT pin.

I have put heat-shrink tubing on both the IR LED and the IR receiver and have put a 4k7 resistor on the IR LED, and still the whole thing is way to sensitive. I have tried a reflective setup, where both LED and IR receiver both point the same direction. That will trigger the receiver even when pointing them up. Also tried a gate-like setup, where the LED and IR receiver face each other and the beam has to be 'broken'. Even with a considate object in between, the IR light still finds a way around it...
