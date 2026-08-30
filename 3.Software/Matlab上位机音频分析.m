function voiceSignalAnalyzer()
clc;
clear all;
close all;
    mainFig = figure('Name', '语音分析系统主界面', ...
                    'Units', 'normalized', ...
                    'Position', [0.2 0.2 0.4 0.4], ...
                    'NumberTitle', 'on', ...
                    'MenuBar', 'figure');
    
    % 制作人信息显示
    uicontrol('Style', 'text', ...
             'String', sprintf('制作人：郝成城 胡伟畅\n 指导老师：黄建宇 刘丹\n 发布日期：2025-10\n 电子信息与自动化学院'), ...
             'Units', 'normalized', ...
             'Position', [0.1 0.05 0.8 0.3], ...
             'FontSize', 12, ...
             'HorizontalAlignment', 'center')
      
    uicontrol('Style', 'text', ...
             'String', sprintf('信号的采集与分析处理'), ...
             'Units', 'normalized', ...
             'Position', [0.1 0.6 0.8 0.3], ...
             'FontSize', 24, ...
             'HorizontalAlignment', 'center');
    % 进入分析界面按钮
    uicontrol('Style', 'pushbutton', ...
             'String', '进入分析界面', ...
             'Units', 'normalized', ...
             'Position', [0.3 0.4 0.4 0.2], ...
             'FontSize', 12, ...
             'Callback', @openAnalyzer);
    
  
    
    %% 回调函数：打开分析界面
    function openAnalyzer(~, ~)
        delete(mainFig); % 关闭主界面
        createAnalyzerUI(); % 创建分析界面
    end

    %% 回调函数：录音功能
    function recordAudio(src, ~)
        persistent recorder;
        Fs = 44100; % 采样率
        
        if strcmp(get(src, 'String'), '开始录音')
            % 创建录音对象
            recorder = audiorecorder(Fs, 16, 1);
            record(recorder);
            set(src, 'String', '停止录音', 'BackgroundColor', [1 0.5 0.5]);
        else
            stop(recorder);
            set(src, 'String', '开始录音', 'BackgroundColor', [0.94 0.94 0.94]);
            
            % 获取并保存录音数据
            audio = getaudiodata(recorder);
            [file, path] = uiputfile('*.mp3', '保存录音文件');
            if ~isequal(file, 0)
                fullPath = fullfile(path, file);
                try
                    audiowrite(fullPath, audio, Fs);
                    msgbox('录音已成功保存为MP3文件！');
                catch
                    errordlg('MP3保存失败，请确保已安装支持MP3的编码器');
                end
            end
        end
    end
end

