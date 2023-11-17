
echo "Testing Sample.txt"

########## DM ############
./cache_simulation -v -t 0 traces/sample.txt

########## TWO-WAY ############
./cache_simulation -v -t 1 traces/sample.txt

########## FOUR-WAY ############
./cache_simulation -v -t 2 traces/sample.txt

########## FULL-ASS ############
./cache_simulation -v -t 3 traces/sample.txt

########## LONG-TRACE ############
./cache_simulation -t 4 traces/long-trace.txt
