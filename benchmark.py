#!/usr/bin/env python

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
    
    sys.exit(0)

# General information
algosDict = {
    'hk': ("./code/held_karp/seq_hk", "./code/held_karp/par_hk"),
    'thk': ("./code/top_hk/seq_top_hk", "./code/top_hk/par_top_hk"),
    'lkh': ("./code/lin_kern/lin_kern", "./code/lin_kern/lin_kern"),
    'gen': ("./code/genetic/seq_genetic", "./code/genetic/par_genetic")
}

defaultProcessCountsDict = { 'g': [8], 'l': [12], 'x' : [8] }

# How many times does each benchmark get run?
runCount = 1

# Specific instance to be run, if None then run all instances for the algorithm in instanceDict
instance = None

# Test cases by algorithm
instanceDict = {
    'hk': ['br17.mat', 'gr21.mat', 'gr24.mat'],
    'thk': ['br17.mat', 'gr21.mat', 'gr24.mat'],
    'lkh': ['lin105.tsp', 'a280.tsp', 'lin318.tsp'],
    'gen': ['lin105.tsp', 'si175.mat', 'a280.tsp']
}

algosList = algosDict.keys()

outFile = None
benchmark = None
algo_args = None
impl = None

# To store results to print at the end
resultsSeq = {'hk': {}, 'thk': {}, 'lkh': {}, 'gen': {}}
resultsPar = {'hk': {}, 'thk': {}, 'lkh': {}, 'gen': {}}


def outmsg(s, noreturn = False):
    if len(s) > 0 and s[-1] != '\n' and not noreturn:
        s += "\n"
    sys.stdout.write(s)
    sys.stdout.flush()
    if outFile != None:
        outFile.write(s)


def doRun(cmdLine):
    tstart = datetime.datetime.now()
    try:
        outmsg("Running '%s'" % cmdLine)
        simProcess = subprocess.Popen(cmdLine, shell=True)
        simProcess.wait()
        returnCode = simProcess.returncode
        # Echo any results printed by simulator on stderr onto stdout
        if simProcess.stderr is not None:
            for line in simProcess.stderr:
                outmsg(line)
    except Exception as e:
        print "Execution of command '%s' failed. %s" % (cmdLine, e)
        return None
    if returnCode == 0:
        delta = datetime.datetime.now() - tstart
        secs = delta.seconds + 24 * 3600 * delta.days + 1e-6 * delta.microseconds
        return secs
    else:
        print "Execution of command '%s' gave return code %d" % (cmdLine, returnCode)
        return None


def printResult():
    global resultsSeq, resultsPar
    if impl == "seq":
        for a in resultsSeq.keys():
            if resultsSeq[a] != {}:
                outmsg("Algorithm %s\n" % a)
                for i in resultsSeq[a].keys():
                    seqAvg = (sum(resultsSeq[a][i]) / len(resultsSeq[a][i]))
                    outmsg("Instance %s\n" % i)
                    outmsg("Sequential average: %0.3f seconds\n" % seqAvg)
                    outmsg("\n")
                outmsg("\n")
    elif impl == "par":
        for a in resultsPar.keys():
            if resultsPar[a] != {}:
                outmsg("Algorithm %s\n" % a)
                for i in resultsPar[a].keys():
                    parAvg = (sum(resultsPar[a][i]) / len(resultsPar[a][i]))
                    outmsg("Instance %s\n" % i)
                    outmsg("Parallel average: %0.3f seconds\n" % parAvg)
                    outmsg("\n")
                outmsg("\n")
    else:
        for a in resultsSeq.keys():
            if resultsSeq[a] != {}:
                outmsg("Algorithm %s\n" % a)
                for i in resultsSeq[a].keys():
                    seqAvg = (sum(resultsSeq[a][i]) / len(resultsSeq[a][i]))
                    parAvg = (sum(resultsPar[a][i]) / len(resultsPar[a][i]))
                    outmsg("Instance %s\n" % i)
                    outmsg("Sequential average: %0.3f seconds\n" % seqAvg)
                    outmsg("Parallel average: %0.3f seconds\n" % parAvg)
                    outmsg("Speedup: %0.3f\n" % (seqAvg / parAvg))
                    outmsg("\n")
                outmsg("\n")


def runTest(a):
    global algosDict, instanceDict, resultsSeq, resultsPar, runCount, instance, algo_args, impl
    (seq, par) = algosDict[a]
    seq_preList = [seq]
    par_preList = [par]
    if a == "lkh":
        seq_preList.append("-t 1")
    if algo_args:
        # Append additional arguments for algorithm
        seq_preList.append(algo_args)
        par_preList.append(algo_args)
    if instance:
        # Run the specified instance
        instances = [instance]
    else:
        # Run all instances in the dict for the specified algorithm
        instances = instanceDict[a]

    run_seq = True
    run_par = True
    if impl == "seq":
        run_par = False
    elif impl == "par":
        run_seq = False

    for i in instances:
        i_list = ["-f", i]
        if run_seq:
            for j in range(runCount):
                seq_cmd = " ".join(seq_preList + i_list)
                secs = doRun(seq_cmd)
                resultsSeq[a][i] = ((resultsSeq[a]).get(i, [])) + [secs]
                outmsg("\n")
        if run_par:
            for j in range(runCount):
                par_cmd = " ".join(par_preList + i_list)
                secs = doRun(par_cmd)
                resultsPar[a][i] = ((resultsPar[a]).get(i, [])) + [secs]
                outmsg("\n")


def run(name, args):
    global algosList, runCount, instance, algo_args, impl
    optString = "ha:i:r:f:c:b:"
    optlist, args = getopt.getopt(args, optString)
    for (opt, val) in optlist:
        if (opt == '-h'):
            usage(name)
        elif (opt == '-a'):
            algosList = [val]
        elif (opt == '-i'):
            impl = val
        elif (opt == '-r'):
            runCount = int(val)
        elif (opt == '-f'):
            instance = val
        elif (opt == '-c'):
            algo_args = val
        elif (opt == '-b'):
            benchmark = val
    # Run the instances for each algorithm
    for a in algosList:
        runTest(a)

    printResult()


if __name__ == "__main__":
    run(sys.argv[0], sys.argv[1:])