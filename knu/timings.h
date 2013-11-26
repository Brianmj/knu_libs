#ifndef KNU_TIMINGS
#define KNU_TIMINGS

// Created 8/29/2013

#include <chrono>
namespace knu
{
	class Timings
	{
		std::chrono::high_resolution_clock::time_point t1;
		std::chrono::high_resolution_clock::time_point t2;

	public:
		void time_stamp_1() { t1 = std::chrono::high_resolution_clock::now(); }
		void time_stamp_2() { t2 = std::chrono::high_resolution_clock::now(); }

		std::chrono::seconds diff_sec() { return std::chrono::duration_cast<
			std::chrono::seconds>(t2 - t1); }

		std::chrono::milliseconds diff_mill() {
			return std::chrono::duration_cast<
				std::chrono::milliseconds> (t2 - t1);
		}

		std::chrono::nanoseconds diff_nano() {
			return t2 - t1;
		}

	};
}

#endif