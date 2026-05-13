# Boing

[![KiCad](https://img.shields.io/badge/KiCad-PCB%20Design-314CB0?style=for-the-badge&logo=kicad&logoColor=white)](https://www.kicad.org)
[![Onshape](https://img.shields.io/badge/Onshape-CAD%20Design-00B0F0?style=for-the-badge&logo=onshape&logoColor=white)](https://www.onshape.com)
[![ESP32](https://img.shields.io/badge/ESP32--C6-Firmware-E7352C?style=for-the-badge&logo=espressif&logoColor=white)](https://www.espressif.com)

![Boing virtual pet](assets/images/hero.png)

A handheld virtual pet built from scratch — custom PCB, 3D-printed enclosure, and C++ firmware running on a XIAO-ESP32-C6. Feed it, play with it, put it to sleep. Neglect it and it sulks.

---

## What's inside

```text
Boing/
├── firmware/           Arduino sketch (flash this)
│   ├── firmware.ino    Entry point
│   ├── config.h        Pin mapping and tuning constants
│   ├── pet.h / .cpp    Pet model — vitals, life stage, mood
│   ├── input.h / .cpp  Debounced button input
│   ├── audio.h / .cpp  Non-blocking melody sequencer
│   ├── renderer.h / .cpp  OLED display driver
│   └── sprites.h / .cpp   Pixel art bitmaps
├── hardware/
│   ├── pcb/            KiCad schematic + PCB layout
│   ├── cad/            STEP file for the enclosure
│   └── BOM.csv         Parts list with prices
├── docs/
│   ├── ARCHITECTURE.md  Firmware design decisions
│   ├── build_guide.md   PCB assembly and enclosure guide
│   └── firmware_guide.md  How to modify the firmware
├── legacy/
│   └── Tamagotchi.ino  Original single-file sketch (kept for reference)
└── assets/images/      Screenshots and photos used in this README
```

---

## Getting started

### Flash the firmware

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) and the ESP32 board package.
2. Install **Adafruit SSD1306** and **Adafruit GFX** via the Library Manager.
3. Open `firmware/firmware.ino` — the IDE will pick up all `.h`/`.cpp` files in the folder automatically.
4. Select **XIAO_ESP32C6** as the board and flash.

### Buttons

| Button | Action | Effect |
| ------ | ------ | ------ |
| A (left) | Feed | +10% hunger |
| B (middle) | Play | +10% joy, −5% energy |
| C (right) | Rest | +15% energy |

Stats decay continuously. Decay accelerates as a stat drops — a starving pet goes downhill faster than a content one.

---

## Hardware

### PCB

The board is a two-layer design made in KiCad. Order from JLCPCB using the files in `hardware/pcb/` — the Gerbers are ready to go.

![PCB layout](assets/images/pcb_layout.png)

![PCB 3D front](assets/images/pcb_3d_front.png) ![PCB 3D back](assets/images/pcb_3d_back.png)

### Schematic

![Schematic](assets/images/schematic.png)

### Enclosure

The shell is designed in Onshape and printed in two halves held together with M3 screws and brass inserts.

![Enclosure front](assets/images/enclosure_front.png) ![Enclosure CAD](assets/images/enclosure_cad.png)

[Open in Onshape](https://cad.onshape.com/documents/fa5791d8e7f345b436054923/w/ad8e09905eb572b0a8a40e9d/e/e81f06aa82a76dd04f8e6832)

### Assembly

![Assembly diagram](assets/images/assembly.png)

Full step-by-step instructions are in [docs/build_guide.md](docs/build_guide.md).

---

## Bill of materials

| Component | Qty | Unit price | Total |
| --------- | --- | ---------- | ----- |
| XIAO ESP32-C6 | 1 | $7.09 | $7.09 |
| 0.96" OLED display | 1 | $1.93 | $1.93 |
| Pin headers | 1 | $1.69 | $1.69 |
| Passive buzzer (12×9.5 mm) | 1 | $2.39 | $2.39 |
| Tactile buttons (4.5×4.5 mm) | 1 pack | $2.57 | $2.57 |
| Brass threaded inserts (M3×4×5) | 1 pack | $2.47 | $2.47 |
| M3 8 mm screws | 1 pack | $2.23 | $2.23 |
| AliExpress shipping | — | $2.53 | $2.53 |
| PCB (JLCPCB, 5×) | 1 | $3.20 | $3.20 |
| JLCPCB shipping | — | $3.12 | $3.12 |
| **Total (inc. tax)** | | | **~$31** |

Full links are in [hardware/BOM.csv](hardware/BOM.csv).

---

## Firmware overview

The firmware splits cleanly into modules — no global state, event-driven input, float vitals with exponential decay, and a four-stage life cycle (egg → juvenile → adult → elder).

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for the full design breakdown.

---

## Customise it

Everything tuneable is in one file: [`firmware/config.h`](firmware/config.h).

- **Decay speed** — `Tuning::DECAY_BASE` and `DECAY_EXPONENT`
- **Action effects** — `FEED_DELTA`, `PLAY_JOY_DELTA`, `REST_DELTA`
- **Life-stage thresholds** — `JUVENILE_AGE_S`, `ADULT_AGE_S`, `ELDER_AGE_S`
- **Button mapping** — `HW::BTN_A/B/C`

Want to add a new action? Add a method to `pet.h`, call it from the `onButton` switch in `firmware.ino`. That's it.

---

## Contact

Tanishq Goyal — [@Tanuki](https://hackclub.slack.com) — [tanishqgoyal590@gmail.com](mailto:tanishqgoyal590@gmail.com)
