# knightlog

Helpers for outputting information for MacroQuest

## Adding to your project

Select a directory and add it:

```cmd
cd Your/Plugin/Directory
git submodule add -b main https://github.com/Knightly1/knightlog.git contrib/knightlog
```

Include in your plugin cpp:
```cpp
#include <mq/Plugin.h>
#include "contrib/knightlog/knightlog.h"
```

Call one of the methods:

```cpp
knightlog::Info('This is a test');
```