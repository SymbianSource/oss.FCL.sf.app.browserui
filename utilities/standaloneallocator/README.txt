------------------------------
This is the custom memory allocator for use with QtWebkit based killer apps for Symbian/S60. 
------------------------------

Major features: 
- faster than Symbian RHeap. O(1) operations vs. O(n) 
- more space efficient for smaller allocations (i.e lower system RAM usage)
- switches at run-time between 1 of 3 allocation algorithms based on requested alloc size

* How Symbian OS process startup works: 
(from http://developer.symbian.org/wiki/index.php/Symbian_OS_Internals/3._Threads,_Processes_and_Libraries ) 
 * Process entry point (__E32Startup) -> RunThread() -> UserHeap::SetupThreadHeap(). 
 * SetupThreadHeap() is where we initialize our custom (heap) allocator.
 * If this is first thread in the process, constructors for static data are called, followed by E32Main(). 
 * If NOT first thread in process, the thread's entry point is called straight away. 
 * NOTE: UserHeap::SetupThreadHeap() is called BEFORE static data is initialized
 
 
Debugging support:

* To use standard Symbian/S60 memory analysis tools, comment out the call to UserHeap::SetupThreadHeap() and recompile the library. 
  Then relink your application (.exe) with the updated library. This will make RHeap your default allocator once again. 

(from Shane Kearns) 
* With a 9.4 (5.0) or later RND environment, you can use BTRACE by defining the required BTRACE macros (in the source, commented out).
  This emits the same BTRACE entries as the default allocator, so you can do memory leak analysis using existing tools based on BTRACE.
  For example, the promising but cancelled SAW. (which is still available inside nokia). 
* Memory analysis tools based on dumping the heap to a file and doing post-mortem analysis won't work unless specifically designed for this allocator.




