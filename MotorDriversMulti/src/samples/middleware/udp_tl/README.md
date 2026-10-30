## UDP Transport Layer Example

This example is used to demonstrate the usage of the UDP Transport Layer.
Example is built upon previous example given for SBUS, showcasing the usage and
advantages of using UDP Transport Layer.

### How to use example

Python script `udp-tl-test.py` is used to generate SBUS packet and send it to
MCU via UDP. Each time MCU receives valid SBUS packet green led is toggled and
packet is echoed back to the python script via UDP.