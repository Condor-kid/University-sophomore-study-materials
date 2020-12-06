#include <bits/stdc++.h>
#include <omp.h>
using namespace std;
typedef double lf;
typedef vector<lf> Vec;
typedef vector<vector<pair<int, lf> > > Mat;
int m, n, th;
Vec operator*(const Vec &v, const Mat &m)
{
	Vec r(m.size(), 0);
#pragma omp parallel for num_threads(th)
	for (int i = 0; i < m.size(); ++i)
		for (const auto &p : m[i])
			r[i] += v[p.first] * p.second;
	return r;
}
int main()
{
	ifstream fin("1138_bus.mtx");
	while (fin.peek() == '%')
		while (fin.get() != '\n')
			;
	fin >> m >> n >> th;
	Mat ma(m);
	for (int x, y, i = 0; i < th; ++i)
	{
		lf t;
		fin >> x >> y >> t;
		ma[x - 1].emplace_back(y - 1, t);
	}
	Vec ve(n);
	for (int i = 0; i < n; ++i)
		ve[i] = rand();
//	while(1)
//{
        cout << "number of threads: ";
	cin >> th;
	auto begin = std::chrono::system_clock::now();
	for (int i = 1e5; i; --i)
		ve *ma;
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - begin;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

//}
        
}










































