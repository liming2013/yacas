/* Math using the standard library, if the precision is less than 13 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "yacasprivate.h"
#include "yacasbase.h"
#include "lispobject.h"
#include "lispatom.h"
#include "lispenvironment.h"
#include "numbers.h"
#include "platmath.h"
#include "errors.h"


double GetDouble(LispCharPtr aString)
{
    return strtod(aString,NULL); //TODO!
}

LispStringPtr Double(double aValue, LispHashTable& aHashTable)
{
    char dummy[150];
//    sprintf(dummy,"%.24g",aValue);
    sprintf(dummy,"%f",aValue);
    return aHashTable.LookUp(dummy);
}

LispStringPtr PlatSin(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(sin(GetDouble(int1)),aHashTable);
}

LispStringPtr PlatCos(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(cos(GetDouble(int1)),aHashTable);
}

LispStringPtr PlatTan(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(tan(GetDouble(int1)),aHashTable);
}

LispStringPtr PlatArcSin(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(asin(GetDouble(int1)),aHashTable);
}

LispStringPtr PlatArcCos(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(acos(GetDouble(int1)),aHashTable);
}

LispStringPtr PlatArcTan(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(atan(GetDouble(int1)),aHashTable);
}

LispStringPtr PlatExp(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(exp(GetDouble(int1)),aHashTable);
}

LispStringPtr PlatLn(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(log(GetDouble(int1)),aHashTable);
}

LispStringPtr PlatPower(LispCharPtr int1, LispCharPtr int2,
                        LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(pow(GetDouble(int1),GetDouble(int2)),aHashTable);
}



LispStringPtr PlatSqrt(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(sqrt(GetDouble(int1)),aHashTable);
}
LispStringPtr PlatPi(LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(acos(-1.0),aHashTable);
}
LispStringPtr PlatFloor(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(floor(GetDouble(int1)),aHashTable);
}
LispStringPtr PlatCeil(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(ceil(GetDouble(int1)),aHashTable);
}
LispStringPtr PlatMod(LispCharPtr int1, LispCharPtr int2,LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(fmod(GetDouble(int1),GetDouble(int2)),aHashTable);
}
LispStringPtr PlatDiv(LispCharPtr int1, LispCharPtr int2,LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(((long)GetDouble(int1))/((long)GetDouble(int2)),aHashTable);
}
LispStringPtr PlatAbs(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(fabs(GetDouble(int1)),aHashTable);
}


/// fast checking for prime numbers

#include "fastprimes.c"

/* subroutine returns 1 if p is in the table of prime numbers up to primes_table_limit */
unsigned primes_table_check(unsigned long p)
{
	unsigned long index;
	unsigned field;
	if (p==0) return primes_table_limit;
	if (p==2) return 1;
	if (p<2 || p>primes_table_limit || (p & 1) == 0) return 0;
	p >>= 1;
	// get index in 8-bit chunks
	index = p >> 3;
	field = p & 7;
	return ((primes_table[index] & (1 << field))==0) ? 0 : 1;
}

LispStringPtr PlatIsPrime(LispCharPtr int1, LispHashTable& aHashTable,LispInt aPrecision)
{
    return Double(primes_table_check((unsigned long)(GetDouble(int1))), aHashTable);
}

// lookup table for transforming the number of digits

const unsigned log2_table_size = 32;
// report the table size
unsigned log2_table_range()
{
	return log2_table_size;
}

