#WorldWiideWeb Wii Threading Sample

In complex programs, there are often several threads of execution. For example,
in a web browser, it is not a good idea to have user input waiting on the page
load, because then the user will get annoyed and leave. However, libogc uses
its own threading subsystem, LWP, which is a little different from most. Thus,
I decided to write this sample program, mostly as an excercise for myself to
get familiar with LWP, and also, to help others who want to use threads.

Usually, people are referred to complete programs when they ask about complex
features like threads or sockets, and it can be hard to dig through all the
code when you just want to see how threads work. Thus, I hope this can be useful
for people who want to make more complex programs.

### Where can I learn more?

Probably the best way to learn threads is to use pthreads. There is a good
tutorial hosted by LLNL [here](https://computing.llnl.gov/tutorials/pthreads/).
You will need a POSIX compatible system, although there is a Windows pthreads
library floating around somewhere.
