# LearningModernCpp

Learning and testing the cpp skills

## Issue Record

When I build header only files, use clang++ or g++ successfully build the a.exe, but cannot run a.exe successfully.
But when I change the .h file to .cpp, clang++ make a.exe run success, but g++ failed to build a.exe, I don't know what's the reason.

Another issue is Clang support only C++17 standard, when I use std::osyncstream, std::hardware_destructive_interference_size or std::jthread,
it builded failed, what fuck! It's version is 18.1.6, I test the same codes at godbolt, it's no problem, Holy Shit!
When I use GCC, it build success, but cannot output at Git Bash, it can only output at powershell!!!
