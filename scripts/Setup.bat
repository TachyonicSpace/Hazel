@echo off

cd ..
cd Hazel/vendor/yaml-cpp
git checkout a90b9ab06e

cd ..
cd ..
cd ..
cd scripts

python Setup.py
PAUSE