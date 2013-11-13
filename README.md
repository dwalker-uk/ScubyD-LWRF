ScubyD-LWRF
===========

This GitHub repository is intended to capture my current (hobby) project focused on Lightwave RF (LWRF) control.

My primary aim from this project is to develop a cheap, low power device which could translate simple LWRF commands into complex sequences of messages.  A simple example of this would be the ability to press a single button (whether on a mood switch, a wireless switch or a remote control) when I leave the house which would turn off all of my home entertainment devices, turn off my internal lights after 10 seconds, and turn on my external lights for 2 minutes.  There are of course many other possibilities.

In approaching the problem, I have drawn from a number of other sources to which I am very grateful and without which I wouldn't have even considered it.  The key sources, which fully deserve credit for their work, are:
 - https://github.com/roberttidey/LightwaveRF - the source of the Arduino code I use to both transmit and receive LWRF signals.  This translates the 10-character commands (see later) into the transmitted waveforms, and vice-versa.
 - http://www.benjiegillam.com/2013/02/lightwaverf-rf-protocol/ - some of the early work I saw which breaks down the LWRF 10-character codes to understand their meaning.  A lot of my work has focused on validating and expanding this.
 - http://lightwaverfcommunity.org.uk/forums/ - an excellent LWRF-focused forum, which includes many threads from a range of contributors which have enabled the collaborative protocol analysis to be done.

Current files in this project include:
 - ScubyD-LWRF-Tx.ino         - My transmit code (using the roberttidey libraries)
 - ScubyD-LWRF-Rx.ino         - My receive code (using the roberttidey libraries)
 - LightwaveRF Protocol.pdf   - A write-up of my view of the 10-character commands which make up the protocol.

Hopefully this will help someone out there - and very happy to receive comments / corrections, or to collaborate on this.

David
