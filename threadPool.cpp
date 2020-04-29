#include "threadPool.h"

void threadPool::addTask(std::function<void()> fm)
{
	std::lock_guard<std::mutex> lc(mu);
	mlTask.emplace_back(fm);
	mCV.notify_one();
}

void threadPool::start(int num)
{
	bWork = true;
	while (num--)
	{
		mlThread.emplace_back(std::make_shared<std::thread>(&threadPool::workThread, this));
	}
}

void threadPool::stop()
{
	{
		std::lock_guard<std::mutex> lc(mu);
		bWork = false;
		mCV.notify_all();
	}
	for (auto t : mlThread)
	{
		t->join();
	}
	mlThread.clear();
}

void threadPool::stopAfterAllWork()
{
	bool bSleep = true;
	while (bSleep)
	{
		{
			std::lock_guard<std::mutex> lc(mu);
			bSleep = !mlTask.empty();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
	stop();
}

void threadPool::workThread()
{
	std::function<void()> fm;
	while (bWork)
	{
		{
			std::unique_lock<std::mutex> lc(mu);
			while (mlTask.empty()&&bWork)
			{
				mCV.wait(lc);
			}
			if (!bWork)
				break;
			fm = mlTask.front();
			mlTask.pop_front();
		}
		fm();
	}
}
