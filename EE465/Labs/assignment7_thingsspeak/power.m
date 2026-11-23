readChannelID = 'YOUR_ID';
readAPIKey = 'YOUR_READ_API_KEY';

%% 1. Read Data %%
[data, time] = thingSpeakRead(readChannelID, 'Fields', [1, 2], 'NumPoints', 30, 'ReadKey', readAPIKey);

% Tag as UTC first, then convert to Pacific Time
time.TimeZone = 'UTC';
time.TimeZone = 'America/Los_Angeles';

temp_C  = data(:, 1);
cpu_pct = data(:, 2);

%% 2. Calculate & Plot Power in Watts %%
cpu_decimal = cpu_pct ./ 100;
power_W = (2.3 .* 100 .* cpu_decimal) + (temp_C .* 0.32);

plot(time, power_W, 'm-o', 'LineWidth', 2);
xlabel('Date / Time');
ylabel('Power (W)');
title('Power Consumption (W)');
grid on;
