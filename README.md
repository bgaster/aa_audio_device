# Audio Anywhere C Audio Device API

It is hard to find a more robust and well developed Audio framework that [JUCE](https://juce.com/) 
and if your interest is in writing audio applications with C++, then go no further. 
However, if, for whatever reason, you want to write in a language other than C++, then JUCE's
C++ nature can have its drawbacks.

This module provides a tiny C wrapper around JUCE's audio device manager, which proivdes a solid foundation
for building audio libraries with other programming languages, such as C or Rust.

What it does not do is provide a wrapper around JUCE's many other excellent components, such as it UI framework, and
so on. This might be useful to some, but the goal is is to provide a very minimal wrapper around JUCE's excellent
audio framework, thus avoiding having to use alternative APIs, such as PortAudio. 

# Requirements 

- JUCE 6.x
- XCode,  for Mac OS
- Visual Studio 2019, for Windows
- CMake 3.12 or greater

# Building

Create a JUCE instance for your platform, e.g. XCode or Visual Studio, with the project [AAStandaloneJUCE.jucer], 
which is under the directory ```external/AAStandaloneJUCE```. The CMake setup requires that the project is exported, including modules 
under ```external/AAStandaloneJUCE```. Once completed it should look like:

```
external
   AAStandaloneJUCE
      Builds
      JuceLibraryCode
```

Note, make sure that the JUCE modules appear under ```external/AAStandaloneJUCE/modules```.

Now to build with cmake:

```
mkdir build
cd build
cmake ..
make
```

You can run one of the examples, such as sine wave generator, with:

```
./aaSine
```

# Library

To use include the header ```aa.h``, which is requires only ```aa_types.h``, and these two can be copied from
the ```include``` directory.

The library itself is built as a static library, for Mac OS:

```
libaa.a
```

and Windows:

```
libaa.lib
```

These will be built in the ```build``` directory.
