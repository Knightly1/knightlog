# knightlog

Helpers for writing chat output in MacroQuest using spdlog.

## Quick(ish) Start

Select a directory and add it:

```cmd
cd Your/Plugin/Directory
git submodule add -b main https://github.com/Knightly1/knightlog.git contrib/knightlog
```

Include in your plugin cpp:
```cpp
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SPDLOG_EOL ""
#include <mq/Plugin.h>
#include "contrib/knightlog/knightlog.h"
```

Add to your globals:

```cpp
KnightLog* knightlog = nullptr;
```

Add to InitializePlugin()
```cpp
PLUGIN_API void InitializePlugin()
{
// ...snip...
    knightlog = new KnightLog();
```

Add to your ShutdownPlugin()
```cpp
PLUGIN_API void ShutdownPlugin()
{
// ...snip...
    delete knightlog;
```

Output something to chat:
```cpp
SPDLOG_INFO("Hello world.");
```

## Functions

KnightLog is based around spdlog, so the available macros for output are:
```cpp
SPDLOG_TRACE("For trace messages.");
SPDLOG_DEBUG("For debug messages.");
SPDLOG_INFO("For informational messages.");
SPDLOG_WARN("For warning messages.");
SPDLOG_ERROR("For error messages.");
SPDLOG_CRITICAL("For critical messages (also help messages).");
```

Keep in mind that the default logger only shows messages at the "info" level and above.  You can change this at runtime using the function
`SetLogLevel()` -- for example, to turn on all messages at "trace" and above:

```cpp
knightlog->SetLogLevel("trace");
```

KnightLog doesn't allow you to turn the logging off, so the least information that you can see is by setting the logging level to critical which
will only show critical messages.  For this reason, it's recommended that commands or outputs (like help) that MUST always provide feedback be output
with SPDLOG_CRITICAL.  Otherwise, when your logging level changes you will lose that output.

You can also get the current log level:
```cpp
std::string currentLogLevel = knightlog->GetLogLevel();
```

And you can get the color codes for each of the colors by passing in their enum:
```cpp
std::string colorCodeForWarn = knighttlog->GetColorByLevel(spdlog::level::warn);
```

The other functions (Setting Patterns and colors) are described below.

## Changing log settings

KnightLog allows you to change the log format and the colors of each log level as well as set the log level of your program (for what shows).

### Log Format

The default log format is `"%^[%T] %L [%n] :: %v%$"` the deciphering of which can be found here: https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#pattern-flags

There is a deviation from the docs in that `%^` and `%$` in the docs are said to only be usable once.  These are overridden here so you can
use them as many times as you'd like.  `%^` is just the color code for the logging level that you are at, while `%$` is equivalent to `\ax`.

There are also two additional options:
```
%j - The source file of the currently running macro (if any)
%k - The line of the currently running macro (if any)
%q - Equivalent to "(%j :: Line %k)" (if a macro is running)
```

KnightLog's constructor takes an argument of the logging format you'd prefer, but the pattern can also be changed at any time at runtime.

For example, if you wanted to match MQ2Nav's log settings you would call:
```cpp
PLUGIN_API void InitializePlugin()
{
// ...snip...
    knightlog = new KnightLog("%^[%n]%$ %v");
```

If you later wanted to change the pattern (perhaps for a specific event) you would call SetPattern.  For example, you can hardcode colors as well if you want all of
your messages to look the same, using the MQ color codes.  To do that at a point in time:

```cpp
knightlog->SetPattern("\ay[%n]\ax %v");
```

### Setting Colors

Colors for each of the levels can be set at runtime by passing in either a map or updating a single color.  If you are updating more than one color, the most efficient
way is to pass in a map.  Either way, it is done by passing in the enum for the level you want to set and the color you want to use.  Abbreviations can be used for colors
(for example `r` for red) or the full hex code can be used in the form of `#xxxxxx`.  For example, to set all the colors:

```cpp
PLUGIN_API void InitializePlugin()
{
// ...snip...
    knightlog = new KnightLog();
    knightlog->SetColorByLevel(
        {
            {spdlog::level::trace,    "#FF00FF"},
            {spdlog::level::debug,    "#FF8C00"},
            {spdlog::level::info,     "#FFFFFF"},
            {spdlog::level::warn,     "#FFD700"},
            {spdlog::level::err,      "#F22613"},
            {spdlog::level::critical, "#F22613"}
        }
    );
```

Or to just set debug and critical:
```cpp
PLUGIN_API void InitializePlugin()
{
// ...snip...
    knightlog = new KnightLog();
    knightlog->SetColorByLevel(
        {
            {spdlog::level::debug,    "#FF8C00"},
            {spdlog::level::critical, "#F22613"}
        }
    );
```

Or to just set info to white:
```cpp
PLUGIN_API void InitializePlugin()
{
// ...snip...
    knightlog = new KnightLog();
    knightlog->SetColorByLevel(spdlog::level::info, "w");
```
