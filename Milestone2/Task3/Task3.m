PLAYER_FREQUENCIES = [1471 1724 2000 2273 2632 2941 3333 3571 3846 4167];
new_t = linspace(0, .2, 20000);
load('light.mat');
y_new = transpose(y);
y_new = y_new(1:20000);
Fs = 100E3;

total_energy = zeros(1,length(PLAYER_FREQUENCIES));

% Get our IIR filter coefficients.
N = 81;
L_N = (N-1)/2;
n = (-L_N:L_N);
f_corner = 5.5E3;
h_ideal_FIR = 2*f_corner/Fs*sinc(n*2*f_corner/Fs);
b_fir = transpose(hamming(N));
a_fir = zeros(1,81);
a_fir(1) = 1;

for i = 1:length(PLAYER_FREQUENCIES)
    player_square_wave = 0.1*(0.5+0.5*square(2*pi*PLAYER_FREQUENCIES(i)*new_t));
    sum_wave = player_square_wave + y_new;
    
    figure(1);
    subplot(length(PLAYER_FREQUENCIES)/2,2,i);
    plot(sum_wave);
    ylim([-1 1.5])
    xlim([0 400])
    ylabel('Voltage (V)');
    xlabel('Time (ms)');
    title(sprintf('Player %d Square Wave + Noise (Time Domain)',i));

    if i == 1
        figure (2);
        subplot(2,2,1);
        plot(player_square_wave);
        ylim([-1 1]);
        xlim([0 400]);
        ylabel('Voltage (V)');
        xlabel('Samples');
        title(sprintf('Player %d Square Wave', i));
        
        subplot(2,2,2);
        plot(y_new);
        ylim([-1 1.5]);
        xlim([0 400]);
        ylabel('Voltage (V)');
        xlabel('Time (ms)');
        title('Noise from Ambient Light');
        
        subplot(2,2,3)
        plot(sum_wave)
        ylim([-1 1.5])
        xlim([0 400])
        ylabel('Voltage (V)');
        xlabel('Time (ms)');
        title(sprintf('Player %d Square Wave + Noise (Time Domain)',i));
        
        sum_wave_fft = fft(sum_wave);
        l_sum_wave_fft = length(sum_wave_fft);
        sum_wave_fft_x1 = sum_wave_fft(1:l_sum_wave_fft/2);
        f_sum_wave_fft = linspace(0, Fs/2, l_sum_wave_fft/2);
        subplot(2,2,4)
        plot(f_sum_wave_fft, abs(sum_wave_fft_x1))
        xlim([0 50E3])
        ylabel('Voltage (mV)');
        xlabel('Frequency (Hz)');
        title(sprintf('Player %d Square Wave + Noise (Frequency Domain)',i));
    end
    
    sum_wave_filter = filter(b_fir,a_fir, sum_wave);
    sum_wave_decimated_DS = sum_wave_filter(1:10:length(sum_wave_filter));
    
    for j = 1:length(PLAYER_FREQUENCIES)
        filtered_at_player = filter(b1(j,:), a1(j,:), sum_wave_decimated_DS);
        total_energy(j) = sum(abs(filtered_at_player).^2);
        
        filtered_player_fft = fft(filtered_at_player);
        l_filtered_player_fft = length(filtered_player_fft);
        filtered_player_fft_x1 = filtered_player_fft(1:l_filtered_player_fft/2);
        F_filtered_player_fft = linspace(0, Fs/2, l_filtered_player_fft/2);
        
        if i == 1 && j == 10
            figure(7);
            plot(F_filtered_player_fft, abs(filtered_player_fft_x1));
        end
        
        figure(4);
        subplot(length(PLAYER_FREQUENCIES)/2,2,j);
        plot(F_filtered_player_fft, abs(filtered_player_fft_x1))
        ylabel('Voltage (mV)');
        xlabel('Frequency (Hz)');
        title(sprintf('Player %d Filtered Sum Wave Frequency Response',j));
        
%         if i == 1 && j == 10
%             sum_wave_fft_DS = fft(sum_wave_decimated_DS);
%             l_sum_wave_fft_DS = length(sum_wave_fft_DS);
%             sum_wave_fft_DS_x1 = sum_wave_fft_DS(1:l_sum_wave_fft_DS/2);
%             F_sum_wave_fft_DS = linspace(0, Fs/2, l_sum_wave_fft_DS/2);
%             
%             figure(5);
%             subplot(2,1,1);
%             plot(F_sum_wave_fft_DS, abs(sum_wave_fft_DS_x1));
%             ylabel('Voltage (mV)');
%             xlabel('Frequency (Hz)');
%             title(sprintf('Player %d Sum Wave (Frequency Domain)',j));
%             
%             subplot(2,1,2);
%             filtered_player_fft = fft(filtered_player);
%             l_filtered_player_fft = length(filtered_player_fft);
%             filtered_player_fft_x1 = filtered_player_fft(1:l_filtered_player_fft/2);
%             F_filtered_player_fft = linspace(0, Fs/2, l_filtered_player_fft/2);
%             plot(F_filtered_player_fft, abs(filtered_player_fft_x1))
%             ylabel('Voltage (mV)');
%             xlabel('Frequency (Hz)');
%             title(sprintf('Player %d Sum Wave Filtered (Frequency Domain)',j));
%         end
    end
    
    figure(6);
    hold on;
    subplot(length(PLAYER_FREQUENCIES)/2,2,i), bar(total_energy);
end