#ifdef HAVE_MATH_H
// A lookup table of Ln(n)/Ln(2) for n = 1 .. 32.
// With this we don't have to use math.h if all we need is to convert the number of digits from one base to another. This is also faster.
// Generated by: PrintList(N(Ln(1 .. 32)/Ln(2)), ",") at precision 40
const double log2_table[log2_table_size] =
{
0.,
1.,
1.5849625007211561814537389439478165087598,
2.,
2.3219280948873623478703194294893901758648,
2.5849625007211561814537389439478165087598,
2.807354922057604107441969317231830808641,
3.,
3.1699250014423123629074778878956330175196,
3.3219280948873623478703194294893901758648,
3.4594316186372972561993630467257929587032,
3.5849625007211561814537389439478165087598,
3.7004397181410921603968126542566947336284,
3.807354922057604107441969317231830808641,
3.9068905956085185293240583734372066846246,
4.,
4.0874628412503394082540660108104043540112,
4.1699250014423123629074778878956330175196,
4.2479275134435854937935194229068344226935,
4.3219280948873623478703194294893901758648,
4.3923174227787602888957082611796473174008,
4.4594316186372972561993630467257929587032,
4.5235619560570128722941482441626688444988,
4.5849625007211561814537389439478165087598,
4.6438561897747246957406388589787803517296,
4.7004397181410921603968126542566947336284,
4.7548875021634685443612168318434495262794,
4.807354922057604107441969317231830808641,
4.8579809951275721207197733246279847624768,
4.9068905956085185293240583734372066846246,
4.9541963103868752088061235991755544235489,
5.
};

// table look-up of small integer logarithms, for converting the number of digits to binary and back
double log2_table_lookup(unsigned n)
{
		if (n<=log2_table_size && n>=1)
			return log2_table[n-1];
		else
		{
			RaiseError("log2_table_lookup: error: invalid argument %d\n", n);
			return 0;
		}
}
// convert the number of digits in given base to the number of bits, and back.
// need to round the number of digits.
unsigned long digits_to_bits(unsigned long digits, unsigned base)
{
	return (unsigned long)(0.5 + double(digits)*log2_table_lookup(base));
}

unsigned long bits_to_digits(unsigned long bits, unsigned base)
{
	return (unsigned long)(0.5 + double(bits)/log2_table_lookup(base));
}

#else

struct RationalFrac
{	// this represents p/q with integer p, q and q>0
	long p, q;
};

// need tables of both Ln(n)/Ln(2) and of Ln(2)/Ln(n).
// table of Ln(n)/Ln(2)
const RationalFrac log2_table_l[log2_table_size] = 
{
{0, 1}, {1, 1}, {24727, 15601}, {2, 1}, {1493, 643}, {40328, 15601}, {1603, 571}, {3, 1}, {23673, 7468}, {2136, 643}, {3411, 986}, {55929, 15601}, {840, 227}, {2174, 571}, {13721, 3512}, {4, 1}, {1402, 343}, {31141, 7468}, {5637, 1327}, {2779, 643}, {3202, 729}, {4397, 986}, {13055, 2886}, {71530, 15601}, {2986, 643}, {1067, 227}, {22619, 4757}, {2745, 571}, {4139, 852}, {17233, 3512}, {157375, 31766}, {5, 1}
};
// table of Ln(2)/Ln(n)
const RationalFrac log2_table_linv[log2_table_size] = 
{
{0, 1}, {1, 1}, {665, 1054}, {1, 2}, {4004, 9297}, {665, 1719}, {2393, 6718}, {1, 3}, {665, 2108}, {4004, 13301}, {1935, 6694}, {665, 2384}, {5231, 19357}, {2393, 9111}, {4049, 15819}, {1, 4}, {4036, 16497}, {665, 2773}, {1206, 5123}, {4004, 17305}, {1588, 6975}, {1935, 8629}, {3077, 13919}, {665, 3049}, {2002, 9297}, {5231, 24588}, {665, 3162}, {2393, 11504}, {4943, 24013}, {537, 2635}, {3515, 17414}, {1, 5}};


// need lookup of both Ln(n)/Ln(2) and of Ln(2)/Ln(n)
const RationalFrac& log2_table_lookup(const RationalFrac* table, unsigned n)
{
	if (n<=log2_table_size && n>=2)
		return table[n-1];
	else
	{
		RaiseError("log2_table_fake_lookup: error: invalid argument %d\n", n);
		return table[0];
	}
};
const RationalFrac& log2_table_lookup_l(unsigned n)
{
	return log2_table_lookup(log2_table_l, n);
}
const RationalFrac& log2_table_lookup_linv(unsigned n)
{
	return log2_table_lookup(log2_table_linv, n);
}

