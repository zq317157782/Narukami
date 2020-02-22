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
#include "parallel.h"
#include "core/stat.h"
#include <vector>
NARUKAMI_BEGIN

class ParallelForLoop
{
public:
    const std::function<void(size_t)> func_1D;
    const std::function<void(Point2i)> func_2D;
    const size_t max_iteration_count;
    const size_t chunk_size;
    size_t next_iteration_index;
    int active_worker;
    ParallelForLoop *next; //next ParallelForLoop instance
    const size_t num_x = -1;

public:
    ParallelForLoop(std::function<void(size_t)> func, const size_t iteration_count, const size_t chunk_size) : func_1D(std::move(func)), max_iteration_count(iteration_count), chunk_size(chunk_size), active_worker(0), next_iteration_index(0) {}
    ParallelForLoop(std::function<void(Point2i)> func, const Point2i &count) : func_2D(std::move(func)), max_iteration_count(count.x * count.y), chunk_size(1), active_worker(0), next_iteration_index(0), num_x(count.x) {}
};

bool is_finished(const ParallelForLoop &loop)
{
    if (loop.next_iteration_index == loop.max_iteration_count && loop.active_worker == 0)
    {
        return true;
    }
    return false;
}

//threads pool
static std::vector<std::thread> threads;
//link list for ParallelForLoop instance;
static ParallelForLoop *work_list = nullptr;
static std::mutex work_list_mutex;
static std::condition_variable work_list_condition;

static bool shutdown_threads = false;
thread_local int thread_index = 0;
void thread_worker_func(int index)
{
    thread_index = index;

    // lock for the work list
    std::unique_lock<std::mutex> lock(work_list_mutex);

    while (!shutdown_threads)
    {
        if (work_list == nullptr)
        {
            //if no works to do,release lock and just sleep for other wakeup
            work_list_condition.wait(lock);
        }
        else
        {
            ParallelForLoop &loop = *work_list;
            //get the iteration range
            size_t begin_index = loop.next_iteration_index;
            size_t end_index = min(begin_index + loop.chunk_size, loop.max_iteration_count);
            //update loop's next index
            loop.next_iteration_index = end_index;
            //switch to next loop
            if (loop.next_iteration_index == loop.max_iteration_count)
            {
                work_list = loop.next;
            }
            //increase loop's active_worker
            loop.active_worker++;

            lock.unlock();
            //--------------loop----------------------
            for (size_t i = begin_index; i < end_index; ++i)
            {
                if (loop.func_1D)
                {
                    loop.func_1D(i);
                }
                else if (loop.func_2D)
                {
                    loop.func_2D(Point2i(i % loop.num_x, i / loop.num_x));
                }
            }
            //--------------loop----------------------
            lock.lock();
            loop.active_worker--;
            if (is_finished(loop))
            {
                //wake up threads
                work_list_condition.notify_all();
            }
        }
    }
    //push thread data into stat
    report_thread_statistics();
}

void setup_threads(int num_core)
{
    for (int i = 0; i < num_core; ++i)
    {
        threads.push_back(std::thread(thread_worker_func, i + 1));
    }
}

void parallel_for(std::function<void(size_t)> func, const size_t count, const size_t chunk_size)
{
    //if cpu's core eual one or count le chunk size
    int num_core = num_system_core();
    if (num_system_core() == 1 || count <= chunk_size)
    {
        for (size_t i = 0; i < count; ++i)
        {
            func(i);
        }
        return;
    }

    //check threads
    if (threads.size() == 0)
    {
        setup_threads(num_core);
    }

    //create on stack
    ParallelForLoop loop(func, count, chunk_size);
    // lock for the work list
    std::unique_lock<std::mutex> lock(work_list_mutex);
    loop.next = work_list;
    work_list = &loop;
    lock.unlock();

    //wake up threads
    work_list_condition.notify_all();

    lock.lock();
    while (!is_finished(loop))
    {
        //get the iteration range
        size_t begin_index = loop.next_iteration_index;
        size_t end_index = min(begin_index + loop.chunk_size, loop.max_iteration_count);
        //update loop's next index
        loop.next_iteration_index = end_index;
        //switch to next loop
        if (loop.next_iteration_index == loop.max_iteration_count)
        {
            work_list = loop.next;
        }
        //increase loop's active_worker
        loop.active_worker++;

        lock.unlock();
        //--------------loop----------------------
        for (size_t i = begin_index; i < end_index; ++i)
        {
            loop.func_1D(i);
        }
        //--------------loop----------------------
        lock.lock();
        loop.active_worker--;
    }
}
void parallel_for_2D(std::function<void(Point2i)> func, const Point2i &count)
{
    //if cpu's core eual one or count le chunk size
    int num_core = num_system_core();
    if (num_system_core() == 1)
    {
        for (int y = 0; y < count.y; ++y)
        {
            for (int x = 0; x < count.x; ++x)
            {
                func(Point2i(x, y));
            }
        }
        return;
    }

    //check threads
    if (threads.size() == 0)
    {
        setup_threads(num_core);
    }

    //create on stack
    ParallelForLoop loop(func, count);
    // lock for the work list
    std::unique_lock<std::mutex> lock(work_list_mutex);
    loop.next = work_list;
    work_list = &loop;
    lock.unlock();

    //wake up threads
    work_list_condition.notify_all();

    lock.lock();
    while (!is_finished(loop))
    {
        //get the iteration range
        size_t begin_index = loop.next_iteration_index;
        size_t end_index = min(begin_index + loop.chunk_size, loop.max_iteration_count);
        //update loop's next index
        loop.next_iteration_index = end_index;
        //switch to next loop
        if (loop.next_iteration_index == loop.max_iteration_count)
        {
            work_list = loop.next;
        }
        //increase loop's active_worker
        loop.active_worker++;

        lock.unlock();
        //--------------loop----------------------
        for (size_t i = begin_index; i < end_index; ++i)
        {
            loop.func_2D(Point2i(i % loop.num_x, i / loop.num_x));
        }
        //--------------loop----------------------
        lock.lock();
        loop.active_worker--;
    }
}

void parallel_for_clean()
{
    if (threads.size() == 0)
    {
        return;
    }

    {
        // lock for the work list
        std::lock_guard<std::mutex> lock_guard(work_list_mutex);
        shutdown_threads = true;
        //wake up threads
        work_list_condition.notify_all();
    }

    //wait threads
    for (std::thread &t : threads)
    {
        t.join();
    }
    //clear
    threads.erase(threads.begin(), threads.end());
    shutdown_threads = false; //ready for next parallel_for
}

NARUKAMI_END