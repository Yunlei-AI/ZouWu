

import argparse
import os
import sys
import glob
#import soundfile as sf
import wave
import numpy as np
from datetime import datetime
from threading import Thread

from zouwu import ZouWu


def readWav(filename, mode):
    if not os.path.exists(filename):
        return
    else:
        fileHandle = wave.open(filename, mode)
        params = fileHandle.getparams()
        nchannels, sampwidth, samplerate, nsamples = params[:4]
        #print(nchannels, sampwidth, samplerate, nsamples)
        # read the data
        str_data = fileHandle.readframes(nsamples)
        fileHandle.close()

        wave_data = np.fromstring(str_data, dtype=np.short)
        if params[0] == 2:
            wave_data.shape = -1, 2
            wave_data = wave_data.T
        return wave_data


class ZouWuDemo(Thread):

    def __init__(
            self,
            library_path,
            data_path,
            model_path,
            sensitivities):

        super(ZouWuDemo, self).__init__()
        self._library_path = library_path
        self._data_path = data_path
        self._model_path = model_path
        self._sensitivities = sensitivities
        self._mdl_id = 1

    def rundemo(self):

        zouwu = None
        mdlid = 1

        try:
            zouwu = ZouWu(self._library_path)
            # three (16bit 16000  sample rate) wavs
            # load model
            zouwu.LoadModel(self._model_path)
            # set sensitivity
            zouwu.SetParam(self._sensitivities, self._mdl_id)

            #res, rate = sf.read(datafile, dtype='float32')
            res = readWav(self._data_path, 'rb')
            print("file:%s %d" % (self._data_path, len(res)))

            wav_chunk_size = 512
            nstep = 0
            while((nstep + wav_chunk_size) <= len(res)):
                tmp_data = res[nstep:nstep+wav_chunk_size]
                nstep = nstep + wav_chunk_size
                result = zouwu.Proc(tmp_data)
                if result > 0:
                    print('[%s] detected keyword %d %.4f' % (
                        str(datetime.now()), result, (nstep - wav_chunk_size)/16000))

        except KeyboardInterrupt:
            print('demo exiting ...')
        finally:
            if zouwu is not None:
                zouwu.release()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--library_path',
        '-l',
        help="Zouwu's dynamic library path",
        type=str,
        default=os.path.join(os.path.dirname(__file__), '../../lib/rpi/libZouwu.so'))

    parser.add_argument(
        '--data_path',
        '-d',
        help="test data path",
        type=str,
        default=os.path.join(os.path.dirname(__file__), '../../resources/testdata/test_xiaolei_2.wav'))
    parser.add_argument(
        '--model_path',
        '-m',
        help="load model path",
        type=str,
        default=os.path.join(os.path.dirname(__file__), '../../resources/model/1.mdl'))
    parser.add_argument('--sensitivities',
                        '-s',
                        help='detection sensitivity [0, 1]',
                        type=float,
                        default=0.3)
    args = parser.parse_args()

    ZouWuDemo(library_path=args.library_path,
              data_path=args.data_path,
              model_path=args.model_path,
              sensitivities=args.sensitivities).rundemo()
