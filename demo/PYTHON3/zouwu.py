import os
import ctypes
from ctypes import CDLL



class ZouWu(object):
    def __init__(
            self,
            library_path):
        if not os.path.exists(library_path):
            raise IOError("Zouwu library path erro at '%s'" % library_path) 
        #      
        #self.zouwulib = CDLL(library_path)
        self.zouwulib = ctypes.cdll.LoadLibrary(library_path)
        # init 
        self.zouwu_pInst = ctypes.c_void_p(0)	
        self.pInst = ctypes.byref(self.zouwu_pInst)
        self.zouwulib.ZouwuInit(self.pInst)
        # load model 
        #self.zouwulib.ZouwuLoadModel(self.zouwu_pInst, model_path.encode())
        #set Param
        #c_sensitivity = ctypes.c_float(sensitivity)
        #pcmd=ctypes.c_char_p(b'sensitivity')
        #mdlid=ctypes.c_int(1)
        #self.zouwulib.ZouwuSetParam(self.zouwu_pInst,pcmd,ctypes.byref(c_sensitivity),mdlid)
    
    def release(self):
        self.zouwulib.ZouwuFinal(self.pInst)  

    def RegModel(self,data,datalen,id=1):
        
        mdlid   = ctypes.c_int(id)
        nSample = ctypes.c_int(datalen)
        self.zouwulib.ZouwuRegModel(self.zouwu_pInst, (ctypes.c_short * len(data))(*data), nSample, mdlid)    
            
    def LoadModel(self,model_path):
    
        if not os.path.exists(model_path):
            raise IOError("Zouwu model path erro at '%s'" % model_path)
        # load model 
        ret = self.zouwulib.ZouwuLoadModel(self.zouwu_pInst, model_path.encode())  
        if ret != 0:
            print("load model erro!")
            return 1
        return 0 
    def SetParam(self,sensitivity=0.3,mdl_id=1):
    
        c_sensitivity = ctypes.c_float(sensitivity)
        pcmd=ctypes.c_char_p(b'sensitivity')
        mdlid=ctypes.c_int(mdl_id)
        self.zouwulib.ZouwuSetParam(self.zouwu_pInst,pcmd,ctypes.byref(c_sensitivity),mdlid)
    
    def Proc(self,data):
    
        c_ret = ctypes.c_int(0)
        self.zouwulib.ZouwuProc(self.zouwu_pInst,(ctypes.c_short * len(data))(*data),ctypes.byref(c_ret))   
        return c_ret.value
    

        
