/*
MIT License

Copyright (c) 2019 ZhuQian

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#include "core/narukami.h"
#include <atomic>
#include <chrono>
#include <thread>
#include <algorithm>
#include <string>
NARUKAMI_BEGIN
//进度条
class ProgressReporter{
private:
	std::atomic<uint64_t> _num_done;//已经完成了个工作数
	uint64_t _num_total;//工作数总数
	const std::string _title;//进度条标题
	const std::chrono::system_clock::time_point _start_time;
	std::atomic<bool> _thread_exit;
	std::thread _update_thread;
	void print_bar(){
		//线程的睡眠时间
		std::chrono::milliseconds sleep_duration(500);
		char buf[128];
		//char buf2[128];
		for(int i=0;i<128;++i){
			buf[i]=' ';
			//buf2[i]='\b';
		}

		int title_size=_title.size()+1;
		snprintf(buf,title_size,"%s",_title.c_str());
		buf[_title.size()]='[';
		buf[_title.size()+51]=']';
		//到这里已经写了titleSize+52个字符了
		char* barStart=&buf[_title.size()+1];
		int it = 0;
		while(!_thread_exit){
			if (it == 100) {
				sleep_duration = std::chrono::milliseconds(2000);
			}
			std::this_thread::sleep_for(sleep_duration);
			float percent=(float)(_num_done)/(_num_total);
			float num=50*percent;
			for(int i=0;i<num;++i){
				barStart[i]='+';
			}

			//统计剩余时间
			 std::chrono::system_clock::time_point now =std::chrono::system_clock::now();
			 int64_t ep= std::chrono::duration_cast<std::chrono::milliseconds>(now-_start_time).count();
			 int32_t seconds=static_cast<int32_t>(ep/1000);//已经经过的时间
			 int32_t s=seconds%60;
			 int32_t m=seconds/60%60;
			 int32_t h=seconds/3600;
		
			 int32_t nokotaSeconds=static_cast<int32_t>(seconds/percent-seconds);//剩余的时间
			 int32_t ns=nokotaSeconds%60;
			 int32_t nm=nokotaSeconds/60%60;
			 int32_t nh=nokotaSeconds/3600;

			 snprintf(buf+52+_title.size(),34+6,"[percent:%.1f|%2d:%2d:%2d=>%2d:%2d:%2d]",percent,nh,nm,ns,h,m,s);//seconds,nokotaSeconds);
			 buf[_title.size()+51+34+6+1]='\0';
			 //buf2[_title.size()+51+34+6+1]='\0';
			 printf("\r");
			 fputs(buf, stdout);
			 fflush(stdout);
			 it++;
		}
		printf("\n");
	}
public:
	ProgressReporter(uint64_t totalNum,const std::string& title):_num_total(std::max((uint64_t)1,totalNum)),_title(title),_start_time(std::chrono::system_clock::now()){
		_num_done=0;
		_thread_exit=false;
		//开启更新进程
		_update_thread=std::thread([this](){
			this->print_bar();
		});
	}
	~ProgressReporter(){
		_num_done = _num_total;
		_thread_exit = true;
		_update_thread.join();
	}
	//更新进度
	void update(int num=1){
		assert(num>=1);
		//更新当前完成任务个数
		_num_done+=num;
	}
	//完成进度
	void done(){
		_num_done=_num_total;
	}
};
NARUKAMI_END