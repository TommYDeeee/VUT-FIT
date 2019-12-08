pkg load signal
[x, Fs] = audioread("sa1.wav");
%x = x - mean(x)
Fs = 16000; N = 512; wlen = 25e-3 * Fs; wshift = 10e-3*Fs; woverlap = wlen - wshift;
win = hamming(wlen)
f = (0:(N/2-1)) / N * Fs;
t = (0:(1 + floor((length(x) - wlen) / wshift) - 1))* wshift/Fs;
X = specgram (x, N, Fs, win, woverlap);
imagesc(t,f,10*log(abs(X).^2));
P = 10*log(abs(X).^2);
set (gca (), "ydir", "normal"); xlabel ("Time"); ylabel ("Frequency"); colormap(jet);

%4 ULOHA
c = 1;
H = zeros(16,length(X));
for a = 1:331;
  for b = 1:16; 
    H(1,a) = sum(P(1:16,a));
    H(2,a) = sum(P(17:32,a));
    H(3,a) = sum(P(33:48,a));
    H(4,a) = sum(P(49:64,a));
    H(5,a) = sum(P(65:80,a));
    H(6,a) = sum(P(81:96,a));
    H(7,a) = sum(P(97:112,a));
    H(8,a) = sum(P(113:128,a));
    H(9,a) = sum(P(129:143,a));
    H(10,a) = sum(P(144:145,a));
    H(11,a) = sum(P(161:176,a));
    H(12,a) = sum(P(177:192,a));
    H(13,a) = sum(P(193:208,a));
    H(14,a) = sum(P(209:224,a));
    H(15,a) = sum(P(225:240,a));
    H(16,a) = sum(P(241:256,a));
  end
end
f= (0:16);
imagesc(t,f,10*log(abs(H).^2));

