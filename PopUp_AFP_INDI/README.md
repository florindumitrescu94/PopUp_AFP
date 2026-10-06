# PopUp AFP — INDI driver

INDI driver for the PopUp AFP pop-up dust cover + dimmable flat panel, ported
from this repo's ASCOM `ICoverCalibrator` driver (`PopUp_AFP/CoverCalibratorDriver/`)
so the panel can be used from INDI/Ekos/KStars on Linux instead of (or
alongside) Windows + ASCOM.

It talks to the same Arduino sketch — `PopUp_AFP_Arduino/PopUp_AFP_Arduino_V2_CURRENT/PopUp_AFP_Arduino_V2.ino`
— over the same 9600-baud, `>CMD#`-framed serial protocol. No firmware changes
are required; flash the Arduino exactly as the main README already describes.

## Building

Dependencies: an INDI installation with headers (`libindi-dev`), cmake, a C++17
compiler, and pkg-config.

```bash
sudo apt install libindi-dev indi-bin cmake build-essential pkg-config
```

```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

`make install` drops the driver at `/usr/local/bin/indi_popup_afp` and its
driver descriptor at `/usr/share/indi/indi_popup_afp.xml` — the two locations
INDI servers and clients (indiserver, KStars/Ekos, the INDI Web Manager) scan
automatically, so nothing else needs registering.

If `/dev/ttyUSB*` or `/dev/ttyACM*` isn't writable by your user, add yourself
to the serial port's owning group (usually `dialout`) and re-login:

```bash
sudo usermod -aG dialout $USER
```

## Using it

Once installed, "PopUp AFP" appears under the Auxiliary device group in any
INDI client. Connect, set the serial port on the Connection tab if it isn't
already `/dev/ttyUSB0`, and the Main Control tab gets:

- **Cover Park / UnPark** — closes/opens the pop-up cover (`>CLOSE#` / `>OPEN#`)
- **Flat Light Control** on/off, plus a 0-255 **Flat Field Light Level** —
  matches the ASCOM driver's `CalibratorOn(Brightness)` / `CalibratorOff()`

The Options tab has **Servo Calibration** (Open/Closed, in degrees 0-270),
replacing the ASCOM SetupDialog's Open/Closed position fields — same values
the main README has you set in NINA (fully open = 270, closed = 0). These are
re-sent to the Arduino on every connect, since the firmware keeps them in RAM,
not EEPROM.

## Known firmware limitation

`AbortCap()` sends `>HALT#`, but `PopUp_AFP_Arduino_V2.ino`'s
`processSerialCommand()` has no `HALT` case today, so it's a no-op on the
device — same as the ASCOM driver's `HaltCover()`. Abort will start working on
its own the day the firmware grows a case for it; no driver change needed.

## What's not yet verified

This was built and compiled against a real `libindi-dev` and passed a smoke
test of the INDI property handshake, but has not been run against the actual
Arduino. Before trusting it on a real panel, verify:

- the serial port name/permissions on the test machine
- that `OPEN`/`CLOSE` actually move the cover through its full range at the
  configured servo angles
- that brightness changes while the light is already on take effect
  immediately (vs. only on the next `EnableLightBox(true)`)
