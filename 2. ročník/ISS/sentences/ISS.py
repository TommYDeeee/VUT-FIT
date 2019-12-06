# scipy - potreba normalizace
from scipy.io import wavfile
from scipy.signal import spectrogram, lfilter, freqz, tf2zpk
import soundfile as sf
import matplotlib.pyplot as plt
import numpy as np

fs, data = wavfile.read('sa1.wav')
data.min(), data.max()
s, fs = sf.read('sa1.wav')
s = s[:250000]
t = np.arange(s.size) / fs

f, t, sgr = spectrogram(s, fs,nperseg=400, noverlap=140, nfft= 512)
sgr_log = 10 * np.log10(sgr+1e-20) 

"""plt.figure(figsize=(9, 3))
plt.pcolormesh(t, f, sgr_log)
plt.gca().set_xlabel('Čas [s]')
plt.gca().set_ylabel('Frekvence [Hz]')
cbar = plt.colorbar()
cbar.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)
plt.tight_layout()
plt.savefig('yourfile.pdf')"""
matrix = [[0]*1 for i in range(16)] 
print(sgr.shape)
print(matrix)