#shader vertex
#version 410 core

layout(location = 0) in vec4 position;

void main()
{
	gl_Position = position;
};

#shader fragment
#version 410 core

layout(location = 0) out vec4 color;
in vec4 gl_FragCoord;

const int MAX_S = 16;

struct apfloat {
	uint size;
	bool sign;
	uint num[MAX_S];
};

uniform float  u_Iterations;
uniform apfloat u_Offsetx;
uniform apfloat u_Offsety;
uniform apfloat u_FragDelta

apfloat ApAdd(apfloat a, apfloat b);
apfloat ApSub(apfloat a, apfloat b);
apfloat ApMul(apfloat a, apfloat b);
apfloat floatToExtended(float f);
float DivergenceTest();

two.num = uint[MAX_S](0);
two.num[0] = 2;
two.sign = false;

void main()
{
	float iter = DivergenceTest();
	color = vec4(1-(iter/u_Iterations), (iter / u_Iterations), 1 - (iter / u_Iterations)* (iter / u_Iterations),1);
}

float DivergenceTest()
{
	//vec2 c = vec2(u_Offset.x + (gl_FragCoord.x * u_FragDelta), u_Offset.y + (gl_FragCoord.y * u_FragDelta));
	apfloat c[2] = apfloat[2](apAdd(u_Offsetx, apMul(gl_FragCoordx, u_FragDelta)), apAdd(u_Offsety, apMul(gl_FragCoordy, u_FragDelta)));
	apfloat z[2] = c;

	apfloat two;
	two.num = uint[MAX_S](0);
	two.num[0] = 2;
	two.sign = false;

	float i = 0.0;
	while ( i < u_Iterations && dot(vec2(z[0].num[0], z[1].num[0]))<4.0)
	{
		//z = vec2((z.x * z.x) - (z.y * z.y) + c.x, 2.0 * z.x * z.y + c.y);
		z[0] = apAdd(apSub(apMul(z[0], z[0]), apMul(z[1], z[1])), c[0]);
		z[1] = apAdd(apMul(two,apMul(z[0],z[1])), c[1]);
		i++;
	}
	return i;
}

apfloat ApAdd(apfloat a, apfloat b)
{
	apfloat r = a;
	uint carry = 0;
	uint carryb = 0;

	if (!(a.sign ^^ b.sign))
	{
		for (int i = MAX_S - 1; i >= 0; i--)
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
				r.num[i] = usubBorrow(b.num[i], a.num[i], carry);
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
				r.num[i] = usubBorrow(b.num[i], a.num[i], carry);
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

apfloat ApSub(apfloat a, apfloat b)
{
	apfloat r = a;
	uint carry = 0;
	uint carryb = 0;

	if (!(a.sign ^^ !b.sign))
	{
		for (int i = MAX_S - 1; i >= 0; i--)
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
				r.num[i] = usubBorrow(b.num[i], a.num[i], carry);
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
				r.num[i] = usubBorrow(b.num[i], a.num[i], carry);
			}
		}
	}
	else if (!b.sign)
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
	apfloat r;
	r.num = uint[MAX_S](0);
	r.sign = (a.sign ^^ b.sign);
	uint carry = 0;
	for (int i = 0; i < MAX_S; i++)
	{
		for (int j = 0; j < MAX_S; j++)
		{
			if (i + j < MAX_S && a.num[i] != 0 && b.num[j] != 0)
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