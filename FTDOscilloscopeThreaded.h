#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include "ftd2xx.h"

class FTDReader;
class FTDWriter;

class FTDOscilloscopeThreaded {
public:
	using Clock = std::chrono::steady_clock;
	
	FTDOscilloscopeThreaded(FTDReader* reader, FTDWriter* writer);
	~FTDOscilloscopeThreaded();
	
	void start();
	void stop();
	void join();
	
	void collectDataWithWait(int sampleIntervalMicros, int waitTimeMicros, std::mutex& out_mx);
	
	bool isRunning() const { return running.load(std::memory_order_relaxed); }
	bool done() const { return done_.load(std::memory_order_relaxed); }
	Clock::time_point deadline() const { return deadline_; }
	
	const std::vector<unsigned char>& getCollectedData() const;
	void clearData();
	
	size_t getSampleCount() const { return sampleCount.load(std::memory_order_relaxed); }
	
private:
	FTDReader* reader;
	FTDWriter* writer;
	
	std::atomic<bool> running;
	std::atomic<bool> stop_;
	std::atomic<bool> done_;
	std::atomic<size_t> sampleCount;
	std::vector<unsigned char> collectedData;
	mutable std::mutex dataMutex;
	
	std::thread collectionThread;
	std::thread displayThread;
	
	int currentSampleIntervalMicros;
	int currentWaitTimeMicros;
	Clock::time_point deadline_;
	
	void collectionThreadFunction(std::mutex& out_mx);
	void displayThreadFunction(std::mutex& out_mx);
	
	void updateDisplay(const std::vector<unsigned char>& latestData, std::mutex& out_mx);
};

