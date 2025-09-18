class Timer
{
public:
	static void Print();
	Timer(std::function<void()>, int);
	~Timer();
	void Update();
private:
	const static int size = 10;
	static std::atomic<bool> isReady;
	static std::atomic<int> index;
	static std::atomic<int> framesPerSecond;
	static std::atomic<int> framesPerSeconds[size];
};