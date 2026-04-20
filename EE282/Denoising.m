% Filtering High Frequency Noise using Low-Pass/Highpass/Bandpass Circuit Implementation
% And comparing against matlab's inbuilt Signal Processing Functions.
% Given constant Resistance and Capacitance that is randomized every run, the script
% Would solve for a filtered signal.
% Variable Definition:
% R = Resistance in Ohms
% C = Capacitance in microFarads
% f_signal = Desired Signal in the 10-200 Hz range
% fa = Signal Amplitude 0.5 to 1.5
% phi = Phase Constant, random shift
% cs = Clean signal
% f_noise = Randomized High Frequency Noise signal
% fn = Noise signal Amplitude 0.2 to 0.8
% ns = Noisy Signal
% y_comb = combined Clean and Noise induced signals
% Sr = Sampling rate of Frequency
% Ts = Sampling interval
% T = Simulation time in seconds
% RC = time constant Tau
% Alpha = discrete time coefficient, a lower value results in stronger filtering
% ========== Octave-Specific Setup ============================================
% Required:
%   gnuplot, gnumake, gfortran
%   Octave packages: control, signal
%pkg load signal;
%graphics_toolkit("gnuplot");
clc;
clear;
close all;
rng('shuffle');
% ========== User Audio Upload ==========================================
info = audioinfo('testaudio.mp3');
[audioData, Fs] = audioread('testaudio.mp3');
fileDuration = length(audioData) / Fs;
% ========== User Noise Signal Range Input ====================================
vfn1 = false; % status for noise signal lower frequency range check I could % have added different wave types but ehh
while ~vfn1 % While vfn1 is false, the loop keeps going
   f_noise_low = input('Enter NOISE signal minimum frequency in Hz:\n');
   % Lower bound for noise signal frequency input
   if f_noise_low > 0
       vfn1 = true; % Sets status to true
   else
       disp('Invalid value. Minimum noise signal frequency must be greater than 0 Hz.');
       % Error checking for noise signal minimum frequency
   end
end
vfn2 = false; % status for noise signal upper frequency range check
while ~vfn2 % While vfn2 is false, the loop keeps going
   f_noise_high = input('Enter NOISE signal maximum frequency in Hz:\n');
   % Upper bound for noise signal frequency input
   if f_noise_high >= f_noise_low
       vfn2 = true; % Sets status to true
   else
       disp('Invalid value. Maximum noise signal frequency must be greater than minimum frequency.');
       % Error checking for noise signal maximum frequency
   end
end
van1 = false; % status for noise signal lower amplitude range check
while ~van1 % While van1 is false, the loop keeps going
   a_noise_low = input('Enter NOISE signal minimum amplitude:\n');
   % Lower bound for noise signal amplitude input
   if a_noise_low > 0
       van1 = true; % Sets status to true
   else
       disp('Invalid value. Minimum noise signal amplitude must be greater than 0.');
       % Error checking for noise signal minimum amplitude
   end
end
van2 = false; % validity status for noise signal upper amplitude range check
while ~van2 % While van2 is false, the loop keeps going
   a_noise_high = input('Enter NOISE signal maximum amplitude:\n');
   % Upper bound for noise signal amplitude input
   if a_noise_high >= a_noise_low
       van2 = true; % Sets status to true
   else
       disp('Invalid value. Maximum noise signal amplitude must be greater than minimum amplitude.');
       % Error checking for noise signal maximum amplitude
   end
end
vn = false; % status for number of noise components
while ~vn % While vn is false, the loop keeps going
   noise_count = input('Enter number of noise components to sum:\n');
   % Number of individual noise waves to add together
   if noise_count >= 1 && floor(noise_count) == noise_count
       vn = true; % Sets status to true
   else
       disp('Invalid value. Number of noise components must be a whole number greater than or equal to 1.');
       % Error checking for number of noise components
   end
end
% Predefined Variables
Sr = Fs;   % Sampling Rate (Hz)
Ts = 1/Sr;    % Sampling Interval (s)
T  = fileDuration;     % Simulation time (s)
t = (0:size(audioData, 1)-1) / Sr; 
% Noise uses user-selected waveform and user-selected ranges
y_noise = zeros(size(t)); % Set the Noise array to a zero first
for i = 1:noise_count % Loop based on how many noise components the user wants
   noise = signal_rand(t, f_noise_low, f_noise_high, a_noise_low, a_noise_high, 'sine');
   % Signal equation for the noise sine wave (fixed to sine)
   y_noise = y_noise + noise;
   % Combine each noise wave to one another
end
% Combined signal (Noise and Clean Signal)
y_noise = y_noise';
size(y_noise)
size(audioData)
y_comb = audioData + [y_noise, y_noise]; 
% ========== Applying Filters =================================================
vr = false; % status for Resistance range check
while ~vr % While vr is false, the loop keeps going
   R = input('Enter resistance 330 to 2200 ohms(Ω):\n');
   % R value variable initialization
   if R >= 330 && R <= 2200 % Resistance range
       vr = true; % Set status of validity to true
   else
       disp('Invalid value. Resistance must be between 330 and 2200 ohms.');
       % Display to the user what resistance value they should pick
       % Also an error checkin
   end
end
vc = false; % status for Capacitance range check
while ~vc % While vc is false, the loop keeps going
   C_uF = input('Enter capacitance (0.1 to 20 μF):\n');
   % C_uF value is the capacitor value between 0.1 to 20 microFarad
   if C_uF >= 0.1 && C_uF <= 20
       % Ensure value input is between 0.1 and 20 microFarad
       C = C_uF * 1e-6;
       % C value is converted from microFarads to Farads
       vc = true; % Sets validity status to true
   else
       disp('Invalid value. Capacitance must be between 0.1 to 20 μF.');
   end
