import argparse
import struct
import os
import sys

import numpy as np
import pyaudio


from datetime import datetime
from threading import Thread
from zouwu import ZouWu


class ZouWuDemo(Thread):
                
    def __init__(
        self,
        library_path,
        model_path,
        sensitivities,
        mdl_id):
        
        
        super(ZouWuDemo, self).__init__()
        self._library_path = library_path
        self._model_path = model_path
        self._sensitivities = sensitivities
        self._mdl_id = mdl_id
        self._CHUNK = 512
        print("library_path :%s" % (library_path))
        print("model_path :%s" % (model_path))
        print("sensitivities :%s" % (sensitivities))
        
    def rundemo(self):
    
        zouwu = None
        p = None
        stream = None
        
        try:
            zouwu = ZouWu(self._library_path)

            
            p = pyaudio.PyAudio()
            stream = p.open(
                rate=16000,
                channels=1,
                format=pyaudio.paInt16,
                input=True,
                frames_per_buffer=2048)
            #load model
            zouwu.LoadModel(self._model_path)
            #set sensitivity
            zouwu.SetParam(self._sensitivities,self._mdl_id)
            
            while True:
                #pcm = stream.read(self._CHUNK,exception_on_overflow = False)
                pcm = stream.read(self._CHUNK)
                data = struct.unpack_from("h" * self._CHUNK, pcm)
                result = zouwu.Proc(data)
                if result  > 0:
                   print('[%s] detected keyword %d'  % (str(datetime.now()),result))
            
        except KeyboardInterrupt:
            print('demo exiting ...')
        finally:
            if zouwu is not None:
                zouwu.release()
            
            if stream is not None:
                stream.close()

            if p is not None:
                p.terminate()            

            
if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--library_path',
        help="Zouwu's dynamic library path",
        type=str,
        default=os.path.join(os.path.dirname(__file__), '../../lib/rpi/libZouwu.so'))

    parser.add_argument(
        '--model_path',
        help='model path',
        type=str,
        default=os.path.join(os.path.dirname(__file__), '../../resources/model/mdl'))

    parser.add_argument('--sensitivities', help='detection sensitivity [0, 1]', default=0.3)
    args = parser.parse_args()

    mdl_id = 1
    
    ZouWuDemo(
        library_path=args.library_path,
        model_path=args.model_path,
        sensitivities=args.sensitivities,
        mdl_id=mdl_id).rundemo()        
