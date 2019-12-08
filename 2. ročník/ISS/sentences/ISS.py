# scipy - potreba normalizace
from scipy.io import wavfile
from scipy.signal import spectrogram, lfilter, freqz, tf2zpk
import soundfile as sf
import matplotlib.pyplot as plt
import numpy as np

fs, data = wavfile.read('sa1.wav')
data.min(), data.max()
s, fs = sf.read('sa1.wav')
t = np.arange(s.size) / fs

f, t, sgr = spectrogram(s, fs,nperseg=400, noverlap=240, nfft= 511)
sgr_log = 10 * np.log10(sgr+1e-20) 
plt.figure(figsize=(9, 3))
plt.pcolormesh(t, f, sgr_log)
plt.gca().set_xlabel('Čas [s]')
plt.gca().set_ylabel('Frekvence [Hz]')
cbar = plt.colorbar()
cbar.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)
plt.tight_layout()
plt.savefig('yourfile.pdf')
matrix = np.zeros((16,331)) 
for a in range(331):
        for c in range (0,16,1):
            matrix[0,a] = matrix[0,a] + sgr_log[c,a]
        for c in range (16,32,1):
            matrix[1,a] = matrix[1,a] + sgr_log[c,a]
        for c in range (32,48,1):
            matrix[2,a] = matrix[2,a] + sgr_log[c,a]
        for c in range (48,64,1):
            matrix[3,a] = matrix[3,a] + sgr_log[c,a]
        for c in range (64,80,1):
            matrix[4,a] = matrix[4,a] + sgr_log[c,a]
        for c in range (80,96,1):
            matrix[5,a] = matrix[5,a] + sgr_log[c,a]
        for c in range (96,112,1):
            matrix[6,a] = matrix[6,a] + sgr_log[c,a]
        for c in range (112,128):
            matrix[7,a] = matrix[7,a] + sgr_log[c,a]
        for c in range (128,144,1):
            matrix[8,a] = matrix[8,a] + sgr_log[c,a]
        for c in range (144,160):
            matrix[9,a] = matrix[9,a] + sgr_log[c,a]    
        for c in range (160,176):
            matrix[10,a] = matrix[10,a] + sgr_log[c,a]   
        for c in range (176,192):
            matrix[11,a] = matrix[11,a] + sgr_log[c,a]   
        for c in range (192,208):
            matrix[12,a] = matrix[12,a] + sgr_log[c,a]   
        for c in range (208,224):
            matrix[13,a] = matrix[13,a] + sgr_log[c,a]   
        for c in range (224,240):
            matrix[14,a] = matrix[14,a] + sgr_log[c,a]
        for c in range (240,256):
            matrix[15,a] = matrix[15,a] + sgr_log[c,a]   
f = range(16)       
"""plt.figure(figsize=(9, 3))
plt.pcolormesh(t, f, matrix)
plt.gca().set_xlabel('Čas [s]')
plt.gca().set_ylabel('Frekvence [Hz]')
cbar = plt.colorbar()
cbar.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)
plt.tight_layout()
plt.savefig('yourfile.pdf')"""
print(matrix[1,1])
test = sgr_log[16,1]+sgr_log[17,1]+sgr_log[18,1]+sgr_log[19,1]+sgr_log[20,1]+sgr_log[21,1]+sgr_log[22,1]+sgr_log[23,1]+sgr_log[24,1]+sgr_log[25,1]+sgr_log[26,1]+sgr_log[27,1]+sgr_log[28,1]+sgr_log[29,1]+sgr_log[30,1]+sgr_log[31,1]
print(test)