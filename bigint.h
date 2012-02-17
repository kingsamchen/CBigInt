/***************************************

** Project:BigInt Class Design

** File:bigint.h

** Edition:v1.0.1 Demo

** Coder:Kingsam Chen¡¢Lei Yu [CodeThink]

** Last Modify:2011-8-13

****************************************/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _CBIGINT_89208EB6_3829_40ec_9569_7258E249101D
#define _CBIGINT_89208EB6_3829_40ec_9569_7258E249101D

#include <iostream>
#include <string>

using std::wstring;
using std::wcout;
typedef unsigned char byte;

class CBigInt
{
	public:
		CBigInt();
		CBigInt(int num);
		CBigInt(const wchar_t* pszNum);
		CBigInt(const CBigInt& num);
		virtual ~CBigInt();

	public:
		CBigInt& operator =(int num);
		CBigInt& operator =(const CBigInt& num);
		CBigInt& operator =(const wchar_t* pszNum);
		friend const CBigInt operator +(const CBigInt& lhs, const CBigInt& rhs);
		friend const CBigInt operator -(const CBigInt& lhs, const CBigInt& rhs);
		friend const CBigInt operator *(const CBigInt& lhs, const CBigInt& rhs);
		friend const CBigInt operator /(const CBigInt& lhs, const CBigInt& rhs);
		CBigInt& operator +=(const CBigInt& rhs);
		CBigInt& operator +=(int rhs);
		CBigInt& operator -=(const CBigInt& rhs);
		CBigInt& operator -=(int rhs);
		CBigInt& operator *=(const CBigInt& rhs);
		CBigInt& operator /=(const CBigInt& rhs);
		CBigInt& operator ++();
		CBigInt operator ++(int);
		CBigInt& operator --();
		CBigInt operator --(int);
		friend bool operator ==(const CBigInt& lhs, const CBigInt& rhs);
		friend bool operator !=(const CBigInt& lhs, const CBigInt& rhs);
		friend bool operator >(const CBigInt& lhs, const CBigInt& rhs);
		friend bool operator >=(const CBigInt& lhs, const CBigInt& rhs);
		friend bool operator <(const CBigInt& lhs, const CBigInt& rhs);
		friend bool operator <=(const CBigInt& lhs, const CBigInt& rhs);

	public:
		int Compare(const CBigInt& rhs) const;
		wstring ToString() const;
		void ToString(wchar_t* szBuffer, int bufferSize) const;
		inline const size_t Length() const;
		bool SetCapcity(int newSize = DATALEN);

	protected:
		void Clear();
		void FormatIntoInternalData(int num);
		void FormatIntoInternalData(const wchar_t* pszNum);
		void CopyInternalData(const CBigInt& num);
		wstring DealWithNum(const wchar_t* p);
		int UnsignedCmp(const CBigInt& rhs) const;
		CBigInt& UnsignedlInc(const CBigInt& rhs);
		CBigInt& UnsignedDec(const CBigInt& rhs);
		inline void AdjustSignal();

	protected:
		inline void ErrorOutput(wchar_t* pszErrorDescription);
		
	protected:
		enum{DATALEN = 20, DATALEN_GROWING_RATE = 150};  // rate is in percentage
		byte* m_pData;
		int m_dataLen;
		int m_eleCount;	 // m_eleCount is off-by-one to index
		bool m_sign;
};


// no null-terminated included
inline const size_t CBigInt::Length() const
{
	// negative sign will take up one
	_ASSERT(m_eleCount >= 0);
	return m_sign ? m_eleCount + 1 : m_eleCount;	
}


/*
	Description:
		Sets the sign to positiveness if the data is equal to 0
	Parameters:
		none
	Return Value:
		none
*/
inline void CBigInt::AdjustSignal()
{
	if (1 == m_eleCount && 0 == m_pData[0])
	{
		m_sign = false;
	}
}


// error output interface
inline void CBigInt::ErrorOutput(wchar_t* pszErrorDescription)
{
	wcout<<pszErrorDescription;
}

#endif