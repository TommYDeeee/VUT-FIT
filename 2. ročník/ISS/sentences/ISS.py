import tkinter
from scipy.io import wavfile
from scipy.signal import spectrogram, lfilter, freqz, tf2zpk
from scipy.stats import pearsonr
import soundfile as sf
import matplotlib.pyplot as plt
import numpy as np

s, fs = sf.read('sa8.wav')
t = np.arange(s.size) / fs

f, t, sgr = spectrogram(s, fs,nperseg=400, noverlap=240, nfft= 511)
sgr_log = 10 * np.log10(sgr+1e-20) 
print(sgr_log.shape)


s, fs = sf.read('q5.wav')
t = np.arange(s.size) / fs

f, t, sgr1 = spectrogram(s, fs,nperseg=400, noverlap=240, nfft= 511)
sgr_log1 = 10 * np.log10(sgr1+1e-20)

matrix_sentence = np.zeros((16,343)) 
matrix_query1 = np.zeros((16,75)) 
print(sgr_log1.shape)
c = 0
for a in range (len(matrix_sentence[0])):
    for b in range (16): 
        for c in range (c,c+16,1):
            matrix_sentence[b,a] = matrix_sentence[b,a] + sgr_log[c,a]
        c = c + 1
    c = 0

c = 0
for a in range (len(matrix_query1[0])):
    for b in range (16): 
        for c in range (c,c+16,1):
            matrix_query1[b,a] = matrix_query1[b,a] + sgr_log1[c,a]
        c = c + 1
    c = 0

print(matrix_sentence)


len_final = np.size(matrix_sentence,1) - np.size(matrix_query1,1)
matrix_sentence = matrix_sentence.transpose()
matrix_query1 = matrix_query1.transpose()
finalmatrix = 0 
secondvalue = 0
scores = []
print(matrix_query1.shape)
for d in range (len_final):
    sum = 0
    for i in range (len(matrix_query1)):
        finalmatrix, secondvalue = pearsonr(matrix_query1[i], matrix_sentence[d + i])
        sum = sum + finalmatrix
    scores.append(sum / np.size(matrix_query1,0))
    
#print(sum)
print(scores)

plt.figure(figsize=(9,3))
axes = plt.gca()
axes.set_xlim([0,343])
axes.set_ylim([0,1])

plt.plot(scores)

plt.show()