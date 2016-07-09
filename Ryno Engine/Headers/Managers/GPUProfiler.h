#pragma once
#include "Global.h"
#include <vector>

namespace RynoEngine{
	class GPUProfiler{
		struct Times{
			Times(){}
			U32 get_elapsed_time(){
				return end - start;
			}
			U64 start;
			U64 end;
			U32 query_id_0;
			U32 query_id_1;
		};
	public:
		//n is the number of samples, helps to reallocate before.
		static void begin();
		static void start_time();
		//calls start_time and end_time.
		//useful to track multiple times one after the other
		static void next_time(); 
		static void end_time();
		static void print_time();
		static void reset();

	private:
		static std::vector<Times> times;
		static U8 current_time;
	};
}