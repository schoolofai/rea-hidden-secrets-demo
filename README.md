Video: VIDEO_URL_PLACEHOLDER (to be replaced with the YouTube link)  <!-- REPLACE with the new YouTube link for "REA Reverse Engineer Anything: I Hid 3 Secrets in My Own App. It Found All 3." (ProAI Clips, SCO-556) once it is uploaded -->

> **Video link coming soon** - the line above is a placeholder and will be replaced with the YouTube link. Timestamps below match that video.

# tinyvault: hide 3 secrets, let REA find them, grade every claim

Companion repo for the ProAI Clips video **"REA Reverse Engineer Anything: I Hid 3 Secrets in My Own App. It Found All 3."** (link at the top once it is live).

> **Legal first.** Only reverse engineer software you own or are clearly permitted to examine. REA's own README says it is "for lawful research, education, interoperability, software preservation, and analysis of software you own or are authorized to examine". Licences and laws vary by country. This is not legal advice. Everything here targets `tinyvault`, a toy program we wrote for this test (MIT).

What you get: `tinyvault.c` (about 40 lines of C with 3 hidden secrets), `run_demo.sh` (the exact REA commands from the video), `decode.py` (turns REA's evidence into the answers) and `SCORECARD.md` (every claim graded against the source).

REA itself lives at https://github.com/morluto/rea (MIT). Setup guide: https://rea.tools/

## What I ran it on
Debian 13, x86_64, 8-core CPU, no GPU. gcc 14.2, Node.js 22, OpenJDK 21, Ghidra 12.1.4, REA 6.4.0 (npm package `rea-agents`). Other versions should work, but addresses like `FUN_001010a0` will change if your compiler changes. Step 6 shows how to find yours.

## Quick start (about 15 minutes, most of it downloading Ghidra)

### Step 1. Build the target (video 1:22)
```bash
gcc -O2 -s -o tinyvault tinyvault.c
ls -l tinyvault
```
Expected output (size varies by compiler):
```
-rwxr-xr-x 1 you you 14504 ... tinyvault
```
`-s` strips every symbol. That's the point: no function names to help REA.

### Step 2. Check the easy route is closed (video 2:19)
```bash
strings tinyvault | grep -c TV_GHOST
```
Expected output:
```
0
```
If you see `1`, your compiler folded the name back into a plain string (mine did on the first try, see video 2:19). Keep the `volatile` in `debug_enabled()` and rebuild.

### Step 3. Install a Java runtime and Ghidra (video 1:52)
```bash
sudo apt install openjdk-21-jdk        # or your OS's JDK 21 package
java -version
```
Expected output starts with `openjdk version "21`.

Download the official Ghidra 12.1.4 zip (about 570 MB) from https://github.com/NationalSecurityAgency/ghidra/releases and unzip it, for example to `~/tools/ghidra_12.1.4_PUBLIC`.

### Step 4. Install REA locally (no global install) (video 2:04)
```bash
mkdir rea-sandbox && cd rea-sandbox
npm init -y
npm install rea-agents
npx rea --version
```
Expected output:
```
6.4.0
```
(Or newer. If you want REA inside your coding agent, the official route is `npx rea-agents setup`, which asks before changing any agent config.)

### Step 5. Point REA at Ghidra and run the doctor (video 2:04)
```bash
export GHIDRA_INSTALL_DIR=~/tools/ghidra_12.1.4_PUBLIC
export JAVA_HOME=/usr/lib/jvm/java-21-openjdk-amd64
npx rea doctor --provider ghidra
```
Expected output: the Ghidra checks report healthy. On Debian 13 it also printed an informational `unsupported_host: debian 13`. It worked anyway.

### Step 6. Find main and decompile it (video 2:50)
Stripped binaries have no `main` symbol, so asking for `main` fails with `Unknown Ghidra procedure name or address: main`. List procedures and pick the big one called from the entry point:
```bash
npx rea search ../tinyvault FUN_ --kind procedures --provider ghidra --filter-output raw_result
```
Expected output (mine):
```
raw_result[4]{address,value}:
  0x101020,FUN_00101020
  0x1010a0,FUN_001010a0
  0x1012d0,FUN_001012d0
  0x101300,FUN_00101300
```
On my build `FUN_001010a0` is main (the others are tiny runtime stubs). Decompile it:
```bash
npx rea decompile ../tinyvault FUN_001010a0 --provider ghidra
```
Expected output (mine, trimmed):
```
uVar9 = 0x811c9dc5;
uVar9 = (uVar9 ^ uVar3) * 0x1000193;
uVar9 = (uVar9 % 1000) * 3 + 7;
if ((byte)(pcVar7[lVar6] ^ 0x5aU) != (&DAT_001020d0)[lVar6]) {
local_68[lVar6] = (byte)*(undefined4 *)((long)&local_58 + (long)(int)lVar6 * 4) ^ 7;
pcVar7 = getenv((char *)local_68);
```
Each run takes about 10 seconds on CPU while Ghidra analyses the file.

### Step 7. Read the hidden table (video 3:00)
```bash
npx rea read-bytes ../tinyvault 0x1020d0 --length 15 --provider ghidra --filter-output raw_result.bytes_hex
```
Expected output:
```
bytes_hex: 0a08151b13770e131403776b6a6c6d
```
Use whatever address your decompile shows next to `DAT_`.

### Step 8. Decode the evidence (video 3:18)
```bash
python3 ../decode.py
```
Expected output:
```
secret 1 code : PROAI-TINY-1067
secret 2 env  : TV_GHOST=1
secret 3 score: FNV-1a(name)%1000*3+7 -> demo = 2449
```

### Step 9. Check against the real binary (video 3:18)
```bash
TV_GHOST=1 ../tinyvault demo PROAI-TINY-1067
../tinyvault ""
```
Expected output:
```
hello demo, your vault score is 2449
tier: silver
vault unlocked
[ghost menu] build=<your build date> internal=42
hello , your vault score is 790
tier: bronze
```

### Step 10. Grade it (video 3:26)
Open `SCORECARD.md`, compare each claim with `tinyvault.c`, and mark it correct or wrong. My agent run (Claude Code + REA, shell blocked, 12 turns, 53 s) scored 10/10 with nothing made up.

## What this test does NOT show (video 3:50)
- It is an easy target: one function, about 40 lines, plain libc calls, trivial XOR.
- Real apps are far bigger, with packers, servers and licence checks. Finding a check is not rebuilding a product.
- Analysis runs locally, but if you use REA through an agent, your model provider sees what the agent sends it.

## Licence
MIT for everything in this repo. REA, Ghidra and any agent you use have their own licences.
