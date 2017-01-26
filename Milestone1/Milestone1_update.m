Fs = 100E3;
T = 1/Fs;
L = 50E3;
f = Fs*(0:(L/2))/L;
t = (0:L-1)*T;

Y = fft(scope29(:,2));
YP2 = abs(Y/L);
YP1 = YP2(1:L/2+1);
YP1(2:end-1) = 2*YP1(2:end-1);

figure(1);
plot(YP1);

Z = fft(scope30(:,2));
ZP2 = abs(Z/L);
ZP1 = ZP2(1:L/2+1);
ZP1(2:end-1) = 2*ZP1(2:end-1);

figure(2);
plot(ZP1);