import matplotlib.pyplot as plt
import subprocess
import time

FLOPS_BIN = "./build-rel/flops"

n_iterations = [1, 10]
n_cores = [1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64]
n_cooldown = 0

def flops_case(iters, cores):
    flops_exec = subprocess.Popen(
        [FLOPS_BIN, "-i", str(iters), "-t", str(cores), "-n"],
        stdout=subprocess.PIPE)
    flops_exec.wait()
    flops_exec.
    ops = list(int(s.strip()) for s in flops_exec.stdout.readlines())
    assert len(ops) == iters, "iterations do not match outputs. flops must have crashed."
    return sum(ops) / len(ops)


for i in n_iterations:
    mops_total = []
    mops_core = []
    for c in n_cores:
        mops = flops_case(i, c) / 1000000
        mops_total.append(mops)
        mops_core.append(mops / c)
        print("I:{} C:{} total:{} core:{}".format(i, c, mops_total[-1], mops_core[-1]))
        time.sleep(n_cooldown)
    plt.plot(n_cores, mops_core, "x-", label="I:{} C:{} (core)".format(i, c))
    plt.plot(n_cores, mops_total, "x", label="I:{} C:{} (total)".format(i, c))

plt.xlabel("# Cores")
plt.ylabel("MOps/s")
plt.title("Multithreading performance")
plt.legend()
plt.show()
