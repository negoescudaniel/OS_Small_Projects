long mod_exp(long base, long exp, long mod){
	long result = 1;
	base = base % mod;
	while(exp>0)
	{
		if(exp%2 == 1)
			result = (result *base) % mod;
		base = (base * base) % mod;
		exp /=2;		
	}
	return result;
}

void xor_encrypt(char *msg, int key){
	int i;
	for(i = 0; msg[i]; i++)
		msg[i] ^= key;
}