# chessgen

A chess move generator and validator written in modern C++

This is still an early version, so bugs might (and probably do) exist. Please fill an issue if you find any.

## Build Status
| OS     | Windows                                                                                                                                 | Linux                                                                                                         |
| ------ | --------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| Master | [![Build status](https://ci.appveyor.com/api/projects/status/fmrgv06nwvoc2rv4?svg=true)](https://ci.appveyor.com/project/MarkHC/chessgen) | [![Build Status](https://travis-ci.org/MarkHC/chessgen.svg?branch=master)](https://travis-ci.org/MarkHC/chessgen) |

## Installation

Install
```
git clone https://github.com/MarkHC/chessgen.git
cd chessgen
mkdir build && cd build
cmake .. 
make install
```

Build tests
```
git clone --recursive https://github.com/MarkHC/chessgen.git
cd chessgen
mkdir build && cd build
cmake -DCHESSGEN_TEST=ON ..
make
```
