/***************************************

** Project:BigInt Class Design

** File:bigint.cpp

** Edition:v1.0.1 Demo

** Coder:Kingsam Chen¡¢Lei Yu [CodeThink]

** Last Modify:2011-8-13

****************************************/

#include "bigint.h"
#include <crtdbg.h>
#include <cstdlib>


CBigInt::CBigInt() : m_sign(false), m_eleCount(0), m_dataLen(DATALEN)
{
	m_pData = new byte[DATALEN];
	_ASSERT(m_pData != NULL);
}


CBigInt::CBigInt(int num) : m_dataLen(DATALEN)
{
	// num with int type will not greater than 20 digits
	m_pData = new byte[DATALEN];
	_ASSERT(m_pData != NULL);

	FormatIntoInternalData(num);
}


CBigInt::CBigInt(const CBigInt& num) : m_dataLen(num.m_dataLen)
{
	m_pData = new byte[num.m_dataLen];
	_ASSERT(m_pData != NULL);

	CopyInternalData(num);
}


CBigInt::CBigInt(const wchar_t* pszNum) : m_dataLen(DATALEN)
{
	wstring num = DealWithNum(pszNum);

	if (!m_eleCount)
	{
		ErrorOutput(L"Invalid initialization\n");
		_ASSERT(false);
	}

	while (m_dataLen <= m_eleCount)
		m_dataLen = m_dataLen * DATALEN_GROWING_RATE / 100;

	m_pData = new byte[m_dataLen];
	_ASSERT(m_pData != NULL);

	FormatIntoInternalData(num.c_str());
	AdjustSignal();
}


CBigInt::~CBigInt()
{
	Clear();
}


void CBigInt::CopyInternalData(const CBigInt& num)
{
	errno_t  err;

	m_sign = num.m_sign;
	m_eleCount = num.m_eleCount;

	if (m_dataLen < num.m_dataLen)
	{
		byte* pTempBuff = new byte[num.m_dataLen];
		_ASSERT(pTempBuff != NULL);
		
		delete [] m_pData;
		m_pData = pTempBuff;

		m_dataLen = num.m_dataLen;
	}

	err = memcpy_s(m_pData, sizeof(byte) * m_dataLen, num.m_pData, sizeof(byte) * m_eleCount);

	// zero if successful
	if (err)
	{
		ErrorOutput(L"Cannot copy the internal data!\n");
		_ASSERT(false);
	}
}


void CBigInt::FormatIntoInternalData(int num)
{
	if (num >= 0)
	{
		m_sign = false;
	} 
	else
	{
		m_sign = true;
		num = -num;
	}

	m_eleCount = 0;

	do 
	{
		m_pData[m_eleCount++] = num % 10;
		num /= 10;
	} while (num);
}


/*
	Description:
		Formats number that in string type into internal data
		You need to make sure the m_eleCount is valid whenever invokes the
		function
	Parameters:
		pszNum[in] - Number in string type has been preprocessed
	Return Value:
		none
*/
void CBigInt::FormatIntoInternalData(const wchar_t* pszNum)
{
	if(L'+' == *pszNum)
	{
		m_sign = false;
	}
	else
	{
		m_sign = true;
	}

	for(int i = 0; i != m_eleCount; ++i)
	{
		m_pData[i] = pszNum[m_eleCount - i] - L'0';
	}
}


/*
	Description:
		Preprocessing. Gets the signal of the number and generates a new
		string object consisted of numeric chars
	Parameters:
		p[in] - The number for initialization
	Return Value:
		wstring - The number has been preprocessed
*/
wstring CBigInt::DealWithNum(const wchar_t* p)
{
	wstring ret;
	ret += L'+';

	if(L'+' == *p)
	{
		++p;
	}
	else if(L'-' == *p)
	{
		ret[0] = L'-';
		++p;
	}

	while((L'0' == *p) && (L'\0' != *(p + 1)))
	{
		++p;
	}

	m_eleCount = 0;

	for(; iswdigit(*p); ++p)
	{
		++m_eleCount;
		ret += *p;
	}

	return ret;
}


CBigInt& CBigInt::operator =(const CBigInt& num)
{
	CopyInternalData(num);

	return *this;
}

CBigInt& CBigInt::operator =(const wchar_t* pszNum)
{
	wstring num = DealWithNum(pszNum);

	if (!m_eleCount)
	{
		ErrorOutput(L"Invalid Number.\n");
		_ASSERT(false);
	}

	m_eleCount = num.size() - 1;

	if(m_dataLen < m_eleCount)
	{
		while (m_dataLen <= m_eleCount)
			m_dataLen = m_dataLen * DATALEN_GROWING_RATE / 100;

		delete [] m_pData;
		m_pData = new byte[m_dataLen];
		_ASSERT(m_pData != NULL);
	}

	FormatIntoInternalData(num.c_str());
	AdjustSignal();
	return *this;
}


