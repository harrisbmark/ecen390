PLAYER_FREQUENCIES = [1471 1724 2000 2273 2632 2941 3333 3571 3846 4167];

Fs = 100E3;
F1 = 3E3;
F2 = 6E3;
A1 = 1;
A2 = 2;

time = linspace(-0.25,0.25,Fs*0.5);
x = A1*sin(2*pi*F1*time) + A2*sin(2*pi*F2*time);

x_ds = x(1:10:length(x));
t_ds = time(1:10:length(time));
Fs_ds = Fs/10;

figure(1);
subplot(2,1,1);
X = fft(x);
L_X = length(X);
X1 = X(1:L_X/2);
F_X = linspace(0,Fs/2,L_X/2);
plot(F_X, abs(X1));

subplot(2,1,2);
X_DS = fft(x_ds);
L_X_DS = length(X_DS);
X_DS1 = X_DS(1:L_X_DS/2);
F_X_DS = linspace(0,Fs/2,L_X_DS/2);
plot(F_X_DS, abs(X_DS1));

figure(2);
subplot(2,1,1);
plot(t,y);
axis tight

subplot(2,1,2);
Fs_light = 1/max(diff(t));
Y_LIGHT = fft(y);
L_LIGHT = length(Y_LIGHT);
Y_LIGHT1 = Y_LIGHT(1:L_LIGHT/2);
F_LIGHT = linspace(0,Fs_light/2,L_LIGHT/2);
plot(F_LIGHT,abs(Y_LIGHT1));

figure(3);
subplot(2,1,1);
plot(E01, E02);
axis tight

subplot(2,1,2);
V_E02 = fft(E02);
L_V_E02 = length(V_E02);
V_E021 = V_E02(1:L_V_E02/2);
F_V_E02 = linspace(0,Fs/2,L_V_E02/2);
plot(F_V_E02,abs(V_E021));
xlim([0 5E3]);

figure(4);
subplot(2,1,1);
t_E01_ds = E01(1:10:length(E01));
v_E02_ds = E02(1:10:length(E02));
Fs_E0_ds = Fs/10;
plot(t_E01_ds,v_E02_ds);
axis tight

subplot(2,1,2);
V_E02_DS = fft(v_E02_ds);
L_V_E02_DS = length(V_E02_DS);
V_E02_DS1 = V_E02_DS(1:L_V_E02_DS/2);
F_V_E02_DS = linspace(0,Fs_E0_ds/2,L_V_E02_DS/2);
plot(F_V_E02_DS,abs(V_E02_DS1));
xlim([0 5E3]);

figure(5);
subplot(2,2,1);
N = 2001;
L_N = (N-1)/2;
n = (-L_N:L_N);
% f_corner = 5.85E3;
f_corner = 5E3;
h_ideal_FIR = 2*f_corner/Fs*sinc(n*2*f_corner/Fs);
plot(n,h_ideal_FIR);
subplot(2,2,2);
f_axis = linspace(0,50E3,2000);
H_ideal_FIR = freqz(h_ideal_FIR,1,f_axis,Fs);
plot(f_axis,abs(H_ideal_FIR));

long_filter_pv = freqz(h_ideal_FIR,1,PLAYER_FREQUENCIES,Fs);
long_filter_ob = freqz(h_ideal_FIR,1,10E3:1E3:50E3,Fs);

subplot(2,2,3);
N = 21;
L_N = (N-1)/2;
n = (-L_N:L_N);
f_corner = 5E3;
h_ideal_FIR = 2*f_corner/Fs*sinc(n*2*f_corner/Fs);
plot(n,h_ideal_FIR);
axis tight
subplot(2,2,4);
f_axis = linspace(0,50E3,2000);
H_ideal_FIR = freqz(h_ideal_FIR,1,f_axis,Fs);
plot(f_axis,abs(H_ideal_FIR));

short_filter_pv = freqz(h_ideal_FIR,1,PLAYER_FREQUENCIES,Fs);
short_filter_ob = freqz(h_ideal_FIR,1,10E3:1E3:50E3,Fs);
%% 

figure(7);
N = 81;
L_N = (N-1)/2;
n = (-L_N:L_N);
f_corner = 5.5E3;
h_ideal_FIR = 2*f_corner/Fs*sinc(n*2*f_corner/Fs);
window = transpose(hamming(N));
plot(n,h_ideal_FIR);
f_axis = linspace(0,50E3,2000);
% H_ideal_FIR = freqz(h_ideal_FIR,1,f_axis,Fs);
ideal_filter = window.*h_ideal_FIR;
FILTERED = freqz(ideal_filter,1,f_axis,Fs);
plot(f_axis,abs(FILTERED));

actual_filter_pv = freqz(ideal_filter,1,PLAYER_FREQUENCIES,Fs);
actual_filter_ob = freqz(ideal_filter,1,10E3:1E3:50E3,Fs);

