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
#include <map>
#include <vector>
#include <string>
#include <functional>
NARUKAMI_BEGIN
class StatsAccumulator
{
private:
    std::map<std::string, uint64_t> _memory_counters;

public:
    void report_memory_counter(const std::string &name, const uint64_t count)
    {
        _memory_counters[name] += count;
    }

    void print(std::ostream &out) const{

        for (const auto &i : _memory_counters)
        {   
            static std::string sy_byte="byte";
            static std::string sy_kb="kb";
            static std::string sy_mb="mb";
            static std::string sy_gb="gb";

            if(i.second<1024){
                 out<<i.first<<" : "<<i.second<<sy_byte<<std::endl;
            }else if(i.second<1024*1024){
                out<<i.first<<" : "<<i.second/1024.0f<<sy_kb<<std::endl;
            }else if(i.second<1024*1024*1024){
                out<<i.first<<" : "<<i.second/(1024.0f*1024.0f)<<sy_mb<<std::endl;
            } else{
                out<<i.first<<" : "<<i.second/(1024.0f*1024.0f*1024.0f)<<sy_gb<<std::endl;
            }
        }
    }
};

class StatRegisterer
{
private:
    static std::vector<std::function<void(StatsAccumulator &)>> *_funcs;

public:
    StatRegisterer(std::function<void(StatsAccumulator &)> func){
        if(!_funcs){
            _funcs = new std::vector<std::function<void(StatsAccumulator &)>>();
        }

        (*_funcs).push_back(func);
    }

    ~StatRegisterer(){
        if(_funcs){
            delete _funcs;
            _funcs=nullptr;
        }
    }

    static void call_callback(StatsAccumulator& stats_acc){
        for (const auto &func : (*_funcs))
        {
            func(stats_acc);
        }
    }
};


#define STAT_MEMORY_COUNTER(name,var)\
static thread_local uint64_t var;\
static void stat_func_##var(StatsAccumulator& stats_acc){\
    stats_acc.report_memory_counter(name,var);\
    var=0;\
}\
static StatRegisterer stat_reg_##var(stat_func_##var);\

#define STAT_INCREASE_MEMORY_COUNTER(var,count)\
var+=count;\

//print statistics infos into ostream
void print_statistics(std::ostream&);

void report_thread_statistics();

NARUKAMI_END