CBigInt& CBigInt::operator =(int num)
{ 
	_ASSERT(m_dataLen >= DATALEN);
	FormatIntoInternalData(num);

	return *this;
}


const CBigInt operator +(const CBigInt& lhs, const CBigInt& rhs)
{
	CBigInt num(lhs);
	num += rhs;

	return num;
}


CBigInt& CBigInt::operator +=(const CBigInt& rhs)
{
	if(m_sign^rhs.m_sign)
	{
		if(m_sign)
		{
			// -a + b = -(a - b)
			// to keep arithmetical interface in compatibility
			CBigInt& t = UnsignedDec(rhs);
			t.m_sign = !t.m_sign;
			AdjustSignal();

			return t;
		}
		else
		{
			return UnsignedDec(rhs);
		}
	}
	else
	{
		return UnsignedlInc(rhs);
	}
}


CBigInt& CBigInt::operator +=(int rhs)
{
	CBigInt num(rhs);
	*this += num;

	return *this;
}


const CBigInt operator -(const CBigInt& lhs, const CBigInt& rhs)
{
	CBigInt num(lhs);
	num -= rhs;

	return num;
}


CBigInt& CBigInt::operator -=(const CBigInt& rhs)
{
	if(m_sign^rhs.m_sign)
	{
		return UnsignedlInc(rhs);
	}
	else
	{
		if(m_sign)
		{
			// the same as operator+=
			CBigInt& t = UnsignedDec(rhs);
			t.m_sign = !t.m_sign;
			AdjustSignal();

			return t;
		}
		else
		{
			return UnsignedDec(rhs);
		}
	}
}


CBigInt& CBigInt::operator -=(int rhs)
{
	CBigInt num(rhs);
	*this -= num;

	return *this;
}

CBigInt& CBigInt::UnsignedlInc(const CBigInt& rhs)
{
	// plus 1 in case of carry
	int predictLen = (m_eleCount > rhs.m_eleCount ? m_eleCount : rhs.m_eleCount) + 1;

	// expand buffer
	if (m_dataLen <= predictLen)
	{
		while (m_dataLen <= predictLen)
		{
			m_dataLen = m_dataLen * DATALEN_GROWING_RATE / 100;
		}

		byte* t = new byte[m_dataLen];
		_ASSERT(t);

		errno_t err = memcpy_s(t, sizeof(byte) * m_dataLen, m_pData, sizeof(byte) * m_eleCount);

		if (err)
		{
			ErrorOutput(L"Expand buffer in UnsignedInc failed");
			_ASSERT(false);
		}

		delete [] m_pData;
		m_pData = t;
	}

	int carry = 0;
	for (int i = 0; i != rhs.m_eleCount; ++i)
	{
		m_pData[i] = (i < m_eleCount ? m_pData[i] : 0) + rhs.m_pData[i] + carry;
		if (m_pData[i] >= 10)
		{
			carry = 1;
			m_pData[i] -= 10;
		}
		else
		{
			carry = 0;
		}
	}

	if (rhs.m_eleCount > m_eleCount)
	{
		m_eleCount = rhs.m_eleCount;

		if (carry)
		{
			m_pData[m_eleCount++] = 1;
		}
	}
	else
	{
		for (int i = rhs.m_eleCount; carry && i < m_eleCount; ++i)
		{
			m_pData[m_eleCount] += carry;
			if (m_pData[i] >= 10)
			{
				carry = 1;
				m_pData[i] -= 10;
			}
			else
			{
				carry = 0;
			}
		}
		if (carry)
		{
			m_pData[m_eleCount++] = 1;
		}
	}

	return *this;
}


CBigInt& CBigInt::UnsignedDec(const CBigInt& rhs)
{
	int biggerCount = (m_eleCount > rhs.m_eleCount ? m_eleCount : rhs.m_eleCount);
	if (m_dataLen < biggerCount)
	{
		m_dataLen = biggerCount;
		byte *t = new byte[m_dataLen];
		_ASSERT(t);
		for (int i = 0; i != m_eleCount; ++i)
		{
			t[i] = m_pData[i];
		}
		delete [] m_pData;
		m_pData = t;
	}

	const CBigInt *big, *small;
	if (UnsignedCmp(rhs) >= 0)
	{
		big = this;
		small = &rhs;
		m_sign = false;
	}
	else
	{
		big = &rhs;
		small = this;
		m_sign = true;
	}

	int borrow = 0, t;
	for(int i = 0; i != big->m_eleCount; ++i)
	{
		t = big->m_pData[i] - (i < small->m_eleCount ? small->m_pData[i] : 0) - borrow;
		if(t < 0)
		{
			borrow = 1;
			m_pData[i] = t + 10;
		}
		else
		{
			borrow = 0;
			m_pData[i] = t;
		}
	}

	m_eleCount = big->m_eleCount;
	while (m_eleCount > 0 && !m_pData[m_eleCount - 1])
	{
		--m_eleCount;
	}
	if(!m_eleCount)
	{
		++m_eleCount;
	}

	return *this;
}


