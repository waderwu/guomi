// relying on romangol.h

static void outputChar(const_buf text, size_t len)
{
	forloop( i, 0, len )
	{
		printf("0x%02x, ", text[i]);
		if (i % 16 == 15)
			puts("");
	}
	puts("");
}

static void outputDword(u4 * text, size_t len)
{
	forloop( i, 0, len )
	{
		printf("0x%08x, ", text[i]);
		if (i % 4 == 3)
			puts("");
	}
	puts("");
}