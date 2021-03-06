#ifndef KNU_TIMINGS
#define KNU_TIMINGS

// Created 8/29/2013

#include <chrono>
#include <vector>
#include <algorithm>

namespace knu
{
	namespace Time
	{
		class Timings
		{
			std::chrono::high_resolution_clock::time_point t1;
			std::chrono::high_resolution_clock::time_point t2;

		public:
			void time_stamp_1() { t1 = std::chrono::high_resolution_clock::now(); }
			void time_stamp_2() { t2 = std::chrono::high_resolution_clock::now(); }

			std::chrono::seconds diff_sec() {
				return std::chrono::duration_cast<
					std::chrono::seconds>(t2 - t1);
			}

			std::chrono::milliseconds diff_mill() {
				return std::chrono::duration_cast<
					std::chrono::milliseconds> (t2 - t1);
			}

			std::chrono::nanoseconds diff_nano() {
				return t2 - t1;
			}

			void mark_t1_as_t2() { t1 = t2; }
		};

		class Fps
		{
			Timings timing;
			int fps;
			int lastFps;

		public:
			Fps() :timing(), fps(0), lastFps(0)
			{
				timing.time_stamp_1();
				timing.time_stamp_2();
			}

			inline void update()
			{
				timing.time_stamp_2();
				++fps;

				if (timing.diff_mill() >= std::chrono::milliseconds(1000))
				{
					// reset
					lastFps = fps;
					fps = 0;
					timing.mark_t1_as_t2();
				}
			}

			inline int get_fps() const { return lastFps; }
		};
		class Mpf
		{
		public:

			Mpf() :
				lastTime(std::chrono::high_resolution_clock::now()),
				currentTime(std::chrono::high_resolution_clock::now()),
				aSecond(2)
			{
				totalTime = currentTime - lastTime;
				times.reserve(1000);
			}
			void tick()
			{
				currentTime = std::chrono::high_resolution_clock::now();
				totalTime += (currentTime - lastTime);
				times.push_back(currentTime - lastTime);
				if (totalTime >= aSecond)
				{
					auto tt = std::accumulate(std::begin(times), std::end(times), std::chrono::duration<float, std::milli>());

					averageMs = (float)std::chrono::duration_cast<std::chrono::milliseconds>(tt).count() / times.size();

					auto maxi = std::max_element(std::begin(times), std::end(times));
					auto mini = std::min_element(std::begin(times), std::end(times));

					if (maxi != std::end(times))
						high = *maxi;

					if (mini != std::end(times))
						low = *mini;

					times.clear();
					totalTime = std::chrono::milliseconds(0);
				}

				lastTime = currentTime;
			}

			float maximum_ms() const { return high.count(); }
			float minimum_ms() const { return low.count(); }
			float average_ms() const { return averageMs; }
		private:
			std::chrono::high_resolution_clock::time_point lastTime, currentTime;
			std::chrono::seconds aSecond;
			std::chrono::high_resolution_clock::duration totalTime;
			float averageMs;
			std::chrono::duration<float, std::milli> high, low;
			std::vector<std::chrono::duration<float, std::milli>> times;
		};
	}
}

#endif