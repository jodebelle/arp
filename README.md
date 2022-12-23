# arp
The simulator requires (at least) the following 5 processes:
- command console, reading the 6 commands, using keyboard keys
- inspection console, receiving from motors the hoist positions while moving, and reporting on the
screen somehow (free choice); the inspection console manages the S ad R buttons as well
(simulated in a free way using the keyboard)
- motor x, simulating the motion along x axis, receiving command and sending back the real time
position including simulated errors
- motor z, similar to motor x
- watchdog: it checks the previous 4 processes periodically, and sends a reset (like the R button) in
case all processes did nothing (no computation, no motion, no input/output) for a certain time, say,
60 seconds.
