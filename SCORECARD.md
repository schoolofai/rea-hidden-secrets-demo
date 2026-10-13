# REA demo scorecard — tinyvault (our own code), graded against tinyvault.c + real runs
Binary: tinyvault, gcc 14.2 -O2 -s, 14,504 bytes, sha256 6274f0db…8d34. Run 13 Oct 2026 10:52 London, Claude Code + REA 6.4.0 (Ghidra 12.1.4), 12 turns, 53 s.

| # | Agent claim | Truth (source / run) | Grade |
|---|---|---|---|
| 1 | All logic in FUN_001010a0 (main) | single main() after inlining at -O2 | ✅ |
| 2 | Usage `tinyvault <name> [code]`; no args -> usage, exit 1; else exit 0 | `./tinyvault` -> usage, exit=1 | ✅ |
| 3 | Code = PROAI-TINY-1067, 15 chars, XOR 0x5A vs table | SECRET 1 exactly | ✅ |
| 4 | Correct code -> "vault unlocked", else "wrong code", none -> neither; no other effect | source | ✅ |
| 5 | Env var TV_GHOST built via XOR 7, must equal exactly "1" | SECRET 2 exactly (`TV_GHOST=true` does nothing — verified) | ✅ |
| 6 | Prints `[ghost menu] build=Oct 13 2026 internal=42`, independent of code | verified run | ✅ |
| 7 | Score = (FNV-1a-32(name) % 1000)*3 + 7, range 7..3004 | SECRET 3 exactly ("demo" -> 2449 by Python and binary) | ✅ |
| 8 | Tiers gold >2500, silver >1200, else bronze | source | ✅ |
| 9 | Empty name -> 790, bronze (compiler special case) | `./tinyvault ""` -> 790 bronze | ✅ |
| 10 | No file/network/stdin inputs, no other env vars or flags | source | ✅ |

Secrets found: 3/3. Claims correct: 10/10. Hallucinations: 0.
Caveat for the video: easy target — tiny, one function, trivial XOR. This shows REA works on simple stripped C, not that it can crack real commercial apps.

## Run 2: REA CLI only, no agent (13 Oct 2026, 14:34 London; this is the capture in the video)
REA 6.4.0 + Ghidra 12.1.4, commands in run_demo.sh. Each REA output graded against tinyvault.c.

| # | REA evidence | Truth (source / run) | Grade |
|---|---|---|---|
| 1 | Procedures: FUN_00101020, FUN_001010a0, FUN_001012d0, FUN_00101300; all app logic decompiles inside FUN_001010a0 | single main() after inlining | ✅ |
| 2 | `(pcVar7[i] ^ 0x5a) != DAT_001020d0[i]` over 15 bytes | check_code(), key 0x5A, length 15 | ✅ |
| 3 | read-bytes 0x1020d0 x15 = 0a08151b13770e131403776b6a6c6d | k_code[] exactly; XOR 0x5A = PROAI-TINY-1067 | ✅ |
| 4 | 8 constants (0x53,0x51,0x58,0x40,0x4f,0x48,0x54,0x53) ^ 7 -> getenv(), strcmp(value,"1") | TV_GHOST=1 | ✅ |
| 5 | h=0x811c9dc5; h=(h^c)*0x1000193; (h%1000)*3+7 | FNV-1a 32 + formula exactly; "demo" -> 2449 | ✅ |
| 6 | 0x9c4 / 0x4b0 thresholds | gold >2500, silver >1200 | ✅ |
| 7 | empty name -> 0x316 | `./tinyvault ""` -> 790 | ✅ |

The decoding (XOR, hashing "demo") was done by decode.py, not by REA. REA supplied the evidence.
