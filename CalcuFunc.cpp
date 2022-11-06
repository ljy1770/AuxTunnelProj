#include "stdafx.h"
#include "CalcuFunc.h"
#include <string>
#include <vector>

bool CalcuFunc::helper(std::wstring s1, std::wstring s2)
{
	if (s1[0] == s2[0]) {
		if (s1.length() > 1 && s2.length() > 1) return helper(s1.substr(1), s2.substr(1));
		else if (s1.length() == 1 && s2.length() == 1) return true;
		else return false;
	}
	else if (s1[0] == '?') {
		if (s1.length() > 1 && s2.length() > 1) return helper(s1.substr(1), s2.substr(1));
		else if (s1.length() == 1 && s2.length() == 1) return true;
		else return false;
	}
	else if (s1[0] == '*') {
		if (s1.length() > 1 && s2.length() > 1) return helper(s1.substr(1), s2) || 
			helper(s1.substr(1), s2.substr(1)) || helper(s1, s2.substr(1));
		else if (s1.length() == 1 && s2.length() == 1) return true;
		else if (s1.length() == 1 && s2.length() > 1) return true;
		else return helper(s1.substr(1), s2);
	}
	else return false;
	return true;
}

bool CalcuFunc::match(std::wstring& s, std::wstring& p, size_t i, size_t j)
{
	if (i < 0 || j < 0)
		return false;
	if (s[i] == p[j] || p[j] == '?')
		return true;
	return false;
}
bool CalcuFunc::isMatch(std::wstring s, std::wstring p)
{
	using namespace std;
	size_t m = s.length();
	size_t n = p.length();
	size_t i, j;
	vector<vector <int> >  dp(m + 1, vector<int>(n + 1, 0));
	dp[0][0] = true;  
	for (j = 1; j <= n; j++) 
	{
		if (p[j - 1] == '*')
		{
			dp[0][j] = true;
		}
		else {
			break;
		}
	}
	//×´Ì¬×ªÒÆ
	for (i = 1; i <= m; i++)
	{
		for (j = 1; j <= n; j++)
		{
			if (match(s, p, i - 1, j - 1))
			{
				dp[i][j] = dp[i - 1][j - 1];
			}
			else if (p[j - 1] == '*')
			{
				dp[i][j] = (dp[i - 1][j] || dp[i][j - 1]);
			}
		}
	}
	return dp[m][n];
}


//int main() {
//	string s1, s2;
//	while (cin >> s1) {
//		cin >> s2;
//		if (helper(s1, s2)) cout << "true" << endl;
//		else cout << "false" << endl;
//	}
//	return 0;
//}
