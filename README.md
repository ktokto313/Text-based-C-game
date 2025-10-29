 A text based C game in terminal written to show skill learnt in OSG202 (FPTU)

Disclaimer: the combat.c file is not at its best shape and merged into main without reviewing properly, it just works for our interest at the time being.
 
The rest is ChatGPT generated from our project proposal

 # 🧙‍♂️ Text-Based Turn-Based RPG (C Language)

A console-based **turn-based RPG** written in pure C.  
The game focuses on **player freedom** and **customization**, allowing users or admins to adjust gameplay parameters such as difficulty, drop rates, and exploration events.

> 🎮 *A small RPG project exploring system programming and modular design in C.*

---

## 🌟 Project Overview

This project is a **text-based**, **turn-based RPG** where players control different character classes, explore maps, and progress through battles to reach the final boss.  
It is built entirely using C and standard libraries, with modular design and JSON-based configuration files.

---

## 🧩 Character Classes

| Class | Description |
|--------|--------------|
| 🧙 **Wizard** | Master of powerful magic and ranged attacks. |
| ⚔️ **Knight** | Balanced fighter with strong defense. |
| ✨ **Paladin** | Holy warrior who can heal and protect allies. |
| 🗡 **Rogue** | Fast and stealthy, excels at critical hits. |
| 🧝 **Elf** | Agile and intelligent ranged attacker. |

---

## ⚙️ Key Features

- 🔧 **Fully configurable gameplay settings**  
  Adjust difficulty, chest drop rates, monster stats, and more.
- 🧍‍♂️ **Three user roles:**
  - **Admin** – Modify core game parameters, maps, and monsters.  
  - **Tester** – Buy items for free and can’t die, but still take damage.  
  - **Player** – Standard gameplay experience.
- 🗺 **Explorable world maps**  
  Travel between maps, explore, and complete side-quests.
- 💰 **Shops and training zones**  
  Available in the starting village for rest and preparation.
- 👑 **Final boss system**  
  Defeat the last boss to finish the game.
- 💾 **Save and Load system**  
  Game data stored in JSON format using the `cJSON` library.

---

## 🔁 Gameplay Loop

1. **Start in the training village**
   - Customize your character and team.
   - Train, rest, and purchase items.

2. **Explore maps**
   - Travel between regions.
   - Encounter random battles, trade, or mini-games.

3. **Progress and grow stronger**
   - Collect gold and loot.
   - Level up and improve your stats.

4. **Defeat the final boss**
   - Reach the final map and complete the game.

---

## 🧠 Technical Details

- **Language:** C (C99 / C11)
- **Save Format:** JSON (via `cJSON` library)
- **Build System:** Makefile
- **Platform:** Linux
- **Syscalls Used:** `open`, `read`, `write`, `close`, `opendir`, etc.

---

## 🧰 Build Instructions

### Prerequisites
- `gcc`
- `make`
- `cJSON` library (already included under `./lib/cJSON/`)

### Build and Run
```bash
make build
./game.out