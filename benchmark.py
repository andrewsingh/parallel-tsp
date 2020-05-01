#!/usr/bin/env python

import sys
import getopt
import subprocess
import datetime
import csv



def usage(fname):
    ustring = "Usage: %s [-h] [-a ALGORITHM] [-r RUNS]" % fname
    print(ustring)
    print("    -h            Print this message")
    print("    -a ALGORITHM  Specify which algorithm to run as one of 'hk', 'thk', 'lkh', or 'gen'")
    print("    -i IMPLEMENT  Specify which implementation to run as one of 'seq' or 'par'")
    print("    -f INSTANCE   Specify which instance to run (name of instance file, such as 'lin318.tsp')")
    print("    -r RUNS       Set number of times each instance is run")
    print("    -c ARGS       String of additional arguments to pass to the executable")
    print("    -b BENCH      Specify a preset benchmark to run as one of 'scale', 'acc', or 'eff'")
    
    sys.exit(0)

# General information
algos_dict = {
    'hk': ("./code/held_karp/seq_hk", "./code/held_karp/par_hk"),
    'thk': ("./code/top_hk/seq_top_hk", "./code/top_hk/par_top_hk"),
    'lkh': ("./code/lin_kern/lin_kern", "./code/lin_kern/lin_kern"),
    'gen': ("./code/genetic/genetic", "./code/genetic/genetic")
}

default_process_counts_dict = { 'g': [8], 'l': [12], 'x' : [8] }

# How many times does each benchmark get run?
run_count = 5

# Specific instance to be run, if None then run all instances for the algorithm in instance_dict
instance = None

# Test cases by algorithm
instance_dict = {
    'hk': ['br17.mat', 'gr21.mat', 'gr24.mat', 'fri26.mat'],
    'thk': ['br17.mat', 'gr21.mat', 'gr24.mat', 'fri26.mat'],
    'lkh': ['br17.mat', 'gr21.mat', 'gr24.mat', 'fri26.mat', 'st70.tsp', 'lin105.tsp', 'u159.tsp', 'kroA200.tsp'],
    'gen': ['br17.mat', 'gr21.mat', 'gr24.mat', 'fri26.mat', 'st70.tsp', 'lin105.tsp', 'u159.tsp', 'kroA200.tsp']
}

# For testing the benchmarks
test_instance_dict = {
    'hk': ['br17.mat', 'gr21.mat'],
    'thk': ['br17.mat', 'gr21.mat'],
    'lkh': ['br17.mat', 'st70.tsp'],
    'gen': ['br17.mat', 'st70.tsp']
}

algos_list = list(algos_dict.keys())

out_file = None
benchmark = None
algo_args = None
impl = None
thread_list = ["2", "4", "8"]
max_threads = 8

# To store results to print(at the end)
results_seq = {'hk': {}, 'thk': {}, 'lkh': {}, 'gen': {}}
results_par = {'hk': {}, 'thk': {}, 'lkh': {}, 'gen': {}}


def outmsg(s, noreturn = False):
    if len(s) > 0 and s[-1] != '\n' and not noreturn:
        s += "\n"
    sys.stdout.write(s)
    sys.stdout.flush()
    if out_file != None:
        out_file.write(s)


def do_run(cmd_line):
    delta = 0
    output = ""
    try:
        outmsg("Running '%s'" % cmd_line)
        tstart = datetime.datetime.now()
        # Raises an error if return code is non-zero
        output = subprocess.run(cmd_line, stdout=subprocess.PIPE, shell=True).stdout.decode('utf-8')
        #output = subprocess.check_output(cmd_line, shell=True)
        delta = datetime.datetime.now() - tstart
        outmsg(output)
    except Exception as e:
        print("Execution of command '%s' failed. %s" % (cmd_line, e))
        return None
    
    tour_cost = int(output.split("\n")[-2].split(" = ")[1])
    secs = delta.seconds + 24 * 3600 * delta.days + 1e-6 * delta.microseconds
    outmsg("{} seconds".format(round(secs, 4)))
    return (secs, tour_cost)



