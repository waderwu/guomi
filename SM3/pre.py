def loopleftshift(X,n):
    return X<<n | X>>(32-n)

if __name__ == "__main__":
    s1 = "{"
    s2 = "{"
    for i in range(0,16):
        s1 += "0x%x,"%loopleftshift(0x79CC4519,i)
    for i in range(16,64):
        j = i%32
        s1 += "0x%x,"%loopleftshift(0x7A879D8A,j)

    print s1
    print s2
