
def oneround(i,A,B,C,D,E,F,G,H):
    if i>=12:
        s2 = "W[%d] = P1( W[%d] ^ W[%d] ^ ROTATELEFT(W[%d],15)) ^ ROTATELEFT(W[%d],7 ) ^ W[%d];"%(i+4,i-12,i-5,i+1,i-9,i-2)
        s2 = s2.replace("i",str(i))
        print s2
    if i<=15:
        s2 ="""TT2 = ROTATELEFT(*(A),12);
TT1 = TT2 + *(E) + t[i];
TT1 = ROTATELEFT(TT1,7);
TT2 ^= TT1;
*(D) = *(D) + FF0(*(A),*(B),*(C)) + TT2 + (W[i] ^ W[%d]);
*(H) = *(H) + GG0(*(E),*(F),*(G)) + TT1 + W[i];
*(B) = ROTATELEFT(*(B), 9);
*(F) = ROTATELEFT(*(F) ,19);
*(H) = P0(*(H));
        """%(i+4)
        s2 = s2.replace("i",str(i))
        s2 = s2.replace("*","")
        s2 = s2.replace("(A)",A)
        s2 = s2.replace("(B)",B)
        s2 = s2.replace("(C)",C)
        s2 = s2.replace("(D)",D)
        s2 = s2.replace("(E)",E)
        s2 = s2.replace("(F)",F)
        s2 = s2.replace("(G)",G)
        s2 = s2.replace("(H)",H)
        print s2
    else:
        s2 ="""TT2 = ROTATELEFT(*(A),12);
TT1 = TT2 + *(E) + t[i];
TT1 = ROTATELEFT(TT1,7);
TT2 ^= TT1;
*(D) = *(D) + FF1(*(A),*(B),*(C)) + TT2 + (W[i] ^ W[%d]);
*(H) = *(H) + GG1(*(E),*(F),*(G)) + TT1 + W[i];
*(B) = ROTATELEFT(*(B), 9);
*(F) = ROTATELEFT(*(F) ,19);
*(H) = P0(*(H));
        """%(i+4)
        s2 = s2.replace("i",str(i))
        s2 = s2.replace("*","")
        s2 = s2.replace("(A)",A)
        s2 = s2.replace("(B)",B)
        s2 = s2.replace("(C)",C)
        s2 = s2.replace("(D)",D)
        s2 = s2.replace("(E)",E)
        s2 = s2.replace("(F)",F)
        s2 = s2.replace("(G)",G)
        s2 = s2.replace("(H)",H)
        print s2

for i in range(0,64,4):
    oneround(i, "A","B","C","D","E","F","G","H");
    oneround(i+1, "D","A","B","C","H","E","F","G");
    oneround(i+2, "C","D","A","B","G","H","E","F");
    oneround(i+3, "B","C","D","A","F","G","H","E");
