# Cybersecurity game "Buffer Overflow Attack Lab (Server Version)"
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![License: CC BY 4.0](https://img.shields.io/badge/License-CC%20BY%204.0-orange.svg)](https://creativecommons.org/licenses/by/4.0/)

Linear training for the [CyberRangeCZ Platform](https://docs.platform.cyberrange.cz/).

This repository implements the **SEED Labs "Buffer Overflow Attack Lab (Server Version)"**
(Wenliang Du, Syracuse University) as a CyberRangeCZ / KYPO-compatible training scenario.
Students exploit a classic **stack buffer overflow** in network services running as
**root** to obtain a **root shell** and read a per-sandbox secret flag on each one, then
defeat **ASLR** by brute force on a third service (same binary, randomization on).

It reuses the infrastructure and scenario structure of the official
[`library-junior-hacker`](https://github.com/cyberrangecz/library-junior-hacker) template.

## Game Levels Summary
- connect to the sandbox and read the briefing (Introduction + Access)
- Task 1 — make the injected **shellcode** run `id` and capture the per-sandbox group it prints → `flag_task1` (`flag-t1-…`), **3 pts**
- Task 2 — exploit **Level 1** (32-bit, leaks frame pointer **and** buffer address) for a root shell → `flag_level1`, **6 pts**
- Task 3 — exploit **Level 2** (32-bit, buffer size **unknown**) with one size-independent payload → `flag_level2`, **7 pts**
- Task 4 — defeat **ASLR** on **Level 3** (`:9092`, same binary as Level 1) by brute force → `flag_level3`, **4 pts**

Total: **20 points**, estimated **~65 minutes**.

## Topology summary
| Host | Image | Mgmt user | Flavor | IP | Role |
|-|-|-|-|-|-|
| attacker | `seed` | `seed` | `m1.medium` | `10.1.26.23` | student's attack box (`gcc`, `python3`, `netcat`, `gdb`) |
| server | `seed` | `seed` | `m1.medium` | `10.1.26.9` | runs the vulnerable BOF containers |
| router | `debian-12-x86_64` | `debian` | `standard.small` | `10.1.26.1` | routing / internet gateway |

The vulnerable services run as three Docker containers on the server. The host keeps
**ASLR on** (`kernel.randomize_va_space = 2`); Levels 1 and 2 opt out per-process with
`setarch -R` (fixed addresses), while Level 3 keeps randomization on:

| Target | Address | Difficulty |
|-|-|-|
| Level 1 | `10.1.26.9:9090` | 32-bit, shows frame pointer **and** buffer address (ASLR off) |
| Level 2 | `10.1.26.9:9091` | 32-bit, shows **only** the buffer address, size unknown (ASLR off) |
| Level 3 | `10.1.26.9:9092` | 32-bit, same binary as Level 1 but **ASLR on** (Task 4 brute force) |

## Anti-cheating (per-sandbox flags)
Each sandbox plants **unique, random** flags, one per task:

- **Levels 1–3** — a `root`-only `/root/flag` (mode `0600`) inside each vulnerable
  container, readable only **after** a root shell (`flag_level1` / `flag_level2` / `flag_level3`).
- **Task 1** — a sandbox-unique **group** on the attacker box (`flag-t1-…`, variable
  `flag_task1`), owned by a dedicated `flagkeeper1` account (the group is its primary
  group). The shellcode runner `a32.out` is installed setuid-root; it calls
  `initgroups(flagkeeper1)` to pick up that group, then drops to the student user, so the
  group shows up in `id` only when `id` is run **through the shellcode** — not when typed
  in a plain shell.

The training definition validates each submission against that sandbox's own value
(`variant_answers: true`). No two students share a flag.

## Deployment
1. Push this directory to your CyberRangeCZ scenario repository (contains `topology.yml` and `provisioning/`).
2. In the CyberRangeCZ web interface:
   - **Sandboxes → Sandbox Definitions → Add Definition** → point to this repo.
   - **Trainings → Training Definitions → Import** → select `training.json`.
3. Create a **Training Instance** with a sandbox pool sized for your class.

## License

This repository uses a dual licensing approach:

* The code (Vagrant, Ansible, and custom code) is licensed under the terms of the MIT License (https://opensource.org/license/mit).
* The game design is licensed under a Creative Commons Attribution 4.0 International License (CC BY 4.0).

**Attribution:**

Lab content adapted from the **SEED Labs** "Buffer Overflow Attack Lab (Server Version)"
by **Wenliang Du**, Syracuse University (https://seedsecuritylabs.org), used for
educational purposes.

Scenario structure and infrastructure based on the game design from:
junior-hacker (https://gitlab.ics.muni.cz/muni-kypo-trainings/games/junior-hacker)

**Cybersecurity Laboratory**
Faculty of Informatics
Masaryk University

**Leading author (junior-hacker):** Miriam Gáliková

**Contributors/Consultants:** Valdemar Švábenský, Jan Vykopal
