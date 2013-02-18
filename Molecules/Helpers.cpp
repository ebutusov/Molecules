#include "StdAfx.h"
#include "Helpers.h"

void
CStringBlender::SetString(const CString &refstring, int calls)
{
	m_call_count = m_pos = 0;
	m_str = refstring;
	m_calls_per_letter = calls;
}

CString
CStringBlender::DoBlend()
{
	CString blended;
	m_call_count++;
	if(m_pos < m_str.GetLength())
	{
		if(m_pos)
			blended = m_str.Left(m_pos);
		TCHAR n = rand()%30+80;
		blended.Insert(m_pos, n);
		if(m_call_count == m_calls_per_letter)
		{
			m_pos++;
			m_call_count = 0;
		}
	}
	else
		return m_str;
	return blended;
}
