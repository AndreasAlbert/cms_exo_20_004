# MadAnalysis implementation for CMS-EXO-20-004

## Setup

```
# Getting the code
wget https://launchpad.net/madanalysis5/trunk/v1.8/+download/MadAnalysis5_v1.8.tgz
tar xf MadAnalysis5_v1.8.tgz
cd madanalysis5
git clone git@github.com:AndreasAlbert/cms_exo_20_004.git


# Environment
cd cms_exo_20_004/Build/
source /cvmfs/sft.cern.ch/lcg/views/LCG_97/x86_64-centos7-gcc8-opt/setup.sh
source ./setup.sh

# Building
make -j4

# Running
./MadAnalysis5job list_of_files.txt

# Output will be generated in a different directory
ls ../Output/list_of_files.txt/
```