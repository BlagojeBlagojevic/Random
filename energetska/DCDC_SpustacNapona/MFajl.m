Vg=28;
R=4;
L=220e-6;
C=1000e-6;
fs=20e3;
Ts=1/fs;
Tsim=Ts/100;
Vout=100;
D=(Vout/Vg)+1;
Lmin=R*(1-D)/(2*fs);
Ki=62.86;
Kp=0.06;
Kd=4.52e-5;