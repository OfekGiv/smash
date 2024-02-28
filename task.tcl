set processNum [lindex $argv 0]
puts "Task started with process number $processNum"
for {set i 0} {$i < 5} {incr i} {
    puts "Iteration $i"
	after 5000
}
puts "Task finished with process number $processNum"