# OS Programming Project
A project about programming in Linux OS using C and bash shell code.
  
The project has 3 parts:
- Shell scripting in bash shell
- Function integral calculation using Linux processes
- Processes time scheduler for Linux systems
  
***NOTE:*** The code of all parts can be executed only in Linux systems.
___
### Shell Scripting
The **logparsher.sh** file contains bash code for the management of **access.log** file, which is a Linux system file. Each of its records may have one of the following formats:
- 127.0.0.1 - userid - [10/Feb/2022:13:55:36 +0300] "GET /server-status HTTP/1.1" 200 2326
- 127.0.0.1 - - [29/Mar/2022:17:02:15 +0300] "POST /phpmyadmin/index.php?route=/navigation&ajax_request=1 HTTP/1.1" 200 3941 "-" "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:98.0) Gecko/20100101 Firefox/98.0"
- ::1 - - [19/Apr/2022:10:22:23 +0300] "GET /phpmyadmin/themes/pmahomme/img/b_bookmark.png HTTP/1.1" 200 637 

Depending on the arguments of **logparsher.sh** the following functions are performed:
- **./logparser.sh &lt;filename&gt;:** Displays on the screen all the contents of the file given as argument line by line.
- **./logparser.sh &lt;filename&gt; --usrid:** Displays(counts) how many times each user, including the empty user, is repeated in the contents of the file. The users appear alphabetically sorted.
- **./logparser.sh &lt;filename&gt; --usrid &lt;user_id&gt;:** Displays the records of the file in which the user given as argument is appeared.
- **./logparser.sh &lt;filename&gt; -method &lt;method_name&gt;:** Displays the records of the file given as an argument that correspond to the method given as an argument to -method. The method argument can be GET or POST.
- **./logparser.sh &lt;filename&gt; --servprot &lt;Network_Protocol&gt;:** Displays the records of the file given as an argument that correspond to the protocol given as an argument to --servprot. The protocol argument can be IPv4 or IPv6.
- **./logparser.sh &lt;filename&gt; --browsers:** Displays Mozilla, Chrome, Safari and Edge browsers and after each browser the number of times they were used.
- **./logparser.sh &lt;filename&gt; --datum &lt;Date&gt;:** Displays all records of the file given as an argument that were made in the month that correspond to the month given as an argument to --datum regardless of the year. The date argument can be on of the alphanumeric values Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov and Dec.
___
### Function Integral Calculation Using Linux Processes
The file **initial.c** contains code to calculate the integral of the function:
*f(x) = ln(x)*sqrt(x)*
  
In the **processes.c** file there is an implementation of calculating the same integral by using linux processes with message exchange.
___
### Processes Time Scheduler
In this part of the project the goal is to implement a scheduling environment in a Linux operating system. Specifically, a scheduler is implemented which takes as input the applications that are to be executed, reads a file with their names, enters them into an appropriate data structure(list) and then routes them by applying one of the following policies:
- **FCFS(First Come First Served)**
- **SJF(Shortest Job First)**
- **RR(Round Robin)**
- **PRIO(Priority Scheduling)**
  
There are two folders:
- work: This folder contains the **work.c** file in which some calculations are performed in order to create an executable with a delay. Also in the folder there is a makefile with which seven executables of the work.c with different delays are created.
- scheduler: It contains the **scheduler.c** file which implements the time scheduler for the processes. It also contains the files homogeneous.txt, mixed.txt and reverse.txt which deffines the paths of the executables of the work.c that are to be routed by scheduler.

**Execution Instructions:**  
After scheduler.c is compiled, it can be executed as follows:  
***./scheduler &lt;policy&gt; [&lt;quantum&gt;] &lt;input_filename&gt;***  
  
where:  
- scheduler: The executable of scheduler.c.
- policy: The routing policy with which the processes will run. Possible values ​​are FCFS, SJF, RR and PRIO.
- quantum: The routing quantum in msec. Only required when a routing policy is set to RR or PRIO.
- input_filename: The name of the file that contains the workload to run through scheduler. Each line of the file contains the name of a process's executable and a number
which will state the required execution time or his priority.
