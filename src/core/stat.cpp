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
#include "core/stat.h"
#include <mutex>
NARUKAMI_BEGIN
static StatsAccumulator stats_accumulator;

std::vector<std::function<void(StatsAccumulator &)>> *StatRegisterer::_funcs = nullptr;

static void _split(const std::string &s, char delim,
                   std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    _split(s, delim, elems);
    return elems;
}

void StatsAccumulator::report_counter(const std::string &name, const uint64_t count)
{
    auto tokens = split(name, '/');
    if (tokens.size() == 2)
    {
        _stats[tokens[0]].counters[tokens[1]] += count;
    }
    else
    {
        _stats["global"].counters[name] += count;
    }
}

void StatsAccumulator::report_memory_counter(const std::string &name, const uint64_t count)
{
    auto tokens = split(name, '/');
    if (tokens.size() == 2)
    {
        _stats[tokens[0]].memory_counters[tokens[1]] += count;
    }
    else
    {
        _stats["global"].memory_counters[name] += count;
    }
}

void StatsAccumulator::report_percent(const std::string &name, const uint64_t num, const uint64_t denom)
{
    auto tokens = split(name, '/');
    if (tokens.size() == 2)
    {
        _stats[tokens[0]].percents[tokens[1]].first += num;
        _stats[tokens[0]].percents[tokens[1]].second += denom;
    }
    else
    {
        _stats["global"].percents[name].first += num;
        _stats["global"].percents[name].second += denom;
    }

}

void print_statistics(std::ostream &out)
{
    stats_accumulator.print(out);
}

void report_thread_statistics()
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    StatRegisterer::call_callback(stats_accumulator);
}
NARUKAMI_END