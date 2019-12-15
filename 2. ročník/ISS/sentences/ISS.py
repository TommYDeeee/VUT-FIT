import tkinter
from scipy.io import wavfile
from scipy.signal import spectrogram, lfilter, freqz, tf2zpk
from scipy.stats import pearsonr
import soundfile as sf
import matplotlib.pyplot as plt
import numpy as np

s, fs = sf.read('sa9.wav')


f, t, sgr = spectrogram(s, fs,nperseg=400, noverlap=240, nfft= 511)
sgr_log = 10 * np.log10(sgr+1e-20) 


s, fs = sf.read('q1.wav')

f1, t1, sgr1 = spectrogram(s, fs,nperseg=400, noverlap=240, nfft= 511)
sgr_log1 = 10 * np.log10(sgr1+1e-20)

s, fs = sf.read('q2.wav')


f2, t2, sgr2 = spectrogram(s, fs,nperseg=400, noverlap=240, nfft= 511)
sgr_log2 = 10 * np.log10(sgr2+1e-20)

matrix_sentence = np.zeros((16,len(sgr_log[0]))) 
matrix_query1 = np.zeros((16,len(sgr_log1[0])))
matrix_query2 = np.zeros((16,len(sgr_log2[0])))

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

c = 0
for a in range (len(matrix_query2[0])):
    for b in range (16): 
        for c in range (c,c+16,1):
            matrix_query2[b,a] = matrix_query2[b,a] + sgr_log2[c,a]
        c = c + 1
    c = 0


len_final = np.size(matrix_sentence,1) - np.size(matrix_query1,1)
len_final2 = np.size(matrix_sentence,1) - np.size(matrix_query2,1)
matrix_sentence = matrix_sentence.transpose()
matrix_query1 = matrix_query1.transpose()
matrix_query2 = matrix_query2.transpose()
finalmatrix = 0 
finalmatrix2 = 0
secondvalue = 0
scores = []
scores2 = []
for d in range (len_final):
    sum = 0
    for i in range (len(matrix_query1)):
        finalmatrix, secondvalue = pearsonr(matrix_query1[i], matrix_sentence[d + i])
        sum = sum + finalmatrix
    scores.append(sum / np.size(matrix_query1,0))

for d in range (len_final2):
    sum = 0
    for i in range (len(matrix_query2)):
        finalmatrix2, secondvalue = pearsonr(matrix_query2[i], matrix_sentence[d + i])
        sum = sum + finalmatrix2
    scores2.append(sum / np.size(matrix_query2,0))

fs, s_test = wavfile.read('sa9.wav')
i = 0
while i < len(scores):
    rating = scores[i]
    if rating > 0.6:
        print('hit')
        print(rating)
        hit = np.zeros(58*160)
        hit = np.asarray(hit, dtype=np.int16)
        for j in range (58 * 160):
            hit[j] = (s_test[(i*160) + j])
        i = i+ 58
        wavfile.write('q1hit_sx293.wav', 16000, hit)
    else:
        i = i + 1

i = 0
while i < len(scores2):
    rating = scores2[i]
    if rating > 0.66:
        print('hit')
        print(rating)
        hit = np.zeros(71*160)
        hit = np.asarray(hit, dtype=np.int16)
        for j in range (71 * 160):
            hit[j] = (s_test[(i*160) + j])
        i = i+ 71
        wavfile.write('q2hit_sx293.wav', 16000, hit)
    else:
        i = i + 1




_,axes = plt.subplots(3, sharex=True, figsize=(10,7))
_.suptitle('"Musicians" and "Beverages" vs ' + 'sx293.wav')



axes[2].plot(np.arange(len(scores))/100, scores, label = 'Musicians')
axes[2].plot(np.arange(len(scores2))/100, scores2, label = 'Beverages')
axes[2].legend()
axes[2].set_xlim([0,len(matrix_sentence)/100])
axes[2].set_ylim([0,1])
axes[2].set_ylabel('scores')
axes[2].set_xlabel('Čas [s]')
axes[2].grid(alpha=0.5, linestyle='--')


s, fs = sf.read('sa9.wav')
f, t, sgr = spectrogram(s, fs,nperseg=400, noverlap=240, nfft= 511)
f = range(17)
matrix_sentence = matrix_sentence.transpose()
axes[1].pcolormesh(t,f,matrix_sentence)
axes[1].set_ylabel('features')
axes[1].set_xlabel('Čas [s]')
axes[1].invert_yaxis()

axes[0].plot(np.arange(s.size) / 16000, s)
axes[0].set_xlabel('Čas [s]')
axes[0].set_ylabel('Signal')




plt.show()
