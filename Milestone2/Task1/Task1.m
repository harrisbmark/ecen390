NUMBER_OF_COEFFICIENTS = 11;
P_FREQ = [1471 1724 2000 2273 2632 2941 3333 3571 3846 4167];
FS = 10E3;
BANDWIDTH = 50;
T = linspace(0, 0.2, 2000);

a(length(P_FREQ),NUMBER_OF_COEFFICIENTS) = 0;
b(length(P_FREQ),NUMBER_OF_COEFFICIENTS) = 0;
total_energy(1:length(P_FREQ)) = 0;

for i = 1:length(P_FREQ)
    freq = P_FREQ(i);
    
    f_lower = freq - BANDWIDTH/2;
    f_upper = freq + BANDWIDTH/2;
    [b(i,:),a(i,:)] = butter(5, [f_lower*2/FS f_upper*2/FS]);
    
    figure(1);
    f_axis = linspace(1000, 4500, 2000);
    H1 = freqz(b(i,:), a(i,:), f_axis, FS);
    hold on;
    xlim([0 5000]);
    ylim([0 1.125]);
    plot(f_axis, abs(H1));
    hold off;
    
    figure(2);
    hold on;
    xlim([0 5000]);
    ylim([-100 10]);
    plot(f_axis, 20*log10(abs(H1)));
    hold off;
end

a
b
% save a1.txt a -ascii -double
% save b1.txt b -ascii -double

for i = 1:length(P_FREQ)
    freq = P_FREQ(i);
    x = square(2*pi*freq*T);
    
    for j = 1:length(P_FREQ)
        filtered = filter(b(j,:), a(j,:), x);
        total_energy(j) = sum(abs(filtered).^2);
    end
    
    figure(3);
    hold on;
    subplot(length(P_FREQ)/2,2,i), bar(total_energy);
end