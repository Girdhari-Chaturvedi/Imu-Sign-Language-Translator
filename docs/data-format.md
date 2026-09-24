# Data Format

Telemetry example:
DATA,S1,AX=0.120,AY=-0.940,AZ=0.310,GX=2.0,GY=-1.0,GZ=0.5,P=1.2,R=-70.3

Fields: S1-S5 sensor number; AX/AY/AZ acceleration in g; GX/GY/GZ angular velocity in degrees/second; P pitch; R roll.

Gesture example:
SIGN:HELLO

Apps can parse SIGN lines for text-to-speech.
