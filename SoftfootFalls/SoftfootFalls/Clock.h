/**
 * @brief A clock that meaures duration in nanoseconds.
 * 
 * This is indented to be instantiated as a new pointer once at the
 * beginning of main as the first piece of new memory, and released
 * as the last piece of memory before main returns.
 */
class Clock
{
public:
	Clock();
	~Clock();
private:
	void Print();
	long long Now();
	long long start = 0L;
};
