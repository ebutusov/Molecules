#pragma once

class CStringBlender
{
public:
	CStringBlender() { 
		m_calls_per_letter = 100;
		m_pos = 0;
		m_call_count = 0;
	}
	void SetString(const CString& refstrin, int calls);
	CString DoBlend();
private:
	CString m_str;
	int m_calls_per_letter;
	int m_pos;
	int m_call_count;
};