int CBigInt::UnsignedCmp(const CBigInt& rhs) const
{
	// compares with length first
	int ret = m_eleCount - rhs.m_eleCount;
	if (ret)
	{
		return ret;
	}
		
	for (int i = m_eleCount - 1; i >= 0; --i)
	{
		ret = m_pData[i] - rhs.m_pData[i];
		if (ret)
		{
			break;
		}
	}

	return ret;
}


/*
	Description:
		Compares *this to rhs
	Parameters:
		rhs[in] - right operand that to be compared with
	Return Value:
		int - returns 0 if two numbers are equal. >0 if *this greater than rhs.
		<0 if *this less than rhs
*/
int CBigInt::Compare(const CBigInt& rhs) const
{
	// compares with sign first
	int ret = (rhs.m_sign - m_sign);
	if (ret)
	{
		return ret;
	}

	// compares to absolute num
	return (m_sign ? -1 : 1) * UnsignedCmp(rhs);
}


/*
	Description:
		Prefix increasement
	Parameters:
		none
	Return Value:
		CBigInt& - The num is increased
*/
CBigInt& CBigInt::operator ++()
{
	*this += 1;

	return *this;
}


CBigInt CBigInt::operator ++(int)
{
	CBigInt num(*this);
	++(*this);

	return num;
}


/*
	Description:
		Prefix decreasement
	Parameters:
		none
	Return Value:
		CBigInt& - The num is decreased
*/
CBigInt& CBigInt::operator --()
{
	*this -= 1;

	return *this;
}


CBigInt CBigInt::operator --(int)
{
	CBigInt num(*this);
	--(*this);
	
	return num;
}


const CBigInt operator *(const CBigInt& lhs, const CBigInt& rhs)
{
	CBigInt num(lhs);
	num *= rhs;

	return num;
}


CBigInt& CBigInt::operator *=(const CBigInt& rhs)
{
	m_sign ^= rhs.m_sign;
	
	// Allocates a temporary buffer to save the product
	// Expands buffer length if necessary
	int tempBuffLen = m_dataLen;
	int digitsMax = m_eleCount + rhs.m_eleCount;

	while (digitsMax >= tempBuffLen)
	{
		tempBuffLen = tempBuffLen * DATALEN_GROWING_RATE / 100;
	}

	byte* pTempBuff = new byte[tempBuffLen];
	_ASSERT(pTempBuff != NULL);

	// the digits of product will not great than the sum of digits of each
	// operand
	memset(pTempBuff, 0, digitsMax * sizeof(byte));

	// this * rhs
	for (int i = 0; i < m_eleCount; ++i)
	{
		int carry = 0;
		int j = 0;

		for (; j < rhs.m_eleCount; ++j)
		{
			int singleSum = m_pData[i] * rhs.m_pData[j] + carry;
			carry = singleSum / 10;
			int singleDigit = singleSum - carry * 10;

			// digitwise add-up
			// requires buffer initialized
			pTempBuff[i + j] += singleDigit;

			if (pTempBuff[i + j] >= 10)
			{
				pTempBuff[i + j] -= 10; 
				++carry;
			}
		}

		// extra digit
		if (carry)
		{
			pTempBuff[i + j] = carry;
		}
	}

	// replaces internal data
	delete [] m_pData;
	m_pData = pTempBuff;

	// updates
	m_dataLen = tempBuffLen;
	m_eleCount = digitsMax;

	while (!m_pData[m_eleCount - 1] && m_eleCount > 1)
	{
		--m_eleCount;
	}

	AdjustSignal();

	return *this;
}


const CBigInt operator /(const CBigInt& lhs, const CBigInt& rhs)
{
	CBigInt num(lhs);
	num /= rhs;

	return num;
}


int cmp(byte *p1 , byte *p2 ,int n)
{
	if (p1[n] != 0)
		return 1;
	for (--n; n >= 0; --n)
		if (p1[n] != p2[n])
			return p1[n] - p2[n];
	return 0;
}


