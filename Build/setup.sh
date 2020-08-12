#!/bin/bash

# Defining colours for shell
GREEN="\\033[1;32m"
RED="\\033[1;31m"
PINK="\\033[1;35m"
BLUE="\\033[1;34m"
YELLOW="\\033[1;33m"
CYAN="\\033[1;36m"
NORMAL="\\033[0;39m"

# Configuring MA5 environment variable
export MA5_BASE=$(readlink -e ../../)
# Configuring PATH environment variable
if [ $PATH ]; then
export PATH=$MA5_BASE/tools/SampleAnalyzer/ExternalSymLink/Bin:/cvmfs/sft.cern.ch/lcg/releases/ROOT/v6.20.02-10e75/x86_64-centos7-gcc8-opt/bin:/cvmfs/sft.cern.ch/lcg/releases/fastjet/3.3.2-c962b/x86_64-centos7-gcc8-opt/bin:$PATH
else
export PATH=$MA5_BASE/tools/SampleAnalyzer/ExternalSymLink/Bin:/cvmfs/sft.cern.ch/lcg/releases/ROOT/v6.20.02-10e75/x86_64-centos7-gcc8-opt/bin:/cvmfs/sft.cern.ch/lcg/releases/fastjet/3.3.2-c962b/x86_64-centos7-gcc8-opt/bin
fi

# Configuring LD_LIBRARY_PATH environment variable
if [ $LD_LIBRARY_PATH ]; then
export LD_LIBRARY_PATH=$MA5_BASE/tools/SampleAnalyzer/Lib:$MA5_BASE/tools/SampleAnalyzer/ExternalSymLink/Lib:/cvmfs/sft.cern.ch/lcg/releases/ROOT/v6.20.02-10e75/x86_64-centos7-gcc8-opt/lib:/cvmfs/sft.cern.ch/lcg/releases/fastjet/3.3.2-c962b/x86_64-centos7-gcc8-opt/lib:/cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-centos7/bin/../lib/gcc/x86_64-pc-linux-gnu/8.2.0:/cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-centos7/bin/../lib/gcc:/cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-centos7/bin/../lib/gcc/x86_64-pc-linux-gnu/8.2.0/../../../../lib64:/lib/../lib64:/usr/lib/../lib64:/cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-centos7/bin/../lib/gcc/x86_64-pc-linux-gnu/8.2.0/../../..:$LD_LIBRARY_PATH
else
export LD_LIBRARY_PATH=$MA5_BASE/tools/SampleAnalyzer/Lib:$MA5_BASE/tools/SampleAnalyzer/ExternalSymLink/Lib:/cvmfs/sft.cern.ch/lcg/releases/ROOT/v6.20.02-10e75/x86_64-centos7-gcc8-opt/lib:/cvmfs/sft.cern.ch/lcg/releases/fastjet/3.3.2-c962b/x86_64-centos7-gcc8-opt/lib:/cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-centos7/bin/../lib/gcc/x86_64-pc-linux-gnu/8.2.0:/cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-centos7/bin/../lib/gcc:/cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-centos7/bin/../lib/gcc/x86_64-pc-linux-gnu/8.2.0/../../../../lib64:/lib/../lib64:/usr/lib/../lib64:/cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-centos7/bin/../lib/gcc/x86_64-pc-linux-gnu/8.2.0/../../..
fi

# Checking that all environment variables are defined
if [[ $MA5_BASE && $PATH && $LD_LIBRARY_PATH ]]; then
echo -e $YELLOW"--------------------------------------------------------"
echo -e "    Your environment is properly configured for MA5     "
echo -e "--------------------------------------------------------"$NORMAL
fi