// multiply aF by aX and round up to the nearest integer
unsigned long fake_multiply_ceil(const RationalFrac& aF, unsigned long aX)
{
	// compute aX * aF.p / aF.q carefully to avoid overflow and to not undershoot: (maximum number we need to support is p*q, not p*x)
	// p*Div(x,q) + Div(Mod(x, q)*p+q-1, q)
	return aF.p*(((unsigned long)aX)/((unsigned long)aF.q))
	+ ((aX % ((unsigned long)aF.q))*aF.p+aF.q-1)/((unsigned long)aF.q);
}


// multiply aF by aX and round up to the nearest integer


// convert the number of digits in given base to the number of bits, and back
unsigned long digits_to_bits(unsigned long digits, unsigned base)
{
	return fake_multiply_ceil(log2_table_lookup_l(base), digits);
}

unsigned long bits_to_digits(unsigned long bits, unsigned base)
{
	return fake_multiply_ceil(log2_table_lookup_linv(base), bits);
}

#endif // HAVE_MATH_H




/* And here is a reference implementation of YacasBigNumber: one using native types.
*/

#ifdef USE_NATIVE
BigNumber::BigNumber(LispCharPtr aString,LispInt aPrecision,LispInt aBase)
{
  if (strchr(aString,'.') || strchr(aString,'e') || strchr(aString,'E'))
  {
    type  = KDouble;
    value.d = GetDouble(aString);
  }
  else
  {
    type  = KInteger;
    value.i = atoi(aString);
  }
}

BigNumber::BigNumber(const BigNumber& aOther)
{
  type  = aOther.type;
  value = aOther.value;
}

BigNumber::BigNumber() 
{
  type = KDouble;value.d = 0.0;
}


BigNumber::~BigNumber()
{
}

void BigNumber::ToString(LispString& aResult, LispInt aBase) const 
{
  char dummy[150];
  dummy[0] = '\0';
  switch (type)
  {
  case KInteger:
    sprintf(dummy,"%ld",value.i);
    break;
  case KDouble:
    sprintf(dummy,"%f",value.d);
    break;
  }
  aResult.SetStringCounted(dummy,PlatStrLen(dummy));
}
const LispCharPtr BigNumber::NumericLibraryName() const
{
  return "Native types";
}

//          result->value.i assign ((BigNumber)aX).value.i op ((BigNumber)aY).value.i; 


#define TWO_FUNC_WITH(result,assign,op) \
{ \
const BigNumber* x = ((const BigNumber*)(&aX));\
const BigNumber* y = ((const BigNumber*)(&aY));\
  switch (x->type) \
  { \
    case KInteger: \
      switch (y->type) \
      { \
        case KInteger: \
          result->type = KInteger; \
          result->value.i assign ((x->value.i) op (y->value.i)); \
          break; \
        case KDouble: \
          result->type = KDouble; \
          result->value.d assign ((x->value.i) op (y->value.d)); \
          break; \
      } \
      break; \
    case KDouble: \
      switch (y->type) \
      { \
        case KInteger: \
          result->type = KDouble; \
          result->value.d assign ((x->value.d) op (y->value.i)); \
          break; \
        case KDouble: \
          result->type = KDouble; \
          result->value.d assign ((x->value.d) op (y->value.d)); \
          break; \
      } \
      break; \
  } \
}

#define TWO_FUNC(assign,op) \
{ \
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), aY.NumericLibraryName())); \
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), NumericLibraryName())); \
  TWO_FUNC_WITH(this,assign,op) \
}


void BigNumber::Multiply(const BigNumber& aX, const BigNumber& aY, LispInt aPrecision) 
TWO_FUNC(=,*)