void sub(byte *p1 , byte *p2 ,int n)
{
	int borrow = 0;
	for (int i = 0; i != n; ++i)
	{
		int t = p1[i] - p2[i] - borrow;
		if (t < 0){
			borrow = 1;
			p1[i] = t + 10;
		}else{
			p1[i] = t;
			borrow = 0;
		}
	}
	if (borrow)
		--p1[n];
}


CBigInt& CBigInt::operator /=(const CBigInt& rhs)
{
	if (rhs.m_eleCount == 1 && *rhs.m_pData == 0)
	{
		ErrorOutput(L"Arithmetic error.\n");
		return *this;
	}

	if (rhs.m_eleCount > m_eleCount)
	{
		m_eleCount = 1;
		*m_pData = 0;
		m_sign = 0;
		return *this;
	}

	bool signal = m_sign ^ rhs.m_sign;

	int predictLen = m_eleCount - rhs.m_eleCount + 1, len = DATALEN;
	while (len <= predictLen)
	{
		len = len * DATALEN_GROWING_RATE /100;
	}
	byte *result = new byte[len];
	_ASSERT(result);

	m_pData[m_eleCount] = 0;

	for (int i = predictLen - 1; i >= 0; --i)
	{
		int n = 0;
		while(cmp(m_pData + i, rhs.m_pData ,rhs.m_eleCount) >= 0){
			sub(m_pData + i, rhs.m_pData ,rhs.m_eleCount);
			++n;
		}
		result[i] = n;
	}
	if (result[predictLen - 1] == 0)
		--predictLen;

	delete [] m_pData;
	m_eleCount = predictLen;
	m_pData = result;
	m_dataLen = len;

	AdjustSignal();
	return *this;
}


bool operator ==(const CBigInt& lhs, const CBigInt& rhs)
{
	return (lhs.Compare(rhs) ? false : true);
}


bool operator !=(const CBigInt& lhs, const CBigInt& rhs)
{
	return (lhs.Compare(rhs) ? true : false);
}


bool operator >(const CBigInt& lhs, const CBigInt& rhs)
{
	return (lhs.Compare(rhs) > 0 ? true : false);
}


bool operator <(const CBigInt& lhs, const CBigInt& rhs)
{
	return (lhs.Compare(rhs) < 0 ? true : false);
}


bool operator >=(const CBigInt& lhs, const CBigInt& rhs)
{
	return (lhs.Compare(rhs) >= 0 ? true : false);
}


bool operator <=(const CBigInt& lhs, const CBigInt& rhs)
{
	return (lhs.Compare(rhs) <= 0 ? true : false);
}


/*
	Description:
		Sets a new capcity of the data buffer to enlarging or to shrinking
	Parameters:
		newSize[in] - Indicates new size of the buffer
	Return Value:
		bool - returns true if it is successful, otherwise false
*/
bool CBigInt::SetCapcity( int newSize /*= DATALEN*/ )
{
	// makes no sense
	if (newSize <= m_eleCount || newSize < DATALEN)
	{
		ErrorOutput(L"SetCapcity failed! Invalid newSize\n");
		return false;
	}

	byte* pTempBuff = new byte[newSize];
	_ASSERT(pTempBuff != NULL);

	errno_t err = memcpy_s(pTempBuff, sizeof(byte) * newSize, m_pData, sizeof(byte) * m_eleCount);

	if (err)
	{
		ErrorOutput(L"copy internal data faild in SetCapcity!\n");
		return false;
	}

	delete [] m_pData;
	m_pData = pTempBuff;

	m_dataLen = newSize;

	return true;
}


wstring CBigInt::ToString() const
{
	_ASSERT(m_eleCount > 0);
	wstring ret;
	
	if (m_sign)
	{
		ret += L'-';
	}

	for (int i = m_eleCount - 1; i >= 0; --i)
	{
		ret += m_pData[i] + L'0';
	}

	return ret;
}


void CBigInt::ToString(wchar_t* szBuffer, int bufferSize) const
{
	// buffer safety test
	_ASSERT(m_eleCount > 0);
	_ASSERT(bufferSize > m_eleCount);

	if (m_sign)
	{
		*szBuffer++ = L'-';
	}
	
	for (int i = m_eleCount - 1; i >=0; --i)
	{
		*szBuffer++ = m_pData[i] + L'0';
	}

	*szBuffer = NULL;
}


void CBigInt::Clear()
{
	if (!m_pData)
	{
		return;
	}

	delete [] m_pData;
	m_pData = NULL;

	m_dataLen = m_eleCount = 0;
	m_sign = false;
}