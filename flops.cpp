#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <future>
#include <algorithm>
#include <fmt/core.h>
#include <argparse/argparse.hpp>

const size_t MemSize = 64;
const size_t Iters = 400000;

template <typename RealType, size_t MemSize, size_t Iters>
RealType random_sum(std::random_device &rd)
{
    std::mt19937_64 gen(rd());
    std::normal_distribution<RealType> dist(0.0, 1.0);

    RealType sum = 0.0;
    std::vector<RealType> mem(MemSize, 1.0);
    for (size_t i = 0; i < Iters; ++i)
    {
        for (size_t j = 0; j < MemSize; j++)
        {
            mem[j] *= dist(gen);
            sum += mem[j];
        }
    }

    return sum;
}

int main(int argc, char **argv)
{
    argparse::ArgumentParser program("flops");
    program.add_argument("-t", "--threads")
        .default_value(1)
        .action([](auto value) { return std::stoi(value); });
    program.add_argument("-i", "--iterations")
        .default_value(1)
        .action([](auto value) { return std::stoi(value); });
    program.add_argument("-n", "--numeric-output")
        .default_value(false)
        .implicit_value(true);
    program.parse_args(argc, argv);

    auto thread_count = program.get<int>("--threads");
    auto iteration_count = program.get<int>("--iterations");
    std::random_device rd;

    for (auto iter = 0; iter < iteration_count; ++iter)
    {
        // launch threads
        std::vector<std::future<double>> fs;
        auto start = std::chrono::high_resolution_clock::now();
        for (auto i = 0; i < thread_count; ++i)
        {
            fs.emplace_back(std::async(std::launch::async, [&rd]() {
                return random_sum<double, MemSize, Iters>(rd);
            }));
        }
        auto sum = std::accumulate(fs.begin(), fs.end(), 0.0, [](auto s, auto &f) {
            return s + f.get();
        });
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0;
        auto ops = 2 * MemSize * Iters * thread_count / elapsed;

        if (program["--numeric-output"] == true)
        {
            fmt::print("{}\n", (unsigned long) ops);
        }
        else
        {
            fmt::print(
                "Iteration: {}\tSum: {:4.0f}\tElapsed: {:.03f}\tMOps(total): {:3.1f}\tMOps(core): {:3.1f}\n",
                iter + 1, std::abs(sum), elapsed, ops / 1000000, ops / thread_count / 1000000);
        }
    }
}
