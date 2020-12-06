#ifndef _my_hpp_
#define _my_hpp_
#include <queue>
#include <mutex>
namespace Condor
{
template <class T>
class MultiAccessQueue : std::queue<T>
{
private:
	mutable std::mutex mu;

public:
	void push(T val)
	{
		std::lock_guard<std::mutex> guard(mu);
		std::queue<T>::push(val);
	}
	void pop()
	{
		std::lock_guard<std::mutex> guard(mu);
		std::queue<T>::pop();
	}
	T front() const
	{
		std::lock_guard<std::mutex> guard(mu);
		return std::queue<T>::front();
	}
};
} // namespace Condor
#endif