end
RC = R*C; % Time Constant from R * C
fc = 1/(2*pi*RC); % Frequency Cutoff equation for a low/highpass filter
fprintf('The Cutoff Frequency is %.4f Hz and the time constant of the Circuit is %.4f \n', fc, RC);
% Calling Filter functions such that Low, High, and Bandpass functions are called
% Passing through the arguments of combined signal, sampling interval, time constant
y_low  = rc_lowpass(y_comb(:, 1), Ts, RC);
y_low_r = rc_lowpass(y_comb(:, 2), Ts, RC);
y_low_combined = [y_low, y_low_r];
y_high = rc_highpass(y_comb(:, 1), Ts, RC);
y_bps  = rc_bandpass(y_comb(:, 1), Ts, 30, 300);
y_bps_r = rc_bandpass(y_comb(:, 2), Ts, 30, 300);
y_bps_combined = [y_bps, y_bps_r];
% Built-in Butterworth filters for comparison
% This would simply be lowpass, highpass, and bandpass on Matlab
[b, a]   = butter(2, fc/(Sr/2), 'low');
y_b_low  = filter(b, a, y_comb(:, 1));
y_b_low_r = filter(b, a, y_comb(:, 2));
y_b_low_combined = [y_b_low, y_b_low_r];
[b, a]   = butter(2, fc/(Sr/2), 'high');
y_b_high = filter(b, a, y_comb(:, 1));
[b, a]   = butter(2, [30 300]/(Sr/2), 'bandpass');
y_b_bp   = filter(b, a, y_comb(:, 1));
% ========== Plotting =========================================================
figure(1); clf;
subplot(3,1,1); plot(t, audioData(:, 1), 'r'); title('Clean Signal'); grid on;
subplot(3,1,2); plot(t, y_noise(:, 1), 'b'); title('Noise'); grid on;
subplot(3,1,3); plot(t, y_comb(:, 1), 'm'); title('Combined Signal'); grid on;
figure(2); clf;
subplot(3,1,1); plot(t, y_low, 'g'); title('RC Low-Pass'); grid on;
subplot(3,1,2); plot(t, y_b_low, 'r'); title('Butterworth Low-Pass'); grid on;
subplot(3,1,3);
plot(t, y_low, 'g'); hold on;
plot(t, y_b_low, 'r');
title('Low-Pass Comparison');
grid on;
legend('RC Low-Pass', 'Butterworth Low-Pass');
figure(3); clf;
subplot(3,1,1); plot(t, y_high, 'm'); title('RC High-Pass'); grid on;
subplot(3,1,2); plot(t, y_b_high, 'y'); title('Butterworth High-Pass'); grid on;
subplot(3,1,3);
plot(t, y_high, 'm'); hold on;
plot(t, y_b_high, 'y');
title('High-Pass Comparison');
grid on;
legend('RC High-Pass', 'Butterworth High-Pass');
figure(4); clf;
subplot(3,1,1); plot(t, y_bps, 'c'); title('Manual Bandpass'); grid on;
subplot(3,1,2); plot(t, y_b_bp, 'k'); title('Butterworth Bandpass'); grid on;
subplot(3,1,3);
plot(t, y_bps, 'c'); hold on;
plot(t, y_b_bp, 'k');
title('Bandpass Comparison');
grid on;
legend('Manual Bandpass', 'Butterworth Bandpass');
% ========== Output Files ====================================================
audiowrite('output_withnoise.mp3', y_comb, Fs);
audiowrite('output_filtered.mp3', y_b_low_combined, Fs);
% ========== Local Functions ==================================================
function y = signal_rand(t, rl, rh, ampl, amph, wave_type)
 % first function for creating the clean signal taking arguments
 % time, random frequency range, random amplitude, and wave_type
   A   = ampl + (amph - ampl) * rand(); % Amplitude Calculation
   f   = rl + (rh - rl) * rand(); % Frequency Calculation
   phi = 2*pi * rand(); % Phase Shift Constant
   if strcmp(wave_type, 'sine') % input type of sine
       y = A * sin(2*pi*f*t + phi); % sine wave equation
   elseif strcmp(wave_type, 'square') % input type of square
       y = A * square(2*pi*f*t + phi); % Square wave equation
   elseif strcmp(wave_type, 'triangle') % input type of triangle
       y = A * sawtooth(2*pi*f*t + phi, 0.5); % Triangle wave in matlab
   else
       error('Unknown waveform type');
   end
end
function y = rc_lowpass(x, Ts, RC) % lowpass filter function
   alpha = Ts / (RC + Ts); % discrete time constant (decay)
   y = zeros(size(x));
   y(1) = x(1);
   for k = 2:length(x)
       y(k) = alpha*x(k) + (1-alpha)*y(k-1); % continuous graph
   end
end
function y = rc_highpass(x, Ts, RC) % highpass filter function
   alpha = RC / (RC + Ts); % discrete time constant (decay)
   y = zeros(size(x));
   y(1) = x(1);
   for k = 2:length(x)
       y(k) = alpha * (y(k-1) + x(k) - x(k-1));
       % Differential Equation model Continuous Simulation
   end
end
function y = rc_bandpass(x, Ts, f_low, f_high)
   fc = sqrt(f_low * f_high);
   Q = 1.0;
   omega = 2*pi*fc*Ts;
   alpha = sin(omega) / (2*Q);
   b = [alpha, 0, -alpha] / (1 + alpha);
   a = [1, -2*cos(omega)/(1 + alpha), (1 - alpha)/(1 + alpha)];
   y = filter(b, a, x);
end
