#include "FTDOscilloscopeThreaded.h"
#include "FTDReader.h"
#include "FTDWriter.h"
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <iomanip>

FTDOscilloscopeThreaded::FTDOscilloscopeThreaded(FTDReader* reader, FTDWriter* writer)
	: reader(reader), writer(writer), running(false), stop_(false), done_(false), sampleCount(0), 
	  currentSampleIntervalMicros(0), currentWaitTimeMicros(0) {
	if (!reader || !writer) {
		throw std::runtime_error("FTDOscilloscopeThreaded: reader and writer must not be null");
	}
}

FTDOscilloscopeThreaded::~FTDOscilloscopeThreaded() {
	stop();
	join();
}

void FTDOscilloscopeThreaded::start() {
	if (running.load(std::memory_order_relaxed)) {
		return;
	}
	
	running = true;
	stop_ = false;
	done_ = false;
	sampleCount = 0;
	{
		std::lock_guard<std::mutex> lock(dataMutex);
		collectedData.clear();
	}
}

void FTDOscilloscopeThreaded::stop() {
	stop_.store(true, std::memory_order_relaxed);
	running.store(false, std::memory_order_relaxed);
}

void FTDOscilloscopeThreaded::join() {
	if (collectionThread.joinable()) {
		collectionThread.join();
	}
	if (displayThread.joinable()) {
		displayThread.join();
	}
}

void FTDOscilloscopeThreaded::collectDataWithWait(int sampleIntervalMicros, int waitTimeMicros, std::mutex& out_mx) {
	if (!running.load(std::memory_order_relaxed)) {
		start();
	}
	
	if (sampleIntervalMicros <= 0) {
		throw std::invalid_argument("Sample interval must be greater than zero microseconds");
	}
	if (waitTimeMicros <= 0) {
		throw std::invalid_argument("Wait time must be greater than zero microseconds");
	}
	
	currentSampleIntervalMicros = sampleIntervalMicros;
	currentWaitTimeMicros = waitTimeMicros;
	deadline_ = Clock::now() + std::chrono::microseconds(waitTimeMicros);
	
	{
		std::lock_guard<std::mutex> lk(out_mx);
		std::cout << "[scope] Starting data collection..." << std::endl;
		std::cout << "[scope] Sample interval: " << sampleIntervalMicros << " microseconds" << std::endl;
		std::cout << "[scope] Collection duration: " << (waitTimeMicros / 1000000.0) << " seconds" << std::endl;
	}
	
	collectionThread = std::thread(&FTDOscilloscopeThreaded::collectionThreadFunction, this, std::ref(out_mx));
	displayThread = std::thread(&FTDOscilloscopeThreaded::displayThreadFunction, this, std::ref(out_mx));
	
	collectionThread.join();
	displayThread.join();
	
	{
		std::lock_guard<std::mutex> lk(out_mx);
		std::cout << "\n[scope] Data collection completed." << std::endl;
		std::cout << "[scope] Total samples: " << sampleCount.load(std::memory_order_relaxed) << std::endl;
		std::cout << "[scope] Total bytes: " << collectedData.size() << std::endl;
	}
}

void FTDOscilloscopeThreaded::collectionThreadFunction(std::mutex& out_mx) {
	while (!stop_.load(std::memory_order_relaxed)) {
		auto now = Clock::now();
		if (now >= deadline_) {
			break;
		}
		
		try {
			reader->read();
			auto buffer = reader->getBuffer();
			
			{
				std::lock_guard<std::mutex> lock(dataMutex);
				for (unsigned char byte : buffer) {
					collectedData.push_back(byte);
				}
			}
			
			sampleCount.fetch_add(1, std::memory_order_relaxed);
			
		} catch (const std::exception& e) {
			std::lock_guard<std::mutex> lk(out_mx);
			std::cerr << "[scope] Error during data collection: " << e.what() << std::endl;
		}
		
		if (Clock::now() < deadline_) {
			std::this_thread::sleep_for(std::chrono::microseconds(currentSampleIntervalMicros));
		}
	}
	
	done_.store(true, std::memory_order_relaxed);
	{
		std::lock_guard<std::mutex> lk(out_mx);
		std::cout << "[scope] Collection thread done" << std::endl;
	}
}

void FTDOscilloscopeThreaded::displayThreadFunction(std::mutex& out_mx) {
	auto next_tick = Clock::now() + std::chrono::milliseconds(100);
	
	while (!stop_.load(std::memory_order_relaxed) && Clock::now() < deadline_) {
		auto now = Clock::now();
		
		if (now >= next_tick) {
			std::vector<unsigned char> latestData;
			size_t currentCount;
			
			{
				std::lock_guard<std::mutex> lock(dataMutex);
				latestData = collectedData;
				currentCount = sampleCount.load(std::memory_order_relaxed);
			}
			
			if (!latestData.empty() || currentCount > 0) {
				updateDisplay(latestData, out_mx);
			}
			
			next_tick += std::chrono::milliseconds(100);
			if (next_tick > deadline_) next_tick = deadline_;
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void FTDOscilloscopeThreaded::updateDisplay(const std::vector<unsigned char>& latestData, std::mutex& out_mx) {
	static size_t lastDisplaySize = 0;
	static size_t lastSampleCount = 0;
	
	size_t currentSampleCount = sampleCount.load(std::memory_order_relaxed);
	size_t displayCount = latestData.size();
	
	if (displayCount == lastDisplaySize && currentSampleCount == lastSampleCount && displayCount > 0) {
		return;
	}
	
	size_t startIdx = (displayCount > 16) ? (displayCount - 16) : 0;
	
	{
		std::lock_guard<std::mutex> lk(out_mx);
		std::cout << "\r[scope] samples=" << std::setw(6) << currentSampleCount 
		          << ", bytes=" << std::setw(6) << displayCount
		          << ", latest: ";
		
		for (size_t i = startIdx; i < latestData.size() && i < startIdx + 16; i++) {
			std::cout << std::hex << std::setfill('0') << std::setw(2) 
			          << static_cast<int>(latestData[i]) << " ";
		}
		
		std::cout << std::dec << std::setfill(' ') << std::flush;
	}
	
	lastDisplaySize = displayCount;
	lastSampleCount = currentSampleCount;
}

const std::vector<unsigned char>& FTDOscilloscopeThreaded::getCollectedData() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return collectedData;
}

void FTDOscilloscopeThreaded::clearData() {
	std::lock_guard<std::mutex> lock(dataMutex);
	collectedData.clear();
	sampleCount.store(0, std::memory_order_relaxed);
}