def print_result():
    global results_seq, results_par
    outmsg("============ FINAL RESULTS ============")
    if impl == "seq":
        for a in results_seq.keys():
            if results_seq[a] != {}:
                outmsg("Algorithm %s\n" % a)
                for i in results_seq[a].keys():
                    seq_avg = (sum(results_seq[a][i]) / len(results_seq[a][i]))
                    outmsg("Instance %s\n" % i)
                    outmsg("Sequential average: %0.3f seconds\n" % seq_avg)
                    outmsg("\n")
                outmsg("\n")
    elif impl == "par":
        for a in results_par.keys():
            if results_par[a] != {}:
                outmsg("Algorithm %s\n" % a)
                for i in results_par[a].keys():
                    par_avg = (sum(results_par[a][i]) / len(results_par[a][i]))
                    outmsg("Instance %s\n" % i)
                    outmsg("Parallel average: %0.3f seconds\n" % par_avg)
                    outmsg("\n")
                outmsg("\n")
    else:
        for a in results_seq.keys():
            if results_seq[a] != {}:
                outmsg("Algorithm %s\n" % a)
                for i in results_seq[a].keys():
                    seq_avg = (sum(results_seq[a][i]) / len(results_seq[a][i]))
                    par_avg = (sum(results_par[a][i]) / len(results_par[a][i]))
                    outmsg("Instance %s\n" % i)
                    outmsg("Sequential average: %0.3f seconds\n" % seq_avg)
                    outmsg("Parallel average: %0.3f seconds\n" % par_avg)
                    outmsg("Speedup: %0.3f\n" % (seq_avg / par_avg))
                    outmsg("\n")
                outmsg("\n")


def run_test(a):
    global algos_dict, instance_dict, results_seq, results_par, run_count, instance, algo_args, impl
    (seq, par) = algos_dict[a]
    seq_pre_list = [seq]
    par_pre_list = [par]
    if a in ["lkh", "gen"]:
        seq_pre_list.append("-t 1")
    if algo_args:
        # Append additional arguments for algorithm
        seq_pre_list.append(algo_args)
        par_pre_list.append(algo_args)
    if instance:
        # Run the specified instance
        instances = [instance]
    else:
        # Run all instances in the dict for the specified algorithm
        instances = instance_dict[a]

    run_seq = True
    run_par = True
    if impl == "seq":
        run_par = False
    elif impl == "par":
        run_seq = False

    for i in instances:
        i_list = ["-f", i]
        if run_seq:
            seq_cmd = " ".join(seq_pre_list + i_list)
            for j in range(run_count):
                (secs, _) = do_run(seq_cmd)
                results_seq[a][i] = ((results_seq[a]).get(i, [])) + [secs]
                outmsg("\n")
        if run_par:
            par_cmd = " ".join(par_pre_list + i_list)
            for j in range(run_count):
                (secs, _) = do_run(par_cmd)
                results_par[a][i] = ((results_par[a]).get(i, [])) + [secs]
                outmsg("\n")



