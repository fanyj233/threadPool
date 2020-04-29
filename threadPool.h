#pragma once
#include <thread>
#include <condition_variable>
#include <list>
#include <functional>
#include <atomic>
class threadPool
{
public:
	threadPool() = default;
	~threadPool() = default;
	void addTask(std::function<void ()>);
	void start(int num);
	void stop();
	void stopAfterAllWork();
private:
	std::list<std::function<void()>> mlTask;
	std::list<std::shared_ptr<std::thread>> mlThread;
	std::mutex mu;
	std::condition_variable mCV;
	std::atomic<bool> bWork;
	void workThread();
};

