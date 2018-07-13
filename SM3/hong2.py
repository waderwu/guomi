def oneround():
    for i in range(16,68):
        s2 = "W[%d] = P1( W[%d] ^ W[%d] ^ ROTATELEFT(W[%d],15)) ^ ROTATELEFT(W[%d],7 ) ^ W[%d];"%(i,i-16,i-9,i-3,i-13,i-6)
        print s2
    for i in range(0,64):
        s2 = "W1[%d] = W[%d] ^ W[%d];"%(i,i,i+4)
    for i in range(0,16):
        s2 = """T = 0x79CC4519;
SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,j)), 7);
SS2 = SS1 ^ ROTATELEFT(A,12);
TT1 = FF0(A,B,C) + D + SS2 + W1[j];
TT2 = GG0(E,F,G) + H + SS1 + W[j];
D = C;
C = ROTATELEFT(B,9);
B = A;
A = TT1;
H = G;
G = ROTATELEFT(F,19);
F = E;
E = P0(TT2);"""
        s2 = s2.replace("j",str(i))
        print s2

    for i in range(16,64):
        s2 = """T = 0x7A879D8A;
SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,j)), 7);
SS2 = SS1 ^ ROTATELEFT(A,12);
TT1 = FF1(A,B,C) + D + SS2 + W1[j];
TT2 = GG1(E,F,G) + H + SS1 + W[j];
D = C;
C = ROTATELEFT(B,9);
B = A;
A = TT1;
H = G;
G = ROTATELEFT(F,19);
F = E;
E = P0(TT2);
"""
        s2 = s2.replace("j",str(i))
        print s2
oneround()
