# tdmesg

print dmesg output with nicer timestamps and level
[64836.573936] ata1.01: status: { DRDY ERR }
[64836.573964] ata1.01: error: { UNC }
[64836.588285] ata1.00: configured for UDMA/133
[64836.596662] ata1.01: configured for UDMA/133
[64836.596703] ata1: EH complete

becomes
[err    ] [2015-10-20 13:22:44]: ata1.01: status: { DRDY ERR }
[err    ] [2015-10-20 13:22:44]: ata1.01: error: { UNC }
[info   ] [2015-10-20 13:22:44]: ata1.00: configured for UDMA/133
[info   ] [2015-10-20 13:22:44]: ata1.01: configured for UDMA/133
[info   ] [2015-10-20 13:22:44]: ata1: EH complete


# Compile with
gcc -O2 -Wall -ansi -posix -pedantic tdmesg.c -o tdmesg

# Author
Matthieu Walter
