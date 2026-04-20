#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <numeric>
#include <algorithm>
#include <cstdint>
#include <windows.h>
#include "thread_pool.hpp"

using namespace wan::pool;
using namespace std::chrono;

struct Scenario
{
    std::string name;
    std::size_t threads;
    std::size_t tasks;
    std::size_t burst;
    int sleep_ms;
    int cpu_iters;
    bool use_priority;
    bool use_delay;
    bool monitor;
    bool profile;
};
struct Result
{
    std::string name;
    double total_ms;
    double throughput;
    double avg;
    double p50;
    double p95;
    double p99;
    std::size_t peak_queue;
    std::size_t current_queue;
    std::size_t active_threads;
    std::size_t total_threads;
    double queue_utilization;
};

static int do_work(int sleep_ms, int cpu_iters)
{
    (void)sleep_ms; // 明确不使用`sleep_ms`
    std::uint64_t sum = 0;
    for (int k = 0; k < cpu_iters; ++k)
    {
        sum += (static_cast<std::uint64_t>(k) * 17ull) + 31ull;
        sum ^= (sum << 7);
        sum *= 0x9E3779B97F4A7C15ull;
    }
    // 返回低位，防止循环被优化消除，同时保持函数签名
    return static_cast<int>(sum & 0x7fffffff);
    if (sleep_ms > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
    return static_cast<int>(sum & 0x7fffffff);
}

static Result run(const Scenario &s)
{
    auto pool = make_thread_pool(s.threads, std::max<std::size_t>(s.tasks, 10000));
    if (!pool)
        return {s.name, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (s.monitor)
    {
        pool->set_statistics_handler([](const pool_statistics &) {});
    }
    pool->set_event_handler([](const std::string &, const std::string &) {});
    if (!pool->start())
        return {s.name, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<std::future<int>> futures;
    futures.reserve(s.tasks);
    std::vector<steady_clock::time_point> submit_times;
    submit_times.reserve(s.tasks);
    auto bench_start = steady_clock::now();
    for (std::size_t i = 0; i < s.tasks; ++i)
    {
        submit_times.push_back(steady_clock::now());
        /**
         * @brief 构造`纯CPU`任务，固定`sleep_ms=0`。
         * @details 排除`sleep`与`delay`对测量的影响，仅评估`CPU`迭代的吞吐与延迟。
         */
        auto fn = [cpu = s.cpu_iters]
        {
            return do_work(0, cpu);
        };
        if (s.use_delay)
        {
            futures.emplace_back(pool->submit_delayed(std::chrono::milliseconds(s.sleep_ms), fn));
        }
        else if (s.use_priority)
        {
            futures.emplace_back(pool->submit_priority(weight::high, fn));
        }
        else
        {
            futures.emplace_back(pool->submit(fn));
        }
        if (s.burst > 0 && (i + 1) % s.burst == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    std::vector<double> lat;
    lat.reserve(futures.size());
    for (std::size_t i = 0; i < futures.size(); ++i)
    {
        auto v = futures[i].get();
        (void)v;
        auto end = steady_clock::now();
        lat.push_back((double)duration_cast<microseconds>(end - submit_times[i]).count() / 1000.0);
    }
    auto bench_end = steady_clock::now();
    auto total_ms = (double)duration_cast<milliseconds>(bench_end - bench_start).count();
    double throughput = futures.empty() ? 0.0 : (futures.size() / (total_ms / 1000.0));
    std::sort(lat.begin(), lat.end());
    double avg = lat.empty() ? 0.0 : std::accumulate(lat.begin(), lat.end(), 0.0) / lat.size();
    auto pct = [&](double p)
    { if(lat.empty()) return 0.0; if(p<0.0) p=0.0; if(p>1.0) p=1.0; std::size_t idx=(std::size_t)(p*(lat.size()-1)); if(idx>=lat.size()) idx=lat.size()-1; return lat[idx]; };
    const auto &stats = pool->get_statistics();
    Result r;
    r.name = s.name;
    r.total_ms = total_ms;
    r.throughput = throughput;
    r.avg = avg;
    r.p50 = pct(0.50);
    r.p95 = pct(0.95);
    r.p99 = pct(0.99);
    r.peak_queue = stats._peak_queue_size.load();
    r.current_queue = stats._current_queue_size.load();
    r.active_threads = pool->get_active_thread_count();
    r.total_threads = pool->get_thread_count();
    r.queue_utilization = pool->get_rank_utilization();
    pool->shutdown(std::chrono::milliseconds(2000));
    return r;
}

static void print_result(const Result &r)
{
    std::cout << r.name << " 总耗时(ms)=" << r.total_ms << " 吞吐(任务/秒)=" << r.throughput << " 平均延迟(ms)=" << r.avg << " P50(ms)=" << r.p50 << " P95(ms)=" << r.p95 << " P99(ms)=" << r.p99 << " 队列峰值=" << r.peak_queue << " 当前队列=" << r.current_queue << " 活跃线程=" << r.active_threads << " 总线程=" << r.total_threads << " 队列利用率=" << r.queue_utilization << "\n";
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    std::size_t repeat = 3;
    std::vector<Scenario> cases;
    /**
     * @brief 仅构造`计算密集型`场景。
     * @details 固定`sleep_ms=0`、`use_delay=false`，并将`burst=0`避免提交阶段的节流`sleep`。
     *          通过调整`cpu_iters`与`threads`来控制负载强度与并发度。
     */
    cases.push_back({"FIFO-CPU1000万-8线程", 5, 10000000, 0, 0, 1000, false, false, false, false});
    cases.push_back({"FIFO-CPU1000万-16线程", 20, 10000000, 0, 0, 1000, false, false, false, false});
    cases.push_back({"优先级-CPU2000万-16线程", 10, 20000000, 0, 0, 1000, true, false, true, true});
    for (const auto &c : cases)
    {
        double sum_ms = 0.0;
        double sum_thr = 0.0;
        std::vector<double> p50s;
        std::vector<double> p95s;
        std::vector<double> p99s;
        double sum_avg = 0.0;
        std::size_t pk = 0;
        std::size_t cq = 0;
        std::size_t act = 0;
        std::size_t th = 0;
        double util = 0.0;
        for (std::size_t r = 0; r < repeat; ++r)
        {
            auto res = run(c);
            sum_ms += res.total_ms;
            sum_thr += res.throughput;
            sum_avg += res.avg;
            p50s.push_back(res.p50);
            p95s.push_back(res.p95);
            p99s.push_back(res.p99);
            pk = std::max(pk, res.peak_queue);
            cq = res.current_queue;
            act = res.active_threads;
            th = res.total_threads;
            util = res.queue_utilization;
        }
        std::sort(p50s.begin(), p50s.end());
        std::sort(p95s.begin(), p95s.end());
        std::sort(p99s.begin(), p99s.end());

        double p50 = p50s[p50s.size() / 2];
        double p95 = p95s[p95s.size() / 2];
        double p99 = p99s[p99s.size() / 2];

        /**
         * @brief 用平均总耗时推导吞吐，确保与任务总数一致
         * @details `throughput_consistent = tasks / (avg_total_ms / 1000.0)`
         */
        double avg_total_ms = sum_ms / repeat;
        double throughput_consistent = c.tasks / (avg_total_ms / 1000.0);

        Result agg{c.name, avg_total_ms, throughput_consistent, sum_avg / repeat, p50, p95, p99, pk, cq, act, th, util};
        print_result(agg);
    }
    return 0;
}