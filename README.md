# Yglob – a glob expression library for C++

Yglob is a multi-platform C++ library for searching for files and directories with *glob* expressions. Glob expressions are paths that include wildcard characters such as `?` and `*`, for instance will `images/a?/*.jpg`  match all files with a `jpg` extension in sub-directories of `images` whose names consists of two characters, with the first being an `a`.

Yglob requires C++20. Internally it uses UTF-8 and uses 

## Example

The following program prints the paths of all cpp files in every subdirectory of the current directory.

```c++
#include <iostream>
#include <Yglob/PathIterator.hpp>

int main()
{
    for (auto& path : Yglob::PathIterator("**/*.cpp"))
        std::cout << path << "\n";
    return 0;
}
```

If run from the Yglob directory itself, the output will be similar to:

```
"./tests/YglobTest/test_PathIterator.cpp"
"./tests/YglobTest/test_GlobPattern.cpp"
... more lines ...
"./src/Yglob/PathPartIterator.cpp"
"./src/Yglob/MatchGlobPattern.cpp"
"./extras/listfiles/ListFiles.cpp"
```

## Supported glob syntax

| Token | Description |
|:--|:--|
| ? | Matches any single character, apart from the directory separator |
| * | Matches any number of characters, including none, apart from the directory separator |
| [chars] | Matches any single character in the set of characters. Ranges can be specified with a dash, e.g. `0-9` and `a-b`. |
| [\^chars] | Matches any single character not in the set of characters. |
| {pattern,pattern,...} | Matches any of the sub-patterns. For example will `*.{jpg,jpeg,png}` match any of the listed file name extensions. |

The glob expressions are by default case-insensitive, so `*.txt` will also match files with a `.TXT` extension. Whether or not plain directory names and file names are compared case-sensitively depends on the operating system. This behaviour is controlled by flags that can be passed to the `PathIterator` constructor.