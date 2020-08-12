# MadAnalysis implementation for CMS-EXO-20-004

## Setup

```bash
# Getting the code
wget https://launchpad.net/madanalysis5/trunk/v1.8/+download/MadAnalysis5_v1.8.tgz
tar xf MadAnalysis5_v1.8.tgz
cd madanalysis5

# Build MadAnalysis
source /cvmfs/sft.cern.ch/lcg/views/LCG_97/x86_64-centos7-gcc8-opt/setup.sh

echo "exit" > tmp.txt
./bin/ma5 -b -f -s tmp.txt
source ./tools/SampleAnalyzer/setup.sh


# Get this implementation
git clone git@github.com:AndreasAlbert/cms_exo_20_004.git
cd cms_exo_20_004/Build/

# Building
make -j4

# Running
./MadAnalysis5job list_of_files.txt

# Output will be generated in a different directory
ls ../Output/list_of_files.txt/
```


If you have already set everything up, you simply need to re-source the environment:

```bash
source /cvmfs/sft.cern.ch/lcg/views/LCG_97/x86_64-centos7-gcc8-opt/setup.sh
source madanalysis5/tools/SampleAnalyzer/setup.sh
```