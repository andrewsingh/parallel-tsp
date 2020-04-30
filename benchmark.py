#!/usr/bin/env python

import os
import os.path
import sys
import getopt
import subprocess
import datetime



def usage(fname):
    ustring = "Usage: %s [-h] [-a ALGORITHM] [-r RUNS]" % fname
    print ustring
    print "    -h            Print this message"
    print "    -a ALGORITHM  Specify which algorithm to run as one of 'hk', 'thk', 'lkh', or 'gen'"
    print "    -i IMPLEMENT  Specify which implementation to run as one of 'seq' or 'par'"
    print "    -f INSTANCE   Specify which instance to run (name of instance file, such as 'lin318.tsp')"
    print "    -r RUNS       Set number of times each instance is run"
    print "    -c ARGS       String of additional arguments to pass to the executable"
    print "    -b BENCH      Specify a preset benchmark to run as one of 'scale', 'acc', or 'speed'"
    print "    -o OUTFILE    Create output file recording measurements"
    sys.exit(0)

# General information
algos_dict = {
    'hk': ("./code/held_karp/par_hk", "./code/held_karp/par_hk"),
    'thk': ("./code/top_hk/par_top_hk", "./code/top_hk/par_top_hk"),
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
    'lkh': ['st70.tsp', 'lin105.tsp', 'u159.tsp', 'kroA200.tsp'],
    'gen': ['st70.tsp', 'lin105.tsp', 'u159.tsp', 'kroA200.tsp']
}

# For testing the benchmarks
test_instance_dict = {
    'hk': ['br17.mat'],
    'thk': ['br17.mat'],
    'lkh': ['st70.tsp'],
    'gen': ['st70.tsp']
}

algos_list = algos_dict.keys()

out_file = None
benchmark = None
algo_args = None
impl = None
thread_list = ["2", "4", "8"]

unique_id = ""

# To store results to print at the end
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
    tstart = datetime.datetime.now()
    try:
        outmsg("Running '%s'" % cmd_line)
        sim_process = subprocess.Popen(cmd_line, shell=True)
        sim_process.wait()
        return_code = sim_process.returncode
        # Echo any results printed by simulator on stderr onto stdout
        if sim_process.stderr is not None:
            for line in sim_process.stderr:
                outmsg(line)
    except Exception as e:
        print "Execution of command '%s' failed. %s" % (cmd_line, e)
        return None
    if return_code == 0:
        delta = datetime.datetime.now() - tstart
        secs = delta.seconds + 24 * 3600 * delta.days + 1e-6 * delta.microseconds
        return secs
    else:
        print "Execution of command '%s' gave return code %d" % (cmd_line, return_code)
        return None


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
            for j in range(run_count):
                seq_cmd = " ".join(seq_pre_list + i_list)
                secs = do_run(seq_cmd)
                outmsg("Execution time: %f seconds" % (secs))
                results_seq[a][i] = ((results_seq[a]).get(i, [])) + [secs]
                outmsg("\n")
        if run_par:
            for j in range(run_count):
                par_cmd = " ".join(par_pre_list + i_list)
                secs = do_run(par_cmd)
                outmsg("Execution time: %f seconds" % (secs))
                results_par[a][i] = ((results_par[a]).get(i, [])) + [secs]
                outmsg("\n")


def run_scale():
    global algos_dict, algos_list, run_count, thread_list
    results = []
    for a in algos_list:
        outmsg("\nAlgo {}".format(a))
        (seq, par) = algos_dict[a]
        seq_pre_list = [seq]
        par_pre_list = [par]
        seq_pre_list.append("-t 1")

        for i in test_instance_dict[a]:
        #for i in instance_dict[a]:
            outmsg("\nInstance {}".format(i))
            i_list = ["-f", i]
            avgs = []
            avg = 0
            outmsg("\nSEQUENTIAL")
            for j in range(run_count):
                outmsg("Run {}".format(j + 1))
                seq_cmd = " ".join(seq_pre_list + i_list)
                secs = do_run(seq_cmd)
                outmsg("{} seconds".format(secs))
                avg += secs
                outmsg("\n")
            avg /= run_count
            avgs.append(round(avg, 4))
            outmsg("AVERAGE: {} seconds\n".format(avg))
           
            for t in thread_list:
                t_list = ["-t", t]
                avg = 0
                outmsg("\n{} THREADS".format(t))
                for j in range(run_count):
                    outmsg("Run {}".format(j + 1))
                    par_cmd = " ".join(par_pre_list + i_list + t_list)
                    secs = do_run(par_cmd)
                    outmsg("{} seconds".format(secs))
                    avg += secs
                    outmsg("\n")
                avg /= run_count
                avgs.append(round(avg, 4))
                outmsg("AVERAGE: {} seconds\n".format(avg))
            results.append([a, i] + avgs)

    outmsg("FINAL RESULTS\n{}".format(results))


def generateFileName(template):
    global unique_id
    myId = ""
    n = len(template)
    ls = []
    for i in range(n):
        c = template[i]
        if c == 'X':
            c = chr(random.randint(ord('0'), ord('9')))
        ls.append(c)
        myId += c
    if unique_id == "":
        unique_id = myId
    return "".join(ls) 

def run(name, args):
    global algos_list, run_count, instance, algo_args, impl, benchmark, out_file
    # Figure out which machine we're running on, adjust number of threads accordingly
    machine = 'x'
    try:
        host = os.environ['HOSTNAME']
    except:
        host = ''
    if host[:3] == 'ghc' or host[:4] == 'unix':
        machine = 'g'
        doCheck = True
    elif host[:8] == 'latedays' or host[:7] == 'compute':
        machine = 'l'
        doCheck = True
    else:
        outmsg("Warning: Host = '%s'. Can only get comparison results on GHC or Latedays machine" % host)
    processCounts = default_process_counts_dict[machine]

    opt_string = "ha:i:r:f:c:b:o:"
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
        elif opt == '-o':
            fname = generateFileName(val)
            try:
                out_file = open(fname, "w")
            except Exception as e:
                out_file = None
                outmsg("Couldn't open output file '%s'" % fname)
    if benchmark == "scale":
        run_scale()
    else:
        # Run the instances for each algorithm
        for a in algos_list:
            run_test(a)

    print_result()


if __name__ == "__main__":
    run(sys.argv[0], sys.argv[1:])