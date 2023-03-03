# 'ltelog' is a firmware for Wio LTE it receives log texts from UART and sends them to the internet via UDP. 

# Specifications

- default UART baud: 115200 bps
- default receive buffer size: 64KB
- default send interval: 15000ms(15s)
- default SIM parameter: soracom.io / sora / sora
- default domain and port: beam.soracom.io:23080

# Operation

- Always receives log texts from UART and write them to the buffer(64KB default).
- Get the first 1460 bytes from the buffer and send them to the internet every 10 seconds(default).
- The send size will be less than 1460 bytes if there is any LF character in the first 1460bytes of the buffer to help the receiving system.

# Why is the interval 15 seconds?

- If you keep sending 1460 bytes every 15 seconds, you should not exceed 300MB in a month.

# Links

- Wio LTE	https://wiki.seeedstudio.com/Wio_LTE_Cat.1/
- build environment	https://github.com/paijp/arduino-cli-docker
- soracom beam(converts UDP to HTTPS)	https://developers.soracom.io/en/docs/beam/

