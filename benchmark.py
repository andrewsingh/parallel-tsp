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
    print "    -r RUNS       Set number of times each instance is run"

    sys.exit(0)

# General information
algosDict = {
    'hk': ("./code/held_karp/seq_hk", "./code/held_karp/par_hk"),
    'thk': ("./code/top_hk/seq_top_hk", "./code/top_hk/par_top_hk"),
    'lkh': ("./code/lin_kern/seq_lin_kern", "./code/lin_kern/par_lin_kern"),
    'gen': ("./code/genetic/seq_genetic", "./code/genetic/par_genetic")
}

defaultProcessCountsDict = { 'g': [8], 'l': [12], 'x' : [8] }

# How many times does each benchmark get run?
runCount = 1

# Test cases by algorithm
instancesDict = {
    'hk': ['br17.mat', 'gr21.mat', 'gr24.mat'],
    'thk': ['br17.mat', 'gr21.mat', 'gr24.mat'],
    'lkh': ['lin105.tsp', 'a280.tsp'],
    'gen': ['lin105.tsp', 'si175.mat', 'a280.tsp']
}

algosList = algosDict.keys()

outFile = None

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

def doRun(cmdList):
    cmdLine = " ".join(cmdList)
    tstart = datetime.datetime.now()
    try:
        outmsg("Running '%s'" % cmdLine)
        simProcess = subprocess.Popen(cmdList)
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
    global algosDict, instancesDict, resultsSeq, resultsPar
    (seq, par) = algosDict[a]
    seq_preList = [seq]
    par_preList = [par]
    for i in instancesDict[a]:
        i_list = ["-f", i]
        for j in range(runCount):
            seq_cmd = seq_preList + i_list
            secs = doRun(seq_cmd)
            resultsSeq[a][i] = ((resultsSeq[a]).get(i, [])) + [secs]
            outmsg("\n")

        for j in range(runCount):
            par_cmd = par_preList + i_list
            secs = doRun(par_cmd)
            resultsPar[a][i] = ((resultsPar[a]).get(i, [])) + [secs]
            outmsg("\n")

def run(name, args):
    global algosList
    optString = "ha:r:"
    optlist, args = getopt.getopt(args, optString)
    for (opt, val) in optlist:
        if (opt == '-h'):
            usage(name)
        elif (opt == '-a'):
            algosList = [val]
        elif (opt == '-r'):
            runCount = int(val)

    # Run the instances for each algorithm
    for a in algosList:
        runTest(a)

    printResult()


if __name__ == "__main__":
    run(sys.argv[0], sys.argv[1:])