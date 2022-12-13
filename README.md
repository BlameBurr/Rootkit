# Goal
The goal of this is to write a proof of concept rootkit that can be loaded into different linux kernels and functions as intended.
The rootkit is aimed primarily at TryHackMe's KoTH gamemode and is designed to prevent writing to the king file used to claim King of The Hill and gain points. Standard preventions work to a degree but are easily reversed whereas a rootkit functions at kernel level to prevent writing and therefore is a lot harder to circumvent if at all possible.

# Problems found along the way
* Absolute paths were a breeze and could be handled as is, on the other hand there was relative paths that needed handling. This was much harder and the code will need cleaning up and refactoring as it's a drop in function an apache2 module; however, with a bit of work that was fixed.
* Then there was symlinks, soft symlinks were a nightmare to resolve as you had find the file it was actually linked to and compare its path; however using the kern_path and d_path (if I remember correctly) this was resolved. Hard links took at lot more work but ultimately actually had an easier resolution of just comparing inodes.

# TODO
* ~~Hide Rootkit~~ (Could improve by reducing the traces left)
* ~~Process Privilege Escalation~~ (Could improve by adding a thing to set specific processes to root rather than current)
* ~~Immutable King File~~
* Reduce system processing overhead on hooked functions where possible (Not sure if current overhead causes instability on live systems)
* Hide Processes
* Create & Hide Bind Shell & Hide Listening Port
* Create Backdoor User
* Client & Magic file, word or packet System


# Usage
To make this project you just type `make` in console and let it do the work for you, you'll need `gcc` and `make`
Once completed you'll have a file named `rootkit.ko` which you need to load in using `sudo insmod rootkit.ko`
To unload it you just run `sudo rmmod rootkit`

Now using `sudo lsmod` you can see that rootkit is loaded in, if you want to hide this you can run `kill 1 -64` (which runs kill with the signal 64). To unhide it you just run the same again. You can also give your current process root permissions by running `kill 1 -16` (which runs it with signal 16). In both commands the 1 just provides a process as part of the expected parameters for the command and isn't actually used.

While this rootkit is loaded everyone will be unable to write to `/root/king.txt` regardless of whether they use sudo or are a kernel module, if you would like to write to it you must ensure that the module is unloaded first. Upon trying to write to it you will get an `Invalid permissions` error. Additionally, there's an option (found on line 4 of `functions/openAt.c`) which can be used to close any process that attempts to write to it. To enable this option you must set closeProcess to 1.
