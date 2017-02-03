% Constants
PLAYER_FREQUENCIES = [1471 1724 2000 2273 2632 2941 3333 3571 3846 4167];
Fs = 100E3;

% Load the FIR, IIR, and ambient light noise.
load('a_fir.txt');
load('b_fir.txt');
load('a1.txt');
load('b1.txt');
load('light.mat');
y = y(1:20000);
t = t(1:20000);

% Variable for total energy.
total_energy = zeros(1,length(PLAYER_FREQUENCIES));

% Plot the ambient noise.
figure(1);
plot(y);
ylim([-1 1.5]);
xlim([0 400]);
ylabel('Voltage (V)');
xlabel('Time (ms)');
title('Noise from Ambient Light');

% Used for the for loop to graph player information.
plot_count = 1;
plot_figure_count = 2;

for i = 1:length(PLAYER_FREQUENCIES)
    player_square_wave = 0.1*(0.5+0.5*square(2*pi*PLAYER_FREQUENCIES(i)*t));
    sum_wave = player_square_wave + y;
    
    if i == length(PLAYER_FREQUENCIES)/2 + 1
        plot_figure_count = plot_figure_count + 1;
        plot_count = 1;
    end
    
    % Plot the player square wave.
    figure(plot_figure_count);
    subplot(length(PLAYER_FREQUENCIES)/2,3,plot_count);
    plot(player_square_wave);
    ylim([-1 1]);
    xlim([0 400]);
    ylabel('Voltage (V)');
    xlabel('Samples');
    title(sprintf('Player %d Square Wave', i));
    plot_count = plot_count + 1;
        
    % Plot the square wave summed with the ambient noise.
    subplot(length(PLAYER_FREQUENCIES)/2,3,plot_count);
    plot(sum_wave);
    ylim([-1 1.5]);
    xlim([0 400]);
    ylabel('Voltage (V)');
    xlabel('Time (ms)');
    title(sprintf('Player %d Square Wave + Noise (Time Domain)',i));
    plot_count = plot_count + 1;
        
    % Find the FFT of the square wave summed with the ambient noise.
    sum_wave_fft = fft(sum_wave);
    l_sum_wave_fft = length(sum_wave_fft);
    sum_wave_fft_x1 = sum_wave_fft(1:l_sum_wave_fft/2);
    f_sum_wave_fft = linspace(0, Fs/2, l_sum_wave_fft/2);
    
    % Plot the FFT of the square wave summed with the ambient noise.
    subplot(length(PLAYER_FREQUENCIES)/2,3,plot_count);
    plot(f_sum_wave_fft, abs(sum_wave_fft_x1))
    xlim([0 50E3]);
    ylabel('Voltage (mV)');
    xlabel('Frequency (Hz)');
    title(sprintf('Player %d Square Wave + Noise (Frequency Domain)',i));
    plot_count = plot_count + 1;
    
    % Create the filter based on the current player, then down-sample it.
    fprintf('Filter for Player %d\n', i);
    sum_wave_filter = filter(b_fir,a_fir, sum_wave);
    sum_wave_decimated_ds = sum_wave_filter(1:10:length(sum_wave_filter));
    
    for j = 1:length(PLAYER_FREQUENCIES)
        % Filter the noisy signal with the current player's coefficients and sum the total energy.
        filtered_at_player = filter(b1(j,:), a1(j,:), sum_wave_decimated_ds);
        total_energy(j) = sum(abs(filtered_at_player).^2);
        fprintf('\tEnergy for player %d: %f\n', j, total_energy(j));
        
        % Find the FFT of the filtered signal.
        filtered_player_fft = fft(filtered_at_player);
        l_filtered_player_fft = length(filtered_player_fft);
        filtered_player_fft_x1 = filtered_player_fft(1:l_filtered_player_fft/2);
        f_filtered_player_fft = linspace(0, Fs/2, l_filtered_player_fft/2);
        
        figure(4);
        subplot(length(PLAYER_FREQUENCIES)/2,2,j);
        plot(f_filtered_player_fft, abs(filtered_player_fft_x1))
        ylabel('Voltage (mV)');
        xlabel('Frequency (Hz)');
        title(sprintf('Player %d Filtered Noisy Signal (Frequency Domain)',j));
    end
    
    fprintf('\n');
    
    figure(5);
    hold on;
    subplot(length(PLAYER_FREQUENCIES)/2,2,i), bar(total_energy);
end