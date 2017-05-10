#!/usr/bin/python
# -*- coding: utf-8 -*- 

# Run some ugp exectuables on some test problems and store the results.
# 
# To get the results faster, just launch this script multiple times, and it will
# take care of running each executable/test combination only once.  The tool
# will hash the parameters of the test and will cache intermediate results in a
# dedicated directory, so that tests are easy to interrupt and resume. Also the
# same test won't be performed two times.
#
# Command line parameters: one YAML file that describes the benchmark, another
# that gives the path where the test will be carried (a memory-filesystem path
# is good).
#

import os
import codecs
import sys
import time
import yaml
import subprocess
import portalocker
import re
import tempfile
import shutil

# Directory of the YAML file (where all the other stuff must be)
cwd = None

# Open the given benchmark configuration
try:
    config_filename = sys.argv[1]
    with open(config_filename, "r") as f:
        config = yaml.safe_load(f)
    cwd = os.path.abspath(os.path.dirname(config_filename))
except:
    print "Error: give the name of a valid YAML benchmark configuration file as argument."
    sys.exit(1)

# Check that the given run path exists and create a temporary directory
if len(sys.argv) < 3 or not os.path.isdir(sys.argv[2]):
    print "Error: you must give a writable directory as second argument to this script."
    sys.exit(1)
temporary_dir = tempfile.mkdtemp(prefix="ugp_bench_tmp", dir=sys.argv[2])

# All the rest of the script is relative to the YAML file
os.chdir(cwd)

# Check that a "Results" directory is available
if not os.path.isdir("Results"):
    print """
Error: no "Results" directory found. Please check that the current
directory is where you usually run your benchmarks, or create a
"Results" directory here to store the results of this benchmark.
"""
    sys.exit(1)

# Set default value for `full_ugp_output'
config['full_ugp_output'] = config.get('full_ugp_output', False)

# Check that ugp executables are runnable and try to run them
for ugp in config['ugp_executables']:
    try:
        subprocess.check_output([ugp, '--version'], cwd=cwd)
    except OSError:
        print "Error: ugp executable `%s` not found." % ugp
        sys.exit(1)
    except subprocess.CalledProcessError as error:
        print error.output
        print "Error calling `%s`: return code %d" % (error.cmd, error.returncode)
        sys.exit(1)

# Check that the directory and files of each test are present and get their contents
test_files = {}
for directory, test_conf in config['tests'].iteritems():
    files = test_conf['files']
    test_files[directory] = {}
    if not os.path.isdir(directory):
        print "Error: test directory `%s` not found." % directory
        sys.exit(1)
    for fn in files:
        try:
            with codecs.open(os.path.join(directory, fn), 'r', 'utf-8') as f:
                test_files[directory][fn] = f.read()
        except OSError:
            print "Error: file `%s' of test `%s' not found." % (fn, directory)
            sys.exit(1)

# Check that we have at least one random seed value
if not config.get('seeds', []):
    print "Error: you must give at least one random seed in the `seeds' array of the config file."
    sys.exit(1)

# Actually run the benchmark
for seed in config['seeds']:
    for directory, test_conf in config['tests'].iteritems():
        files = test_conf['files']
        precmd = test_conf.get('precmd', None)
        stripped_dir = re.sub('[^a-zA-Z0-9_]', '', directory)
        # Copy the whole test to a temporary location
        new_directory = os.path.join(temporary_dir, stripped_dir)
        os.mkdir(new_directory)
        for fn in files:
            shutil.copy(os.path.join(directory, fn), new_directory)

        for ugp in config['ugp_executables']:
            stripped_ugp = re.sub('[^a-zA-Z0-9_]', '', ugp)

            # Prepare a summary of this ugp/test combination for deduplication
            test_data = {
                'ugp_name': stripped_ugp,
                'test_files': test_files[directory],
                'test_name': stripped_dir, # just for convenience when displaying results, not significant
            }

            # Determine whether this particular combination has already been run
            comb_directory = None
            comb_dir_prefix = stripped_dir + "_" + stripped_ugp
            for potential_comb in os.listdir('Results'):
                if os.path.isdir(os.path.join('Results', potential_comb)) and comb_dir_prefix in potential_comb:
                    # Potential match, checking that test files and ugp version are the same...
                    with codecs.open(os.path.join('Results', potential_comb, 'test_data.yaml'), 'r', 'utf-8') as f:
                        run_test_data = yaml.safe_load(f)
                    if run_test_data['ugp_name'] != test_data['ugp_name']:
                        print "Not resuming from %s, ugp versions differ" % potential_comb
                        continue
                    if run_test_data['test_files'] != test_data['test_files']:
                        print "Not resuming from %s, test files differ" % potential_comb
                        continue
                    comb_directory = os.path.join('Results', potential_comb)
                    print "Resuming combination ugp/test %s..." % comb_directory
                    break

            # If the combination has never been run, start a new one
            if not comb_directory:
                comb_directory = os.path.join('Results', comb_dir_prefix + time.strftime("_%Y-%m-%d_%H:%M:%S"))
                os.mkdir(comb_directory)
                with open(os.path.join(comb_directory, 'test_data.yaml'), 'w') as f:
                    f.write(yaml.safe_dump(test_data, default_flow_style=False, allow_unicode=True))
                print "Starting new combination in %s" % comb_directory

            # Decide statistics location
            stats_filename = os.path.join(comb_directory, str(seed) + ".csv")
            log_filename = os.path.join(comb_directory, str(seed) + "_vebose.log")
            # Check whether these stats have already been produced...
            if os.path.isfile(stats_filename):
                print "%s found, skipping." % stats_filename
                continue
            # ... or are being produced right now
            lock_filename = stats_filename + ".lock"
            lock_file = open(lock_filename, "a")
            try:
                portalocker.lock(lock_file, portalocker.LOCK_EX | portalocker.LOCK_NB)
            except portalocker.LockException:
                print "%s being produced, skipping." % stats_filename
                lock_file.close()
                continue;
            if precmd:
                seeded_precmd = precmd.replace("$seed", str(seed))
                print "Running pre-command %s" % seeded_precmd
                subprocess.call(seeded_precmd, shell=True, cwd=new_directory)
            print "Running MicroGP to produce %s..." % stats_filename
            stats_filename_temp = str(seed) + time.strftime("_%Y-%m-%d,%H:%M:%S") + ".csv"
            if os.path.isfile(os.path.join(new_directory, stats_filename_temp)):
                os.remove(os.path.join(new_directory, stats_filename_temp))
            process = subprocess.Popen([
                os.path.abspath(ugp),
                '--randomSeed', str(seed),
                '--log', 'verbose.log', 'verbose', 'brief',
                '--statisticsPathName', str(stats_filename_temp)],
                stdout = subprocess.PIPE, stderr = subprocess.PIPE, cwd = new_directory)
            output = ""
            for line in iter(process.stdout.readline, ''):
                output += line
                if config['full_ugp_output'] or " * Population " in line or "WARNING" in line or "ERROR" in line:
                    print line,
            stdoutput, stderrors = process.communicate()
            if process.returncode is not 0:
                print output,
                print stdoutput
                print stderrors
                print "Error calling the executable `%s`: return code %d" % (ugp, process.returncode)
                lock_file.close()
                continue
            shutil.move(os.path.join(new_directory, 'verbose.log'), log_filename)
            shutil.move(os.path.join(new_directory, stats_filename_temp), stats_filename)
            lock_file.close()
            os.remove(lock_filename)
        shutil.rmtree(new_directory)

shutil.rmtree(temporary_dir)

