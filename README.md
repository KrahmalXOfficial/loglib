# <center> loglib </center>
A simple logging library written in C

- Usage: 
```C
#include "loglib.h"
#include <stdio.h>

int main() 
{
    set_log_out(stdout);
    set_format("[%YYYY-%M-%d %h:%m:%s,%ms.3] [%lvl.1] [%name] %msg");
    
    flog(CRITICAL, "root", "Hello, World!"); 
    // "Hello, World!" - message; CRITICAL - level; "root" - logger name
    
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