def run_scale(a):
    global algos_dict, algos_list, run_count, thread_list, instance_dict
    #instance_dict = test_instance_dict

    outmsg("\nAlgorithm {}".format(a))
    results = []
    (seq, par) = algos_dict[a]
    seq_pre_list = [seq]
    par_pre_list = [par]
    if a in ["lkh", "gen"]:
        seq_pre_list.append("-t 1")

    for i in instance_dict[a]:
        outmsg("\nInstance {}".format(i))
        i_list = ["-f", i]
        seq_cmd = " ".join(seq_pre_list + i_list)
        avgs = []
        avg = 0
        for j in range(run_count):
            outmsg("Run {}".format(j + 1))
            (secs, _) = do_run(seq_cmd)
            avg += secs
        avg = round(avg / run_count, 4)
        avgs.append(avg)
        outmsg("AVERAGE: {} seconds\n".format(avg))
       
        for t in thread_list:
            t_list = ["-t", t]
            par_cmd = " ".join(par_pre_list + i_list + t_list)
            avg = 0
            for j in range(run_count):
                outmsg("Run {}".format(j + 1))
                (secs, _) = do_run(par_cmd)
                avg += secs
            avg = round(avg / run_count, 4)
            avgs.append(avg)
            outmsg("AVERAGE: {} seconds\n".format(avg))
        results.append([a, i.split(".")[0]] + avgs)

    outmsg("{} final results\n{}".format(a, results))
    with open("results/scale.csv", "a+", newline="") as f:
        writer = csv.writer(f)
        writer.writerows(results)



def run_eff():
    global algos_dict, algos_list, run_count, max_threads, instance_dict
    #instance_dict = test_instance_dict

    results = [[""] + algos_list]
    
    for i in instance_dict["lkh"]:
        outmsg("\nInstance {}".format(i))
        i_list = ["-f", i]
        results_row = [i.split(".")[0]]
        for a in algos_list:
            if i not in instance_dict[a]:
                results_row.append("")
            else:
                outmsg("\nAlgorithm {}".format(a))
                (_, par) = algos_dict[a]
                par_cmd = " ".join([par, "-t", str(max_threads)] + i_list)
                avg = 0
                for j in range(run_count):
                    outmsg("Run {}".format(j + 1))
                    (secs, _) = do_run(par_cmd)
                    avg += secs
                    outmsg("\n")
                avg = round(avg / run_count, 4)
                outmsg("AVERAGE: {} seconds\n".format(avg))
                results_row.append(avg)
        results.append(results_row)

    outmsg("Final results\n{}".format(results))
    with open("results/eff.csv", "a+", newline="") as f:
        writer = csv.writer(f)
        writer.writerows(results)



def run_acc():
    global algos_dict, algos_list, run_count, max_threads, instance_dict
    #instance_dict = test_instance_dict

    results = [[""] + algos_list]
    
    for i in instance_dict["lkh"]:
        outmsg("\nInstance {}".format(i))
        i_list = ["-f", i]
        results_row = [i.split(".")[0]]
        for a in algos_list:
            if i not in instance_dict[a]:
                results_row.append("")
            else:
                outmsg("\nAlgorithm {}".format(a))
                (_, par) = algos_dict[a]
                par_cmd = " ".join([par, "-t", str(max_threads)] + i_list)
                (secs, tour_cost) = do_run(par_cmd)
                outmsg("\n")
                results_row.append(tour_cost)
        results.append(results_row)

    outmsg("Final results\n{}".format(results))
    with open("results/acc.csv", "a+", newline="") as f:
        writer = csv.writer(f)
        writer.writerows(results)



def run(name, args):
    global algos_list, run_count, instance, algo_args, impl, benchmark
    opt_string = "ha:i:r:f:c:b:"
    optlist, args = getopt.getopt(args, opt_string)
    for (opt, val) in optlist:
        if (opt == '-h'):
            usage(name)
        elif (opt == '-a'):
            algos_list = [val]
        elif (opt == '-i'):
            impl = val
        elif (opt == '-r'):
            run_count = int(val)
        elif (opt == '-f'):
            instance = val
        elif (opt == '-c'):
            algo_args = val
        elif (opt == '-b'):
            benchmark = val

    if benchmark == "scale":
        for a in algos_list:
            run_scale(a)
    elif benchmark == "eff":
        run_eff()
    elif benchmark == "acc":
        run_acc()
    else:
        # Run the instances for each algorithm
        for a in algos_list:
            run_test(a)
        print_result()


if __name__ == "__main__":
    run(sys.argv[0], sys.argv[1:])