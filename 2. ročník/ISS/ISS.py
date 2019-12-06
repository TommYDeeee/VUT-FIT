# scipy - potreba normalizace
from scipy.io import wavfile
fs, data = wavfile.read('sa1.wav')
data.min(), data.max()
