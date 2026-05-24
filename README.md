# loglib
A simple logging library written in C

- Usage: 
```C
#include "loglib.h"

int main() 
{
    slog("Hello World!", LOG_CRITICAL, "root"); 
    // "Hello World!" - message; LOG_CRITICAL - level; "root" - logger name
    
    return 0;
}
```

## Quick start
- Clone repo:
```
git clone https://github.com/KrahmalXOfficial/loglib.git
```

## License
The repository is distributed under the GNU GPLv3 license