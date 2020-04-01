# Reliable Transport Protocol

An emulator with two versions: alternating bits and go back to N.

Modified functions calls such as checksum_for(), B_input(), A_output(), A_timer_interrupt(), to_layer_3().

This program asks the user to input the number of package, possibility of lost package and corrupted package, average time interval between messages sent, and trace.

After entering customized data, if trace, the program shows how different scenrios are handled: 

  If package received, a_input() returns the messgae "ACK_______________". 
  
  If package not received (i.e. lost), a_output() will resend the package and reset the timer.
  
  If package is corrupted, B_input() reports a checksum error. Package will be resent from layer 5.

