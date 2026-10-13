// tinyvault: a tiny CLI written by ProAI Clips purely as a reverse-engineering test target. MIT licence.
// It hides three "secrets" so we can grade what an AI agent recovers from the stripped binary.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// SECRET 1: unlock code stored XOR-obfuscated (key 0x5A), never as plain text.
static const unsigned char k_code[] = {0x0a,0x08,0x15,0x1b,0x13,0x77,0x0e,0x13,0x14,0x03,0x77,0x6b,0x6a,0x6c,0x6d};
static int check_code(const char *in){
  size_t n = sizeof k_code;
  if (strlen(in) != n) return 0;
  for (size_t i = 0; i < n; i++) if ((unsigned char)(in[i] ^ 0x5A) != k_code[i]) return 0;
  return 1;
}

// SECRET 2: hidden debug menu, enabled by an environment variable whose name is built at runtime.
static int debug_enabled(void){
  char name[16]; volatile int codes[] = {84^7,86^7,95^7,71^7,72^7,79^7,83^7,84^7};  // "TV_GHOST"
  for (int i = 0; i < 8; i++) name[i] = (char)(codes[i]^7); name[8] = 0;
  const char *v = getenv(name);
  return v && strcmp(v, "1") == 0;
}

// SECRET 3: a "vault score" formula used to decide the reward tier.
static uint32_t vault_score(const char *s){
  uint32_t h = 2166136261u;                 // FNV-1a offset basis
  while (*s) { h ^= (unsigned char)*s++; h *= 16777619u; }  // FNV-1a prime
  return (h % 1000u) * 3u + 7u;             // then *3 + 7
}

int main(int argc, char **argv){
  if (argc < 2) { printf("usage: tinyvault <name> [code]\n"); return 1; }
  uint32_t sc = vault_score(argv[1]);
  printf("hello %s, your vault score is %u\n", argv[1], sc);
  if (sc > 2500) printf("tier: gold\n"); else if (sc > 1200) printf("tier: silver\n"); else printf("tier: bronze\n");
  if (argc > 2) printf(check_code(argv[2]) ? "vault unlocked\n" : "wrong code\n");
  if (debug_enabled()) printf("[ghost menu] build=%s internal=%d\n", __DATE__, 42);
  return 0;
}
