t=bytes.fromhex("0a08151b13770e131403776b6a6c6d")
print("secret 1 code :", bytes(b^0x5a for b in t).decode())
w=[0x53,0x51,0x58,0x40,0x4f,0x48,0x54,0x53]
print("secret 2 env  :", bytes(b^7 for b in w).decode()+"=1")
h=0x811c9dc5
for c in b"demo": h=((h^c)*0x1000193)&0xffffffff
print("secret 3 score: FNV-1a(name)%1000*3+7 -> demo =", h%1000*3+7)