function createAnalyzerUI()
    % 分析界面主函数
    fig = figure('Name', '语音信号分析仪', ...
                'Units', 'normalized', ...
                'Position', [0.1 0.1 0.8 0.8], ...
                'NumberTitle', 'off', ...
                'MenuBar', 'figure');
    
    %% 界面组件
    % 文件操作区域
    uicontrol('Style', 'pushbutton', ...
             'String', '加载语音', ...
             'Units', 'normalized', ...
             'Position', [0.05 0.9 0.1 0.05], ...
             'Callback', @loadAudio);
    
    uicontrol('Style', 'togglebutton', ...
             'String', '实时录音', ...
             'Units', 'normalized', ...
             'Position', [0.16 0.9 0.1 0.05], ...
             'Tag', 'recordButton', ...
             'Callback', @recordAudio);
    
    uicontrol('Style', 'pushbutton', ...
             'String', '实时处理', ...
             'Units', 'normalized', ...
             'Position', [0.27 0.9 0.1 0.05], ...
             'Tag', 'realTimeBtn', ...
             'Callback', @startRealTimeProcessing);
    
    uicontrol('Style', 'pushbutton', ...
             'String', '播放原始', ...
             'Units', 'normalized', ...
             'Position', [0.38 0.9 0.1 0.05], ...
             'Enable', 'off', ...
             'Tag', 'playOriginalBtn', ...
             'Callback', @playOriginal);
    
    uicontrol('Style', 'pushbutton', ...
             'String', '播放加噪后', ...
             'Units', 'normalized', ...
             'Position', [0.49 0.9 0.1 0.05], ...
             'Enable', 'off', ...
             'Tag', 'playNoisyBtn', ...
             'Callback', @playNoisy);
    
    uicontrol('Style', 'pushbutton', ...
             'String', '播放滤波后', ...
             'Units', 'normalized', ...
             'Position', [0.6 0.9 0.1 0.05], ...
             'Enable', 'off', ...
             'Tag', 'playFilteredBtn', ...
             'Callback', @playFiltered);
    
    % 噪声控制区域
    noisePanel = uipanel('Title', '噪声设置', ...
                        'Units', 'normalized', ...
                        'Position', [0.05 0.75 0.2 0.15]);
    
    uicontrol(noisePanel, ...
             'Style', 'popupmenu', ...
             'String', {'高斯白噪声','正弦噪声'}, ...
             'Units', 'normalized', ...
             'Position', [0.1 0.6 0.8 0.3], ...
             'Tag', 'noiseTypeMenu', ...
             'Callback', @toggleNoiseParams);
    
    snrEdit = uicontrol(noisePanel, ...
             'Style', 'edit', ...
             'String', '20', ...
             'Units', 'normalized', ...
             'Position', [0.1 0.3 0.35 0.3], ...
             'Tag', 'snrEdit', ...
             'Tooltip', '信噪比(dB)');
    
    freqEdit = uicontrol(noisePanel, ...
             'Style', 'edit', ...
             'String', '1000', ...
             'Units', 'normalized', ...
             'Position', [0.55 0.3 0.35 0.3], ...
             'Tag', 'freqEdit', ...
             'Visible', 'off', ...
             'Tooltip', '正弦频率(Hz)');
    
    uicontrol(noisePanel, ...
             'Style', 'pushbutton', ...
             'String', '添加噪声', ...
             'Units', 'normalized', ...
             'Position', [0.3 0.05 0.4 0.25], ...
             'Callback', @addNoise);
    
    % 滤波器区域
    filterPanel = uipanel('Title', '滤波器设置', ...
                         'Units', 'normalized', ...
                         'Position', [0.05 0.55 0.2 0.15]);
    
    uicontrol(filterPanel, ...
             'Style', 'popupmenu', ...
             'String', {'低通滤波器', '高通滤波器', '带通滤波器'}, ...
             'Units', 'normalized', ...
             'Position', [0.1 0.6 0.8 0.3], ...
             'Tag', 'filterTypeMenu');
    
    cutoffFreqEdit = uicontrol(filterPanel, ...
              'Style', 'edit', ...
              'String', '1000', ...
              'Units', 'normalized', ...
              'Position', [0.1 0.3 0.8 0.3], ...
              'Tag', 'cutoffFreqEdit', ...
              'Tooltip', '截止频率(Hz)');
    
    uicontrol(filterPanel, ...
             'Style', 'pushbutton', ...
             'String', '应用滤波器', ...
             'Units', 'normalized', ...
             'Position', [0.3 0.05 0.4 0.25], ...
             'Callback', @applyFilter);
    
    % 实时处理区域
    realTimePanel = uipanel('Title', '实时处理设置', ...
                           'Units', 'normalized', ...
                           'Position', [0.05 0.35 0.2 0.15]);
    
    uicontrol(realTimePanel, ...
             'Style', 'popupmenu', ...
             'String', {'无滤波', '低通滤波', '高通滤波'}, ...
             'Units', 'normalized', ...
             'Position', [0.1 0.6 0.8 0.3], ...
             'Tag', 'realTimeFilterMenu');
    
    realTimeCutoffEdit = uicontrol(realTimePanel, ...
             'Style', 'edit', ...
             'String', '2000', ...
             'Units', 'normalized', ...
             'Position', [0.1 0.3 0.8 0.3], ...
             'Tag', 'realTimeCutoffEdit', ...
             'Tooltip', '实时滤波截止频率(Hz)');
    
    % 图形显示区域
    axTimeOriginal = axes('Units', 'normalized', ...
                         'Position', [0.3 0.6 0.6 0.3], ...
                         'Box', 'on', ...
                         'Tag', 'axTimeOriginal');
    
    axFreqOriginal = axes('Units', 'normalized', ...
                         'Position', [0.3 0.25 0.6 0.3], ...
                         'Box', 'on', ...
                         'Tag', 'axFreqOriginal');
    
    axTimeNoisy = axes('Units', 'normalized', ...
                      'Position', [0.3 0.6 0.6 0.3], ...
                      'Box', 'on', ...
                      'Visible', 'off', ...
                      'Tag', 'axTimeNoisy');
    
    axFreqNoisy = axes('Units', 'normalized', ...
                      'Position', [0.3 0.25 0.6 0.3], ...
                      'Box', 'on', ...
                      'Visible', 'off', ...
                      'Tag', 'axFreqNoisy');
    
    axTimeFiltered = axes('Units', 'normalized', ...
                      'Position', [0.3 0.6 0.6 0.3], ...
                      'Box', 'on', ...
                      'Visible', 'off', ...
                      'Tag', 'axTimeFiltered');
    
    axFreqFiltered = axes('Units', 'normalized', ...
                      'Position', [0.3 0.25 0.6 0.3], ...
                      'Box', 'on', ...
                      'Visible', 'off', ...
                      'Tag', 'axFreqFiltered');
    
    % 实时处理图形区域
    axRealTimeTime = axes('Units', 'normalized', ...
                         'Position', [0.3 0.6 0.6 0.3], ...
                         'Box', 'on', ...
                         'Visible', 'off', ...
                         'Tag', 'axRealTimeTime');
    
    axRealTimeFreq = axes('Units', 'normalized', ...
                         'Position', [0.3 0.25 0.6 0.3], ...
                         'Box', 'on', ...
                         'Visible', 'off', ...
                         'Tag', 'axRealTimeFreq');
    
    % 信息显示区域
    infoPanel = uicontrol('Style', 'text', ...
                         'Units', 'normalized', ...
                         'Position', [0.05 0.05 0.2 0.2], ...
                         'HorizontalAlignment', 'left', ...
                         'FontSize', 9);
    
    %% 数据存储结构
    audioData = struct('y', [], 'y1', [], 'y_noisy', [], 'y_filtered', [], 'Fs', [], ...
                      'noiseType', '无', 'SNR', NaN, 'noiseFreq', NaN, 'filterType', '无', 'cutoffFreq', NaN, ...
                      'filterSNR', NaN, 'THD', NaN, 'SINAD', NaN, ...
                      'originalSNR', NaN, 'originalTHD', NaN, 'originalSINAD', NaN, ...
                      'isRealTime', false, 'realTimeRecorder', [], 'realTimeTimer', [], ...
                      'realTimeFilterType', '无', 'realTimeCutoff', 2000);
    guidata(fig, audioData);
    
    %% 回调函数：加载音频
    function loadAudio(~, ~)
        [file, path] = uigetfile({'*.wav;*.mp3;*.ogg;*.flac', '音频文件'},...
                                '选择语音文件');
        if isequal(file, 0), return; end
        
        try
            fullPath = fullfile(path, file);
            [y, Fs] = audioread(fullPath);
            
            % 如果是立体声，转换为单通道
            if size(y,2) > 1
                y1 = mean(y, 2); % 将立体声转换为单通道
            else
                y1 = y;
            end
            
            % 更新数据存储
            audioData.y = y;
            audioData.y1 = y1;
            audioData.Fs = Fs;
            audioData.y_noisy = [];
            audioData.y_filtered = [];
            guidata(fig, audioData);
            
            % 绘制原始信号
            plotSignal(axTimeOriginal, axFreqOriginal, y1, Fs, '原始信号', 'b');
            
            % 计算原始信号的信噪比和失真度
            [snr_value, thd_percent, sinad_value] = calculateSignalMetrics(y1, Fs);
            
            % 保存原始信号指标
            audioData.originalSNR = snr_value;
            audioData.originalTHD = thd_percent;
            audioData.originalSINAD = sinad_value;
            guidata(fig, audioData);
            
            % 启用功能按钮
            set(findobj(fig, 'Tag', 'playOriginalBtn'), 'Enable', 'on');
            
            % 更新信息面板，显示原始信号质量指标
            infoStr = {['文件已加载：', file], ...
                      ['采样率: ', num2str(Fs), ' Hz'], ...
                      ['时长: ', sprintf('%.2f', length(y1)/Fs), ' 秒'], ...
                      ['原始信号质量指标:'], ...
                      ['  SNR: ', sprintf('%.2f dB', snr_value)], ...
                      ['  THD: ', sprintf('%.2f%%', thd_percent)], ...
                      ['  SINAD: ', sprintf('%.2f dB', sinad_value)]};
            set(infoPanel, 'String', infoStr);
            
        catch ME
            errordlg(['文件读取错误: ' ME.message]);
        end
    end

    %% 噪声生成函数
    function y_noisy = addGaussianNoise(y, SNR)
        signalPower = mean(y.^2);
        noisePower = signalPower / (10^(SNR/10));
        noise = sqrt(noisePower) * randn(size(y));
        y_noisy = y + noise;
        y_noisy = y_noisy / max(abs(y_noisy(:)));
    end

    function y_noisy = addSineNoise(y, freq, Fs)
        t = (0:length(y)-1)/Fs;
        noise = 0.1 * sin(2*pi*freq*t);
        if size(y,2) > 1
            noise = repmat(noise', 1, size(y,2));
        end
        y_noisy = y + noise;
        y_noisy = y_noisy / max(abs(y_noisy(:)));
    end
    
    %% 录音功能
    function recordAudio(src, ~)
        audioData = guidata(fig);
        
        if strcmp(get(src, 'String'), '实时录音')
            % 开始录音
            Fs = 44100;
            recorder = audiorecorder(Fs, 16, 1);
            record(recorder);
            set(src, 'String', '停止录音', 'BackgroundColor', [1 0 0]);
            
            % 保存录音器对象
            audioData.recordRecorder = recorder;
            guidata(fig, audioData);
        else
            % 停止录音并处理
            audioData = guidata(fig);
            if isfield(audioData, 'recordRecorder') && ~isempty(audioData.recordRecorder)
                stop(audioData.recordRecorder);
            end
            set(src, 'String', '实时录音', 'BackgroundColor', [0.94 0.94 0.94]);
            
            % 获取录音数据
            y = getaudiodata(audioData.recordRecorder);
            Fs = 44100;
            
            % 自动加载录音数据
            audioData.y = y;
            audioData.y1 = y;
            audioData.Fs = Fs;
            audioData.y_noisy = [];
            audioData.y_filtered = [];
            guidata(fig, audioData);
            
            % 更新显示
            plotSignal(axTimeOriginal, axFreqOriginal, y, Fs, '原始信号', 'b');
            
            % 计算原始信号的信噪比和失真度
            [snr_value, thd_percent, sinad_value] = calculateSignalMetrics(y, Fs);
            
            % 保存原始信号指标
            audioData.originalSNR = snr_value;
            audioData.originalTHD = thd_percent;
            audioData.originalSINAD = sinad_value;
            guidata(fig, audioData);
            
            % 启用播放原始按钮
            set(findobj(fig, 'Tag', 'playOriginalBtn'), 'Enable', 'on');
            
            % 更新信息面板，显示原始信号质量指标
            infoStr = {'已加载实时录音信号', ...
                      ['采样率: ', num2str(Fs), ' Hz'], ...
                      ['时长: ', sprintf('%.2f', length(y)/Fs), ' 秒'], ...
                      ['原始信号质量指标:'], ...
                      ['  SNR: ', sprintf('%.2f dB', snr_value)], ...
                      ['  THD: ', sprintf('%.2f%%', thd_percent)], ...
                      ['  SINAD: ', sprintf('%.2f dB', sinad_value)]};
            set(infoPanel, 'String', infoStr);
        end
    end
    
    %% 实时处理功能
    function startRealTimeProcessing(~, ~)
        audioData = guidata(fig);
        
        if ~audioData.isRealTime
            % 开始实时处理
            Fs = 44100;
            bufferSize = 1024; % 缓冲区大小
            
            % 创建录音对象
            recorder = audiorecorder(Fs, 16, 1);
            record(recorder);
            
            % 获取实时处理参数
            filterType = get(findobj(realTimePanel, 'Tag', 'realTimeFilterMenu'), 'String');
            filterType = filterType{get(findobj(realTimePanel, 'Tag', 'realTimeFilterMenu'), 'Value')};
            cutoffFreq = str2double(get(findobj(realTimePanel, 'Tag', 'realTimeCutoffEdit'), 'String'));
            
            % 设计滤波器
            switch filterType
                case '低通滤波'
                    d = designfilt('lowpassiir', 'FilterOrder', 6, 'HalfPowerFrequency', cutoffFreq/(Fs/2));
                case '高通滤波'
                    d = designfilt('highpassiir', 'FilterOrder', 6, 'HalfPowerFrequency', cutoffFreq/(Fs/2));
                otherwise
                    d = [];
            end
            
            % 创建定时器用于实时处理
            timerObj = timer('ExecutionMode', 'fixedRate', ...
                           'Period', 0.1, ... % 100ms更新一次
                           'TimerFcn', @realTimeUpdate);
            
            % 保存状态
            audioData.isRealTime = true;
            audioData.realTimeRecorder = recorder;
            audioData.realTimeTimer = timerObj;
            audioData.realTimeFilterType = filterType;
            audioData.realTimeCutoff = cutoffFreq;
            audioData.realTimeFilter = d;
            audioData.realTimeBuffer = [];
            guidata(fig, audioData);
            
            % 更新按钮状态
            set(findobj(fig, 'Tag', 'realTimeBtn'), 'String', '停止实时', 'BackgroundColor', [1 0 0]);
            
            % 显示实时处理图形
            set([axRealTimeTime, axRealTimeFreq], 'Visible', 'on');
            set([axTimeOriginal, axFreqOriginal, axTimeNoisy, axFreqNoisy, axTimeFiltered, axFreqFiltered], 'Visible', 'off');
            
            % 启动定时器
            start(timerObj);
            
            % 更新信息面板
            infoStr = {'实时处理已启动', ...
                      ['滤波类型: ', filterType], ...
                      ['截止频率: ', num2str(cutoffFreq), ' Hz'], ...
                      ['请说话或播放音频...']};
            set(infoPanel, 'String', infoStr);
            
        else
            % 停止实时处理
            stop(audioData.realTimeTimer);
            delete(audioData.realTimeTimer);
            stop(audioData.realTimeRecorder);
            
            % 重置状态
            audioData.isRealTime = false;
            audioData.realTimeRecorder = [];
            audioData.realTimeTimer = [];
            audioData.realTimeBuffer = [];
            guidata(fig, audioData);
            
            % 更新按钮状态
            set(findobj(fig, 'Tag', 'realTimeBtn'), 'String', '实时处理', 'BackgroundColor', [0.94 0.94 0.94]);
            
            % 隐藏实时处理图形
            set([axRealTimeTime, axRealTimeFreq], 'Visible', 'off');
            set([axTimeOriginal, axFreqOriginal], 'Visible', 'on');
            
            % 更新信息面板
            set(infoPanel, 'String', '实时处理已停止');
        end
        
        function realTimeUpdate(~, ~)
            audioData = guidata(fig);
            if ~audioData.isRealTime, return; end
            
            % 获取最新的音频数据
            recorder = audioData.realTimeRecorder;
            currentSample = recorder.TotalSamples;
            
            if currentSample > 0
                % 读取最新的缓冲区数据
                startSample = max(1, currentSample - 2048 + 1);
                y = getaudiodata(recorder, 'double');
                y = y(startSample:end);
                
                if length(y) >= 512
                    % 应用实时滤波
                    if ~isempty(audioData.realTimeFilter)
                        y_filtered = filtfilt(audioData.realTimeFilter, y);
                    else
                        y_filtered = y;
                    end
                    
                    % 计算实时信号质量指标
                    [snr_value, thd_percent, sinad_value] = calculateSignalMetrics(y_filtered, 44100);
                    
                    % 更新实时显示
                    plotRealTimeSignal(axRealTimeTime, axRealTimeFreq, y_filtered, 44100, '实时信号', 'b');
                    
                    % 更新信息面板
                    infoStr = {'实时处理中...', ...
                              ['滤波类型: ', audioData.realTimeFilterType], ...
                              ['截止频率: ', num2str(audioData.realTimeCutoff), ' Hz'], ...
                              ['实时信号质量指标:'], ...
                              ['  SNR: ', sprintf('%.2f dB', snr_value)], ...
                              ['  THD: ', sprintf('%.2f%%', thd_percent)], ...
                              ['  SINAD: ', sprintf('%.2f dB', sinad_value)]};
                    set(infoPanel, 'String', infoStr);
                end
            end
        end
    end
    
    %% 基于FFT的信噪比计算函数
    function [snr, thd_percent, sinad] = calculateSignalMetrics(y, Fs)
        if isempty(y) || isempty(Fs)
            snr = NaN;
            thd_percent = NaN;
            sinad = NaN;
            return;
        end
        
        % 确保信号是单通道
        if size(y,2) > 1
            y = mean(y, 2);
        end
        
        % 去除直流分量
        y = y - mean(y);
        
        % 选择适当的FFT长度
        N_fft = min(256, length(y));
        if N_fft < 64
            N_fft = 64;
        end
        
        % 应用汉宁窗
        hanning_win = 0.5 - 0.5 * cos(2 * pi * (0:N_fft-1) / (N_fft-1));
        
        % 准备FFT输入数据
        y_segment = y(1:N_fft) .* hanning_win';
        
        % 执行FFT
        Y = fft(y_segment, N_fft);
        Y_mag = abs(Y) / N_fft;
        
        % 去除直流分量影响
        Y_mag(1) = 0;
        
        % 寻找基波位置
        [fundamentalAmplitude, fundBin] = max(Y_mag(2:floor(N_fft/2)));
        fundBin = fundBin + 1;
        
        % 计算基波功率
        startBin = max(2, fundBin - 2);
        endBin = min(floor(N_fft/2), fundBin + 2);
        fundamentalPower = sum(Y_mag(startBin:endBin).^2);
        
        % 计算总功率
        totalPower = sum(Y_mag(2:floor(N_fft/2)).^2);
        
        % 计算谐波功率
        harmonicPower = 0;
        for h = 2:5
            harmonicBin = round(fundBin * h);
            if harmonicBin <= floor(N_fft/2)
                harStart = max(2, harmonicBin - 1);
                harEnd = min(floor(N_fft/2), harmonicBin + 1);
                harmonicPower = harmonicPower + sum(Y_mag(harStart:harEnd).^2);
            end
        end
        
        % 计算噪声功率
        noisePower = totalPower - fundamentalPower - harmonicPower;
        
        % 防错处理
        if noisePower <= 0
            noisePower = totalPower * 0.01;
        end
        if fundamentalPower <= 0
            fundamentalPower = totalPower * 0.5;
        end
        
        % 计算指标
        if noisePower > 0
            snr = 10 * log10(fundamentalPower / noisePower);
        else
            snr = 60;
        end
        
        if fundamentalPower > 0
            thd_voltage_ratio = sqrt(harmonicPower / fundamentalPower);
            thd_percent = 100 * thd_voltage_ratio;
        else
            thd_percent = 0;
        end
        
        if (noisePower + harmonicPower) > 0
            sinad = 10 * log10(fundamentalPower / (noisePower + harmonicPower));
        else
            sinad = snr;
        end
        
        % 限制指标范围
        snr = max(min(snr, 100), 0);
        thd_percent = max(min(thd_percent, 100), 0);
        sinad = max(min(sinad, 100), 0);
    end

    %% 回调函数：添加噪声
    function addNoise(~, ~)
        audioData = guidata(fig);
        if isempty(audioData.y)
            errordlg('请先加载音频文件');
            return;
        end
        
        % 获取噪声参数
        noiseType = get(findobj(noisePanel, 'Tag', 'noiseTypeMenu'), 'String');
        noiseType = noiseType{get(findobj(noisePanel, 'Tag', 'noiseTypeMenu'), 'Value')};
        
        try
            switch noiseType
                case '高斯白噪声'
                    SNR = str2double(get(findobj(noisePanel, 'Tag', 'snrEdit'), 'String'));
                    y_noisy = addGaussianNoise(audioData.y1, SNR);
                    
                case '正弦噪声'
                    freq = str2double(get(findobj(noisePanel, 'Tag', 'freqEdit'), 'String'));
                    if freq > audioData.Fs/2
                        error('正弦频率超过奈奎斯特频率');
                    end
                    y_noisy = addSineNoise(audioData.y1, freq, audioData.Fs);
            end
            
            % 更新数据存储
            audioData.y_noisy = y_noisy;
            audioData.noiseType = noiseType;
            guidata(fig, audioData);
            
            % 显示加噪信号
            plotSignal(axTimeNoisy, axFreqNoisy, y_noisy, audioData.Fs, '加噪信号', 'r');
            set([axTimeNoisy, axFreqNoisy], 'Visible', 'on');
            set([axTimeOriginal, axFreqOriginal], 'Visible', 'off');
            
            % 计算信号质量指标
            [snr_value, thd_percent, sinad_value] = calculateSignalMetrics(y_noisy, audioData.Fs);
            
            % 更新信息面板
            infoStr = {['噪声类型: ' noiseType], ...
                      ['添加时间: ' datestr(now)], ...
                      ['加噪后信号质量指标:'], ...
                      ['  SNR: ' sprintf('%.2f dB', snr_value)], ...
                      ['  THD: ' sprintf('%.2f%%', thd_percent)], ...
                     
                      ['原始信号质量指标:'], ...
                      ['  SNR: ' sprintf('%.2f dB', audioData.originalSNR)], ...
                      ['  THD: ' sprintf('%.2f%%', audioData.originalTHD)], ...
                      ['  SINAD: ' sprintf('%.2f dB', audioData.originalSINAD)]};
            if strcmp(noiseType, '高斯白噪声')
                infoStr{end+1} = ['目标SNR: ' num2str(SNR) ' dB'];
            else
                infoStr{end+1} = ['频率: ' num2str(freq) ' Hz'];
            end
            set(infoPanel, 'String', infoStr);
            
            % 启用播放加噪后按钮
            set(findobj(fig, 'Tag', 'playNoisyBtn'), 'Enable', 'on');
            
        catch ME
            errordlg(['参数错误: ' ME.message]);
        end
    end

    %% 回调函数：应用滤波器
    function applyFilter(~, ~)
        audioData = guidata(fig);
        if isempty(audioData.y)
            errordlg('请先加载音频文件');
            return;
        end
        
        % 确定要滤波的信号
        if ~isempty(audioData.y_noisy)
            signalToFilter = audioData.y_noisy;
            signalType = '加噪信号';
        else
            signalToFilter = audioData.y1;
            signalType = '原始信号';
        end
        
        % 获取滤波器参数
        filterType = get(findobj(filterPanel, 'Tag', 'filterTypeMenu'), 'String');
        filterType = filterType{get(findobj(filterPanel, 'Tag', 'filterTypeMenu'), 'Value')};
        cutoffFreq = str2double(get(findobj(filterPanel, 'Tag', 'cutoffFreqEdit'), 'String'));
        
        % 确保截止频率不超过奈奎斯特频率
        if cutoffFreq > audioData.Fs/2
            cutoffFreq = audioData.Fs/2;
            set(findobj(filterPanel, 'Tag', 'cutoffFreqEdit'), 'String', num2str(cutoffFreq));
        end
        
        % 设计滤波器
        switch filterType
            case '低通滤波器'
                d = designfilt('lowpassiir', 'FilterOrder', 8, 'HalfPowerFrequency', cutoffFreq/audioData.Fs*2);
            case '高通滤波器'
                d = designfilt('highpassiir', 'FilterOrder', 8, 'HalfPowerFrequency', cutoffFreq/audioData.Fs*2);
            case '带通滤波器'
                d = designfilt('bandpassiir', 'FilterOrder', 8, 'HalfPowerFrequency1', 300/audioData.Fs*2, 'HalfPowerFrequency2', cutoffFreq/audioData.Fs*2);
        end
        
        % 应用滤波器
        y_filtered = filtfilt(d, signalToFilter);
        y_filtered = y_filtered / max(abs(y_filtered(:)));
        
        % 计算信号质量指标
        [snr_value, thd_percent, sinad_value] = calculateSignalMetrics(y_filtered, audioData.Fs);
        
        % 更新数据存储
        audioData.y_filtered = y_filtered;
        audioData.filterType = filterType;
        audioData.cutoffFreq = cutoffFreq;
        audioData.filterSNR = snr_value;
        audioData.THD = thd_percent;
        audioData.SINAD = sinad_value;
        guidata(fig, audioData);
        
        % 显示滤波后的信号
        plotSignal(axTimeFiltered, axFreqFiltered, y_filtered, audioData.Fs, '滤波后信号', 'g');
        set([axTimeFiltered, axFreqFiltered], 'Visible', 'on');
        set([axTimeOriginal, axFreqOriginal, axTimeNoisy, axFreqNoisy], 'Visible', 'off');
        
        % 更新信息面板
        infoStr = {['滤波器类型: ' filterType], ...
                  ['截止频率: ' num2str(cutoffFreq) ' Hz'], ...
                  ['滤波时间: ' datestr(now)], ...
                  ['滤波对象: ' signalType], ...
                  ['滤波后信号质量指标:'], ...
                  ['  SNR: ' sprintf('%.2f dB', snr_value)], ...
                  ['  THD: ' sprintf('%.2f%%', thd_percent)], ...
                  ['  SINAD: ' sprintf('%.2f dB', sinad_value)]};
        if strcmp(signalType, '原始信号')
            infoStr{end+1} = ['原始信号质量指标:'];
            infoStr{end+1} = ['  SNR: ' sprintf('%.2f dB', audioData.originalSNR)];
            infoStr{end+1} = ['  THD: ' sprintf('%.2f%%', audioData.originalTHD)];
            infoStr{end+1} = ['  SINAD: ' sprintf('%.2f dB', audioData.originalSINAD)];
        end
        set(infoPanel, 'String', infoStr);
        
        % 启用播放滤波后按钮
        set(findobj(fig, 'Tag', 'playFilteredBtn'), 'Enable', 'on');
    end

    %% 实时信号绘图函数
    function plotRealTimeSignal(axTime, axFreq, y, Fs, titleStr, color)
        % 时域绘图
        t = (0:length(y)-1)/Fs;
        plot(axTime, t, y, color);
        set(axTime, 'XGrid','on', 'YGrid','on', ...
                   'XLim', [0 t(end)], 'YLim', [-1 1]);
        xlabel(axTime, '时间(秒)');
        ylabel(axTime, '幅度');
        title(axTime, titleStr);
        
        % 频域分析
        L = length(y);
        Y = fft(y);
        P2 = abs(Y/L);
        P_shifted = fftshift(P2);
        f = (-Fs/2 : Fs/L : Fs/2 - Fs/L);
        
        plot(axFreq, f, P_shifted, color);
        set(axFreq, 'XGrid','on', 'YGrid','on', ...
                   'XLim', [-Fs/2 Fs/2]);
        xlabel(axFreq, '频率(Hz)');
        ylabel(axFreq, '|幅度|');
        title(axFreq, '实时频域频谱');
        
        % 刷新图形
        drawnow;
    end

    %% 通用绘图函数
    function plotSignal(axTime, axFreq, y, Fs, titleStr, color)
        % 时域绘图
        t = (0:length(y)-1)/Fs;
        plot(axTime, t, y, color);
        set(axTime, 'XGrid','on', 'YGrid','on', ...
                   'XLim', [0 t(end)], 'YLim', [-1 1]);
        xlabel(axTime, '时间(秒)');
        ylabel(axTime, '幅度');
        title(axTime, titleStr);
        
        % 频域分析
        L = length(y);
        Y = fft(y);
        P2 = abs(Y/L);
        P_shifted = fftshift(P2);
        f = (-Fs/2 : Fs/L : Fs/2 - Fs/L);
        
        plot(axFreq, f, P_shifted, color);
        set(axFreq, 'XGrid','on', 'YGrid','on', ...
                   'XLim', [-Fs/2 Fs/2]);
        xlabel(axFreq, '频率(Hz)');
        ylabel(axFreq, '|幅度|');
        title(axFreq, '频域频谱');
    end

    %% 播放功能
    function playOriginal(~, ~)
        audioData = guidata(fig);
        if ~isempty(audioData.y)
            sound(audioData.y, audioData.Fs);
        end
    end

    function playNoisy(~, ~)
        audioData = guidata(fig);
        if ~isempty(audioData.y_noisy)
            sound(audioData.y_noisy, audioData.Fs);
        end
    end

    function playFiltered(~, ~)
        audioData = guidata(fig);
        if ~isempty(audioData.y_filtered)
            sound(audioData.y_filtered, audioData.Fs);
        end
    end

    %% 参数面板切换
    function toggleNoiseParams(~, ~)
        type = get(findobj(noisePanel, 'Tag', 'noiseTypeMenu'), 'Value');
        if type == 1 % 高斯噪声
            set(findobj(noisePanel, 'Tag', 'snrEdit'), 'Visible', 'on');
            set(findobj(noisePanel, 'Tag', 'freqEdit'), 'Visible', 'off');
        else % 正弦噪声
            set(findobj(noisePanel, 'Tag', 'snrEdit'), 'Visible', 'off');
            set(findobj(noisePanel, 'Tag', 'freqEdit'), 'Visible', 'on');
        end
    end
end