#!/usr/bin/perl

use strict;
use warnings;

# Output file path
 my $output_file = 'output/Ghonim_Nordstrom_output4_(1)_mac_os1.txt';
# my $output_file = 'output/Ghonim_Nordstrom_output4_(2)_mac_os.txt';
# my $output_file = 'output/Ghonim_Nordstrom_output4_(1)_linux.txt';
# my $output_file = 'output/Ghonim_Nordstrom_output4_(1)_linux.txt';

# Open the file for writing
open(my $fh, '>', $output_file) or die "Could not open file '$output_file' $!";

# Print the header to the file with fixed widths for each column
printf $fh "%-20s %-15s %-15s\n", "Number of Threads", "Time (Seconds)", "Speedup";

# Variable to store time for single-thread execution
my $single_process_time;

# Run the program with different numbers of processes
for (my $num_processes = 1; $num_processes <= 11; $num_processes++) {
    # Execute the MPI program and capture its output
    my $output = `mpirun -np $num_processes ./bin/Heat_Diff_MPI`;
    #my $output = `mpirun -np $num_processes ./HW4_2`;

    # Extract the time from the output. 
    my ($time_in_seconds) = $output =~ /Execution Time: ([\d\.]+) seconds/;

    if (defined $time_in_seconds) {
        # Calculate speedup. Avoid division by zero by checking if $num_processes is 1.
        my $speedup = $num_processes == 1 ? 1 : ($single_process_time / $time_in_seconds); # // is the defined-or operator, prevents use of uninitialized value
        $single_process_time = $time_in_seconds if $num_processes == 1;

        # Write the results to the output file with fixed widths for each column
        printf $fh "%-20d %-15f %-15f\n", $num_processes, $time_in_seconds, $speedup;
    } else {
        warn "Could not extract execution time for $num_processes processes";
    }
}

# Close the file
close $fh;

print "Results saved in $output_file\n";
