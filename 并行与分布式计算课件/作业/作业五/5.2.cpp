#include <chrono>
#include <iostream>
#include "my.hpp"
Condor::MultiAccessQueue<int> q;
void producer(int cnt)
{
	for (int i = 0; i < cnt; ++i)
		q.push(i);
}
void consumer(int cnt)
{
	for (int i = 0; i < cnt; ++i)
		q.pop();
}
int main()
{
	int num;
	std::cout << "number of producer-consumers: ";
	std::cin >> num;
	auto begin = std::chrono::system_clock::now();
#pragma omp parallel for
	for (int i = 0; i < num; ++i)
		producer(1000);
#pragma omp parallel for
	for (int i = 0; i < num; ++i)
		consumer(1000);
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - begin;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
}






