void BigNumber::MultiplyAdd(BigNumber& aResult,
              const BigNumber& aX, 
              const BigNumber& aY, 
              LispInt aPrecision) 
TWO_FUNC_WITH(((BigNumber*)(&aResult)),+=,*)

void BigNumber::Add(const BigNumber& aX, const BigNumber& aY, LispInt aPrecision) 
TWO_FUNC(=,+)

void BigNumber::Negate(const BigNumber& aX) 
{
  switch (((const BigNumber*)(&aX))->type)
  {
    case KInteger: value.i = -((BigNumber*)(&aX))->value.i; break;
    case KDouble:  value.d = -((BigNumber*)(&aX))->value.d; break;
  }
}
void BigNumber::Divide(const BigNumber& aX, const BigNumber& aY, LispInt aPrecision) 
TWO_FUNC(=,/)

void BigNumber::ShiftLeft( const BigNumber& aX, LispInt aNrToShift)
{
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), NumericLibraryName())); 
  LISPASSERT(x.type == KInteger);
  value.i = ((BigNumber*)(&aX))->value.i << aNrToShift;
}
void BigNumber::ShiftRight( const BigNumber& aX, LispInt aNrToShift)
{
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), NumericLibraryName())); 
  LISPASSERT(x.type == KInteger);
  value.i = ((BigNumber*)(&aX))->value.i >> aNrToShift;
}
void BigNumber::BitAnd(const BigNumber& aX, const BigNumber& aY)
{
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), aY.NumericLibraryName())); 
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), NumericLibraryName())); 
  LISPASSERT(x->type != KInteger);
  LISPASSERT(y->type != KInteger);
  value.i = (((BigNumber*)(&aX))->value.i) & (((BigNumber*)(&aY))->value.i);
}
void BigNumber::BitOr(const BigNumber& aX, const BigNumber& aY)
{
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), aY.NumericLibraryName())); 
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), NumericLibraryName())); 
  LISPASSERT(x->type != KInteger);
  LISPASSERT(y->type != KInteger);
  value.i = (((BigNumber*)(&aX))->value.i) | (((BigNumber*)(&aY))->value.i);
}
void BigNumber::BitXor(const BigNumber& aX, const BigNumber& aY)
{
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), aY.NumericLibraryName())); 
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), NumericLibraryName())); 
  LISPASSERT(x->type != KInteger);
  LISPASSERT(y->type != KInteger);
  value.i = (((BigNumber*)(&aX))->value.i) ^ (((BigNumber*)(&aY))->value.i);
}


double BigNumber::Double() const
{
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), NumericLibraryName())); 
  if (type == KInteger)
    return double(value.i);
  else//if (type == KDouble)
    return value.d;
}

LispInt BigNumber::BitCount() const
{
  LispInt result = 0;
  if (type == KInteger)
  {
  // prepare the absolute value of the integer for shifting
    long temp = (value.i < 0) ? -value.i : value.i;
    while (temp>0)
    {
      temp >>= 1;
      ++result;
    }
  }
  else	// KDouble, return the binary exponent
  {
  // prepare the absolute value of the number
    double temp = (value.d < 0) ? -value.d : value.d;
    if (temp<1)
    {
      while (temp<1)
      {
        temp *= 2;
	++result;
      }
    }
    else if (temp>1)
    {
      while (temp>1)
      {
        temp /= 2;
	++result;
      }
    }
  }
  return result;
}

LispInt BigNumber::Sign() const
{
  if (type == KInteger)
  {
    return ( (value.i > 0) ? 1 : ((value.i < 0) ? -1 : 0) );
  }
  else
  {
    return ( (value.d > 0) ? 1 : ((value.d < 0) ? -1 : 0) );
  }
}


void BigNumber::SetTo(const BigNumber& aX)
{
  LISPASSERT(!StrCompare(aX.NumericLibraryName(), NumericLibraryName())); 
  type = ((BigNumber*)(&aX))->type;
  value = ((BigNumber*)(&aX))->value;
}

#endif
