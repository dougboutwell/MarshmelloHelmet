# MarshmelloHelmet

(Unfinished)

LED-Backlit, DMX-controllable Marshmello helmet, capable of displaying light shows synchronized to music played via Serato.

The basic idea here is to...

1. 3D print a wearable Marshmello helmet from a transluscent filament,
2. Fill the inside of the helmet with a hand-wired array of WS2812 LEDs to create a simple pixel matrix,
3. Drive that pixel matrix with a small microcontroller (i.e. Arduino Nano), powered by AAA batteries, attached to the helmet interior,
4. Use SoundStripe + Serato, or a similar software pairing, to synchronize music and DMX playback from a laptop,
5. Use a DMX shield on a second Arduino to receive that DMX data,
6. Translate that data into a custom protocol that simplifies lighting commands into an animation mode + timing information,
7. Use a pair of NRF24L01+ transcievers to send that information between the two Arduino units.

I actually had most of the components working, but then COVID happened, and I had nowhere to wear it, and I subsequently put all of this on a shelf, where it's been gathering dust.
