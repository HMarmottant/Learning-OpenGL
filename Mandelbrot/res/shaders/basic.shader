#shader vertex
#version 410 core

layout(location =0) in vec4 position;


void main()
{
	gl_Position = position;
};

#shader fragment
#version 410 core

layout(location =0) out vec4 color;

in vec4 gl_FragCoord;

const int MAX_S = 16;

struct apfloat {
	uint size;
	bool sign;
	uint num[MAX_S];
};

uniform apfloat u_A;
uniform apfloat u_B;

apfloat ApAdd(apfloat a, apfloat b);
apfloat ApSub(apfloat a, apfloat b);
apfloat ApMul(apfloat a, apfloat b);
apfloat floatToExtended(float f);


void main()
{
	bool a = u_A.sign;
	bool b = u_B.sign;
	apfloat r = floatToExtended(32.0f);
	/*int t = floatBitsToInt(32.0f);
	int sign = bitfieldExtract(t, 31, 1);
	int frac = bitfieldExtract(t, 0, 23);
	int exp = bitfieldExtract(t, 23, 8);
	exp = exp > 0 ? exp - 127 : exp + 256 - 127;*/
	color = vec4(r.num[0] == 6, 0, 0, 0);
}

apfloat ApAdd(apfloat a, apfloat b)
{
	apfloat r = a;
	uint carry = 0;
	uint carryb = 0;

	if (!(a.sign ^^ b.sign))
	{
		for (uint i = MAX_S - 1; i >= 0; i--)
		{
			if (carry > 0)
			{
				carry = 0;
				carryb = 0;
				r.num[i] = uaddCarry(a.num[i], b.num[i], carry);
				r.num[i] = uaddCarry(r.num[i], 1, carryb);
				carry = carry + carryb;
			}
			else
			{
				r.num[i] = uaddCarry(a.num[i], b.num[i], carry);
			}
		}
		return r;
	}
	else if (a.sign)
	{
		for (int i = MAX_S - 1; i >= 0; i--)
		{
			if (carry > 0)
			{
				carry = 0;
				carryb = 0;
				r.num[i] = usubBorrow( b.num[i], a.num[i], carry);
				if (carry > 0)
				{
					r.num[i] = uaddCarry(r.num[i], 1, carryb);
				}
				else
				{
					r.num[i] = usubBorrow(r.num[i], 1, carryb);
				}
				carry = carry + carryb;
			}
			else
			{
				r.num[i] = usubBorrow( b.num[i], a.num[i], carry);
			}
		}
	}
	else if (b.sign)
	{
		for (int i = MAX_S - 1; i >= 0; i--)
		{
			if (carry > 0)
			{
				carry = 0;
				carryb = 0;
				r.num[i] = usubBorrow(a.num[i], b.num[i], carry);
				if (carry > 0)
				{
					r.num[i] = uaddCarry(r.num[i], 1, carryb);
				}
				else
				{
					r.num[i] = usubBorrow(r.num[i], 1, carryb);
				}
				carry = carry + carryb;
			}
			else
			{
				r.num[i] = usubBorrow(a.num[i], b.num[i], carry);
			}
		}
	}

	if (carry > 0)
	{
		r.sign = true;
		for (int i = MAX_S - 1; i >= 0; i--)
		{
			r.num[i] = (0xFFFFFFFF - r.num[i] + 1);
		}
	}

	return r;
}


apfloat ApMul(apfloat a, apfloat b)
{
	uint msb;
	uint lsb;
	apfloat r ;
	r.num = uint[MAX_S](0);
	r.sign = (a.sign ^^ b.sign);
	uint carry = 0;
	for (int i = 0; i < MAX_S; i++)
	{
		for (int j = 0; j < MAX_S; j++)
		{
			if (i + j < MAX_S && a.num[i] != 0 && b.num[j] !=0)
			{
				umulExtended(a.num[i], b.num[j], msb, lsb);

				carry = 1;
				for (int m = i + j; m >= 0 && carry > 0; m--)
				{
					r.num[m] = uaddCarry(lsb, r.num[m], carry);
				}

				carry = 1;

				for (int m = i + j - 1; m >= 0 && carry > 0; m--)
				{
					r.num[m] = uaddCarry(msb, r.num[m], carry);
				}
			}
		}
	}

	return r;
}

apfloat floatToExtended(float f)
{
	int t = floatBitsToInt(f);

	apfloat r;
	r.num = uint[MAX_S](0);

	if (t > 0)
	{
		r.sign = true;
	}
	else
	{
		r.sign = false;
		t = -t;
	}

	int exp = bitfieldExtract(t, 23, 8);
	exp = exp > 0 ? exp - 127  : exp + 256 - 127  ;

	//r.num[0] = exp;

	if ((exp + 1) >= 32 && (exp + 1) <= MAX_S * -32) return r;

	int frac = bitfieldExtract(t, 0, 23) + 0x800000;
	int fracsplit;

	int apoffset = (exp + 1) / 32;
	int apbitoffset = (exp + 1) - apoffset * 32;


	if ((exp + 1) <= 0)
	{
		if ( 32 + apbitoffset - 24 >= 0)
		{
			r.num[1 - apoffset] = bitfieldInsert(0, frac, 32 + apbitoffset - 24, 24);
		}
		else
		{
			fracsplit = bitfieldExtract(t, 24 - 32 - apbitoffset, 32 + apbitoffset);

			r.num[2 - apoffset] = bitfieldInsert(0, frac, 64 + apbitoffset - 24, -32 - apbitoffset + 24);
			r.num[1 - apoffset] = bitfieldInsert(0, fracsplit, 0, 32 + apbitoffset);
		}
	}
	else
	{
		if ( apbitoffset - 24 >= 0)
		{
			r.num[0] = bitfieldInsert(0, frac, apbitoffset - 24, 24);
		}
		else
		{
			fracsplit = bitfieldExtract(t, 24 - apbitoffset, apbitoffset);

			r.num[1] = bitfieldInsert(0, frac, 32 - 24 + apbitoffset, 24 - apbitoffset);
			r.num[0] = bitfieldInsert(0, fracsplit, 0, apbitoffset);
		}
	}	

	r.num[0] = apbitoffset;

	return r;
}